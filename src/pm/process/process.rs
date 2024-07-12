// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    hal::{
        arch::x86::cpu::context::ContextInformation,
        mem::VirtualAddress,
    },
    mm::{
        elf::Elf32Fhdr,
        KernelPage,
        VirtMemoryManager,
        Vmem,
    },
    pm::{
        process::identity::ProcessIdentity,
        thread::{
            InterruptedThread,
            ReadyThread,
            RunningThread,
            SleepingThread,
            ZombieThread,
        },
    },
};
use ::alloc::rc::Rc;
use core::cell::RefCell;
use kcall::{
    Error,
    ErrorCode,
    GroupIdentifier,
    ProcessIdentifier,
    ThreadIdentifier,
    UserIdentifier,
};

//==================================================================================================
// Process
//==================================================================================================

///
/// # Description
///
/// A type that represents the inner state of a process.
///
struct ProcessState {
    /// Process identifier.
    pid: ProcessIdentifier,
    /// Process identity.
    identity: ProcessIdentity,
    /// Running thread.
    running: Option<RunningThread>,
    /// Ready threads.
    ready: Option<ReadyThread>,
    /// Sleeping threads.
    sleeping: Option<SleepingThread>,
    /// List of interrupted threads.
    interrupted: Option<InterruptedThread>,
    /// List of zombie threads.
    zombies: Option<ZombieThread>,
    /// Memory address space.
    vmem: Vmem,
}

impl ProcessState {
    fn find_thread(&self, tid: ThreadIdentifier) -> Result<&SleepingThread, Error> {
        if let Some(ref sleeping) = self.sleeping {
            if sleeping.id() == tid {
                return Ok(sleeping);
            }
        }

        Err(Error::new(ErrorCode::NoSuchEntry, "thread not found"))
    }

    fn wakeup_sleeping_thread(&mut self, tid: ThreadIdentifier) -> Result<(), Error> {
        self.find_thread(tid)?;

        let thread = self.sleeping.take().unwrap();
        let thread = thread.wakeup();
        self.ready = Some(thread);

        Ok(())
    }
}

//==================================================================================================
// Running Process
//==================================================================================================

///
/// # Description
///
/// A type that represents a running process.
///
#[derive(Clone)]
pub struct RunningProcess(Rc<RefCell<ProcessState>>);

impl RunningProcess {
    pub fn schedule(self) -> (RunnableProcess, *mut ContextInformation) {
        let running_thread = self.0.borrow_mut().running.take().unwrap();
        let (ready_thread, ctx) = running_thread.schedule();
        self.0.borrow_mut().ready = Some(ready_thread);

        (RunnableProcess(self.0), ctx)
    }

    pub fn sleep(
        self,
    ) -> Result<
        (RunnableProcess, *mut ContextInformation),
        (SuspendedProcess, *mut ContextInformation),
    > {
        let running_thread = self.0.borrow_mut().running.take().unwrap();
        let (sleeping_thread, ctx) = running_thread.sleep();
        self.0.borrow_mut().sleeping = Some(sleeping_thread);

        if self.is_runnable() {
            return Ok((RunnableProcess(self.0), ctx));
        }

        Err((SuspendedProcess(self.0), ctx))
    }

    pub fn wakeup_sleeping_thread(&mut self, tid: ThreadIdentifier) -> Result<(), Error> {
        self.0.borrow_mut().wakeup_sleeping_thread(tid)
    }

    pub fn get_tid(&self) -> Option<ThreadIdentifier> {
        match self.0.borrow().running {
            Some(ref running) => Some(running.id()),
            None => None,
        }
    }

    ///
    /// # Description
    ///
    /// Gets the process identifier of the target process.
    ///
    /// # Returns
    ///
    /// The process identifier of the target process.
    ///
    pub fn pid(&self) -> ProcessIdentifier {
        self.0.borrow().pid
    }

    pub fn clone_vmem(&self, mm: &VirtMemoryManager) -> Result<Vmem, Error> {
        mm.new_vmem(&self.0.borrow().vmem)
    }

    ///
    /// # Description
    ///
    /// Clones the identity of the target process.
    ///
    /// # Return Values
    ///
    /// The cloned identity of the target process.
    ///
    pub fn clone_identity(&self) -> ProcessIdentity {
        self.0.borrow().identity.clone()
    }

    pub fn get_uid(&self) -> UserIdentifier {
        self.0.borrow().identity.get_uid()
    }

    pub fn set_uid(&mut self, uid: UserIdentifier) -> Result<(), Error> {
        self.0.borrow_mut().identity.set_uid(uid)
    }

    pub fn get_euid(&self) -> UserIdentifier {
        self.0.borrow().identity.get_euid()
    }

    pub fn set_euid(&mut self, euid: UserIdentifier) -> Result<(), Error> {
        self.0.borrow_mut().identity.set_euid(euid)
    }

    pub fn get_gid(&self) -> GroupIdentifier {
        self.0.borrow().identity.get_gid()
    }

    pub fn set_gid(&mut self, gid: GroupIdentifier) -> Result<(), Error> {
        self.0.borrow_mut().identity.set_gid(gid)
    }

    pub fn get_egid(&self) -> GroupIdentifier {
        self.0.borrow().identity.get_egid()
    }

    pub fn set_egid(&mut self, egid: GroupIdentifier) -> Result<(), Error> {
        self.0.borrow_mut().identity.set_egid(egid)
    }

    fn is_runnable(&self) -> bool {
        self.0.borrow().ready.is_some()
    }
}

//==================================================================================================
// Runnable Process
//==================================================================================================

pub struct RunnableProcess(Rc<RefCell<ProcessState>>);

impl RunnableProcess {
    ///
    /// # Description
    ///
    /// Instantiates a new runnable process.
    ///
    /// # Parameters
    ///
    /// - `pid`: Process identifier.
    /// - `identity`: Process identity.
    /// - `thread`: Running thread.
    /// - `vmem`: Virtual memory address space.
    ///
    /// # Return Values
    ///
    /// A new suspended process.
    ///
    pub fn new(
        pid: ProcessIdentifier,
        identity: ProcessIdentity,
        thread: ReadyThread,
        vmem: Vmem,
    ) -> Self {
        Self(Rc::new(RefCell::new(ProcessState {
            pid,
            identity,
            running: None,
            ready: Some(thread),
            sleeping: None,
            interrupted: None,
            zombies: None,
            vmem,
        })))
    }

    pub fn run(self) -> (RunningProcess, *mut ContextInformation) {
        let next_thread: ReadyThread = self.0.borrow_mut().ready.take().unwrap();
        let (next_thread, next_context) = next_thread.resume();
        self.0.borrow_mut().running = Some(next_thread);
        (RunningProcess(self.0), next_context)
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        elf: &Elf32Fhdr,
    ) -> Result<VirtualAddress, Error> {
        mm.load_elf(&mut self.0.borrow_mut().vmem, elf)
    }

    pub fn wakeup_sleeping_thread(&mut self, tid: ThreadIdentifier) -> Result<(), Error> {
        self.0.borrow_mut().wakeup_sleeping_thread(tid)
    }

    pub fn pid(&self) -> ProcessIdentifier {
        self.0.borrow().pid
    }
}

//==================================================================================================
// Suspended Process
//==================================================================================================

///
/// # Description
///
/// A type that represents a suspended process. A suspended process is a process that has all its
/// threads in either the ready or sleeping states.
///
pub struct SuspendedProcess(Rc<RefCell<ProcessState>>);

impl SuspendedProcess {
    pub fn wakeup_sleeping_thread(
        self,
        tid: ThreadIdentifier,
    ) -> Result<RunnableProcess, SuspendedProcess> {
        let found = self.0.borrow().find_thread(tid).is_ok();
        if found {
            self.0.borrow_mut().wakeup_sleeping_thread(tid).unwrap();
            Ok(RunnableProcess(self.0))
        } else {
            Err(SuspendedProcess(self.0))
        }
    }

    pub fn interrupt(self) -> InterruptedProcess {
        let interrupted_thread = self.0.borrow_mut().sleeping.take().unwrap();
        let interrupted_thread = interrupted_thread.interrupt();
        self.0.borrow_mut().interrupted = Some(interrupted_thread);
        InterruptedProcess(self.0)
    }

    pub fn pid(&self) -> ProcessIdentifier {
        self.0.borrow().pid
    }

    pub fn copy_from_user_unaligned(
        &self,
        dst: &mut KernelPage,
        src: VirtualAddress,
        size: usize,
    ) -> Result<(), Error> {
        self.0
            .borrow_mut()
            .vmem
            .copy_from_user_unaligned(dst, src, size)
    }
}

pub struct InterruptedProcess(Rc<RefCell<ProcessState>>);

impl InterruptedProcess {
    pub fn resume(self) -> (RunningProcess, *mut ContextInformation) {
        let thread = self.0.borrow_mut().interrupted.take().unwrap();
        let (thread, ctx) = thread.resume();
        self.0.borrow_mut().running = Some(thread);
        (RunningProcess(self.0), ctx)
    }
}

//==================================================================================================
// Zombie Process
//==================================================================================================

///
/// # Description
///
/// A type that represents a zombie process. A zombie process is a process that has finished its
/// execution and is waiting for its parent to collect its exit status and release its resources.
///
pub struct ZombieProcess {
    process: Rc<RefCell<ProcessState>>,
    status: i32,
}

impl ZombieProcess {
    pub fn pid(&self) -> ProcessIdentifier {
        self.process.borrow().pid
    }
}

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
            ReadyThread,
            RunningThread,
        },
    },
};
use alloc::{
    collections::LinkedList,
    rc::Rc,
};
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
pub struct ProcessInner {
    /// Process identifier.
    pid: ProcessIdentifier,
    /// Process identity.
    identity: ProcessIdentity,
    /// Running thread.
    running: Option<RunningThread>,
    /// Ready threads.
    ready: LinkedList<ReadyThread>,
    /// Sleeping threads.
    sleeping: LinkedList<ReadyThread>,
    /// Memory address space.
    vmem: Vmem,
}

///
/// # Description
///
/// A type that represents a process.
///
#[derive(Clone)]
struct Process(Rc<RefCell<ProcessInner>>);

impl Process {
    /// Initializes a new process.
    pub fn new(
        pid: ProcessIdentifier,
        identity: ProcessIdentity,
        thread: ReadyThread,
        vmem: Vmem,
    ) -> Self {
        let mut ready: LinkedList<ReadyThread> = LinkedList::new();
        ready.push_back(thread);
        Self(Rc::new(RefCell::new(ProcessInner {
            pid,
            identity,
            running: None,
            ready,
            sleeping: LinkedList::new(),
            vmem,
        })))
    }

    pub fn clone_vmem(&self, mm: &VirtMemoryManager) -> Result<Vmem, Error> {
        mm.new_vmem(&self.0.borrow().vmem)
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

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        elf: &Elf32Fhdr,
    ) -> Result<VirtualAddress, Error> {
        mm.load_elf(&mut self.0.borrow_mut().vmem, elf)
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

    pub fn wakeup_sleeping_thread(&mut self, tid: ThreadIdentifier) -> Result<(), Error> {
        let at: usize = match self
            .0
            .borrow()
            .sleeping
            .iter()
            .position(|thread| thread.id() == tid)
        {
            Some(thread) => thread,
            None => {
                let reason: &str = "thread not found";
                error!("wakeup_sleeping_thread(): {}", reason);
                return Err(Error::new(ErrorCode::NoSuchEntry, &reason));
            },
        };

        let thread: ReadyThread = self.0.borrow_mut().sleeping.remove(at);
        self.0.borrow_mut().ready.push_back(thread);

        Ok(())
    }

    pub fn schedule(&mut self) -> *mut ContextInformation {
        let previous_thread = self.0.borrow_mut().running.take().unwrap();
        let mut previous_thread: ReadyThread = previous_thread.suspend();
        let previous_context: *mut ContextInformation = previous_thread.context_mut();
        self.0.borrow_mut().ready.push_back(previous_thread);

        previous_context
    }

    pub fn sleep(&mut self) -> *mut ContextInformation {
        let previous_thread = self.0.borrow_mut().running.take().unwrap();
        let mut previous_thread: ReadyThread = previous_thread.suspend();
        let previous_context: *mut ContextInformation = previous_thread.context_mut();
        self.0.borrow_mut().sleeping.push_back(previous_thread);

        previous_context
    }

    pub fn run(&mut self) -> Option<*mut ContextInformation> {
        let mut next_thread: ReadyThread = match self.0.borrow_mut().ready.pop_front() {
            Some(next_thread) => next_thread,
            None => return None,
        };

        let next_context: *mut ContextInformation = next_thread.context_mut();
        let next_thread: RunningThread = next_thread.resume();
        self.0.borrow_mut().running = Some(next_thread);
        return Some(next_context);
    }

    ///
    /// # Description
    ///
    /// Gets the identity of the target process.
    ///
    /// # Return Values
    ///
    /// The identity of the target process.
    ///
    pub fn identity(&self) -> ProcessIdentity {
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
pub struct RunningProcess(Process);

impl RunningProcess {
    pub fn schedule(mut self) -> (SuspendedProcess, *mut ContextInformation) {
        let ctx: *mut ContextInformation = self.0.schedule();
        (SuspendedProcess(self.0), ctx)
    }

    pub fn sleep(mut self) -> (SuspendedProcess, *mut ContextInformation) {
        let ctx: *mut ContextInformation = self.0.sleep();
        (SuspendedProcess(self.0), ctx)
    }

    pub fn get_tid(&self) -> Option<ThreadIdentifier> {
        self.0.get_tid()
    }

    pub fn pid(&self) -> ProcessIdentifier {
        self.0.pid()
    }

    pub fn wakeup_sleeping_thread(&mut self, tid: ThreadIdentifier) -> Result<(), Error> {
        self.0.wakeup_sleeping_thread(tid)
    }

    pub fn clone_vmem(&self, mm: &VirtMemoryManager) -> Result<Vmem, Error> {
        self.0.clone_vmem(mm)
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
        self.0.identity()
    }

    pub fn get_uid(&self) -> UserIdentifier {
        self.0.get_uid()
    }

    pub fn set_uid(&mut self, uid: UserIdentifier) -> Result<(), Error> {
        self.0.set_uid(uid)
    }

    pub fn get_euid(&self) -> UserIdentifier {
        self.0.get_euid()
    }

    pub fn set_euid(&mut self, euid: UserIdentifier) -> Result<(), Error> {
        self.0.set_euid(euid)
    }

    pub fn get_gid(&self) -> GroupIdentifier {
        self.0.get_gid()
    }

    pub fn set_gid(&mut self, gid: GroupIdentifier) -> Result<(), Error> {
        self.0.set_gid(gid)
    }

    pub fn get_egid(&self) -> GroupIdentifier {
        self.0.get_egid()
    }

    pub fn set_egid(&mut self, egid: GroupIdentifier) -> Result<(), Error> {
        self.0.set_egid(egid)
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
#[derive(Clone)]
pub struct SuspendedProcess(Process);

impl SuspendedProcess {
    pub fn pid(&self) -> ProcessIdentifier {
        self.0.pid()
    }

    ///
    /// # Description
    ///
    /// Instantiates a new suspended process.
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
        SuspendedProcess(Process::new(pid, identity, thread, vmem))
    }

    pub fn run(mut self) -> Result<(RunningProcess, *mut ContextInformation), SuspendedProcess> {
        match self.0.run() {
            Some(context) => Ok((RunningProcess(self.0), context)),
            None => Err(self),
        }
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        elf: &Elf32Fhdr,
    ) -> Result<VirtualAddress, Error> {
        self.0.exec(mm, elf)
    }

    pub fn wakeup_sleeping_thread(&mut self, tid: ThreadIdentifier) -> Result<(), Error> {
        self.0.wakeup_sleeping_thread(tid)
    }

    pub fn copy_from_user_unaligned(
        &self,
        dst: &mut KernelPage,
        src: VirtualAddress,
        size: usize,
    ) -> Result<(), Error> {
        self.0.copy_from_user_unaligned(dst, src, size)
    }
}

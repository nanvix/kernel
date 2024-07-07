// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::mem,
    config,
    error::{
        Error,
        ErrorCode,
    },
    hal::{
        self,
        arch::x86::cpu::context::ContextInformation,
        mem::{
            AccessPermission,
            Address,
            PageAligned,
            VirtualAddress,
        },
    },
    mm::{
        self,
        elf::Elf32Fhdr,
        VirtMemoryManager,
        Vmem,
    },
    pm::{
        stack::Stack,
        thread::{
            ReadyThread,
            RunningThread,
            ThreadIdentifier,
            ThreadManager,
        },
    },
};
use alloc::{
    collections::LinkedList,
    rc::Rc,
};
use core::{
    cell::RefCell,
    fmt::Debug,
    ops::{
        Deref,
        DerefMut,
    },
};

//==================================================================================================
// Exports
//==================================================================================================

pub use kcall::ProcessIdentifier;

//==================================================================================================
// Process State
//==================================================================================================

///
/// # Description
///
/// A type that represents the state of a process.
///
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ProcessState {
    /// The process is ready to run.
    Ready,
    /// The process is running.
    Running,
}

//==================================================================================================
// Process
//==================================================================================================

pub struct ProcessInner {
    /// Identifier.
    id: ProcessIdentifier,
    /// State.
    state: ProcessState,
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
pub struct Process(Rc<ProcessInner>);

impl Process {
    /// Initializes a new process.
    pub fn new(id: ProcessIdentifier, thread: ReadyThread, vmem: Vmem) -> Self {
        let mut ready: LinkedList<ReadyThread> = LinkedList::new();
        ready.push_back(thread);
        Self(Rc::new(ProcessInner {
            id,
            state: ProcessState::Running,
            running: None,
            ready,
            sleeping: LinkedList::new(),
            vmem,
        }))
    }

    /// Gets the virtual memory address space of the target process.
    pub fn vmem(&self) -> &Vmem {
        &self.0.vmem
    }

    /// Gets the identifier of the process.
    pub fn id(&self) -> ProcessIdentifier {
        self.0.id
    }
}

impl Deref for Process {
    type Target = ProcessInner;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl DerefMut for Process {
    fn deref_mut(&mut self) -> &mut Self::Target {
        Rc::get_mut(&mut self.0).unwrap()
    }
}

//==================================================================================================
// Process Manager Inner
//==================================================================================================

///
/// # Description
///
/// A type that represents the process manager.
///
struct ProcessManagerInner {
    next_pid: ProcessIdentifier,
    /// Running process.
    running: Option<Process>,
    /// Ready processes.
    ready: LinkedList<Process>,
    /// Thread manager.
    tm: ThreadManager,
}

impl ProcessManagerInner {
    /// Initializes the process manager.
    pub fn new(kernel: ReadyThread, root: Vmem, tm: ThreadManager) -> Self {
        // The kernel process is special, handcraft it.
        let kernel: Process = Process(Rc::new(ProcessInner {
            state: ProcessState::Running,
            id: ProcessIdentifier::from(0),
            running: Some(kernel.resume()),
            sleeping: LinkedList::new(),
            ready: LinkedList::new(),
            vmem: root,
        }));

        Self {
            next_pid: ProcessIdentifier::from(1),
            ready: LinkedList::new(),
            running: Some(kernel),
            tm,
        }
    }

    fn forge_user_context(
        vmem: &Vmem,
        kstack: &Stack,
        user_stack: VirtualAddress,
        user_func: VirtualAddress,
        kernel_func: VirtualAddress,
    ) -> Result<ContextInformation, Error> {
        let cr3: u32 = vmem.pgdir().physical_address()?.into_raw_value() as u32;
        let esp: u32 = unsafe {
            hal::arch::forge_user_stack(
                kstack.top(),
                user_stack.into_raw_value(),
                user_func.into_raw_value(),
                kernel_func.into_raw_value(),
            )
        } as u32;
        let esp0: u32 = kstack.top() as u32;

        trace!("forge_context(): cr3={:#x}, esp={:#x}, ebp={:#x}", cr3, esp, esp0);
        Ok(ContextInformation::new(cr3, esp, esp0))
    }

    /// Creates a new thread.
    pub fn create_thread(
        &mut self,
        mm: &mut VirtMemoryManager,
        vmem: &Vmem,
        user_stack: VirtualAddress,
        user_func: VirtualAddress,
        kernel_func: VirtualAddress,
    ) -> Result<ReadyThread, Error> {
        let stack: Stack = Stack::new(mm)?;

        let context: ContextInformation =
            Self::forge_user_context(vmem, &stack, user_stack, user_func, kernel_func)?;

        self.tm.create_thread(context, stack)
    }

    /// Creates a new process.
    pub fn create_process(
        &mut self,
        mm: &mut VirtMemoryManager,
    ) -> Result<ProcessIdentifier, Error> {
        extern "C" {
            pub fn __leave_kernel_to_user_mode();
        }

        trace!("create_process()");
        let current: &Process = self.get_running();

        // Create a new memory address space for the process.
        let mut vmem: Vmem = mm.new_vmem(&current.vmem)?;

        let user_stack: VirtualAddress = mm::user_stack_top().into_inner();
        let user_func: VirtualAddress = mm::USER_BASE;
        let kernel_func: VirtualAddress =
            VirtualAddress::from_raw_value(__leave_kernel_to_user_mode as usize)?;
        let thread: ReadyThread =
            self.create_thread(mm, &vmem, user_stack, user_func, kernel_func)?;

        // Alloc user stack.
        let vaddr: PageAligned<VirtualAddress> = PageAligned::from_raw_value(
            mm::user_stack_top().into_raw_value() - config::KSTACK_SIZE - mem::PAGE_SIZE,
        )?;
        mm.alloc_upages(
            &mut vmem,
            vaddr,
            config::KSTACK_SIZE / mem::PAGE_SIZE,
            AccessPermission::RDWR,
        )?;

        let pid: ProcessIdentifier = self.next_pid;
        self.next_pid = ProcessIdentifier::from(Into::<usize>::into(pid) + 1);
        let process: Process = Process::new(pid, thread, vmem);

        self.ready.push_back(process);

        Ok(pid)
    }

    /// Schedule a process to run.
    pub fn schedule(&mut self) -> (*mut ContextInformation, *mut ContextInformation) {
        // Reschedule running process.
        let mut previous_process: Process = self.running.take().unwrap();
        previous_process.state = ProcessState::Ready;
        let previous_thread = previous_process.running.take().unwrap();
        let mut previous_thread: ReadyThread = previous_thread.suspend();
        let previous_context: *mut ContextInformation = previous_thread.context_mut();
        previous_process.ready.push_back(previous_thread);
        self.ready.push_back(previous_process);

        // Select next ready process to run.
        loop {
            let mut next_process: Process = self.ready.pop_front().unwrap();
            next_process.state = ProcessState::Running;
            if let Some(mut next_thread) = next_process.ready.pop_front() {
                let next_context: *mut ContextInformation = next_thread.context_mut();
                let next_thread: RunningThread = next_thread.resume();
                next_process.running = Some(next_thread);
                self.running = Some(next_process);
                return (previous_context, next_context);
            }

            self.ready.push_back(next_process);
        }
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        pid: ProcessIdentifier,
        elf: &Elf32Fhdr,
    ) -> Result<(), Error> {
        // Find corresponding process.
        let process: &mut Process = match self.ready.iter_mut().find(|p| p.id() == pid) {
            Some(p) => p,
            None => return Err(Error::new(ErrorCode::NoSuchProcess, "process not found")),
        };

        mm.load_elf(&mut process.vmem, elf)?;

        Ok(())
    }

    ///
    /// # Description
    ///
    /// Returns the ID of the calling process.
    ///
    /// # Returns
    ///
    /// The ID of the calling process.
    ///
    pub fn get_pid(&self) -> ProcessIdentifier {
        self.get_running().id()
    }

    ///
    /// # Description
    ///
    /// Returns the ID of the calling thread.
    ///
    /// # Returns
    ///
    /// The ID of the calling thread.
    ///
    pub fn get_tid(&self) -> ThreadIdentifier {
        self.get_running().running.as_ref().unwrap().id()
    }

    pub fn find_process(&self, pid: ProcessIdentifier) -> Result<&Process, Error> {
        // Search the list of ready processes.
        if let Some(process) = self.ready.iter().find(|p| p.id() == pid) {
            return Ok(process);
        }

        // Search the list of sleeping processes.
        if let Some(process) = self.ready.iter().find(|p| p.id() == pid) {
            return Ok(process);
        }

        let reason: &str = "process not found";
        error!("find_process(): {}", reason);
        Err(Error::new(ErrorCode::NoSuchEntry, reason))
    }

    ///
    /// # Description
    ///
    /// Puts the calling thread to sleep.
    ///
    /// # Returns
    ///
    /// Upon successful completion, empty is returned. Otherwise, an error code is returned instead.
    ///
    pub fn sleep(&mut self) -> (*mut ContextInformation, *mut ContextInformation) {
        // Safety: it is safe to call unwrap because there is always a process running.
        let mut previous_process: Process = self.running.take().unwrap();
        // Safety: it is safe to call unwrap because there is always a thread running.
        let thread: RunningThread = previous_process.running.take().unwrap();
        let mut thread: ReadyThread = thread.suspend();
        let previous_context: *mut ContextInformation = thread.context_mut();
        previous_process.sleeping.push_back(thread);

        // Reschedule.
        match previous_process.ready.pop_front() {
            Some(mut next_thread) => {
                let next_context: *mut ContextInformation = next_thread.context_mut();
                let next_thread: RunningThread = next_thread.resume();
                previous_process.running = Some(next_thread);
                self.running = Some(previous_process);
                (previous_context, next_context)
            },
            None => {
                previous_process.state = ProcessState::Ready;
                self.ready.push_back(previous_process);

                // Safety: it is safe to call unwrap because there is always a process ready to be run.
                let mut next_process: Process = self.ready.pop_front().unwrap();
                next_process.state = ProcessState::Running;
                // Safety: it is safe to call unwrap because there is always a thread ready to be run.
                let mut next_thread: ReadyThread = next_process.ready.pop_front().unwrap();
                let next_context: *mut ContextInformation = next_thread.context_mut();
                let next_thread: RunningThread = next_thread.resume();
                next_process.running = Some(next_thread);
                self.running = Some(next_process);
                (previous_context, next_context)
            },
        }
    }

    ///
    /// # Description
    ///
    /// Wakes up a thread.
    ///
    /// # Parameters
    ///
    /// - `tid`: ID of the thread to wake up.
    ///
    /// # Returns
    ///
    /// Upon successful completion, empty is returned. Otherwise, an error code is returned instead.
    ///
    pub fn wakeup(&mut self, tid: ThreadIdentifier) -> Result<(), Error> {
        // Check if thread belongs to running process.
        if let Some(thread) = self
            .get_running_mut()
            .sleeping
            .iter()
            .position(|t| t.id() == tid)
        {
            let thread: ReadyThread = self.get_running_mut().sleeping.remove(thread);
            self.get_running_mut().ready.push_back(thread);
            return Ok(());
        } else {
            // Check if thread belongs to ready processes.
            for process in self.ready.iter_mut() {
                if let Some(thread) = process.sleeping.iter().position(|t| t.id() == tid) {
                    let thread: ReadyThread = process.sleeping.remove(thread);
                    process.ready.push_back(thread);
                    return Ok(());
                }
            }
        }

        let reason: &str = "thread not found";
        error!("wake_up(): {}", reason);
        Err(Error::new(ErrorCode::NoSuchEntry, reason))
    }

    fn get_running(&self) -> &Process {
        // NOTE: The following call to unwrap is safe because there is always a running process.
        self.running.as_ref().unwrap()
    }

    ///
    /// # Description
    ///
    /// Returns a mutable reference to the running process.
    ///
    /// # Returns
    ///
    /// A mutable reference to the running process.
    ///
    fn get_running_mut(&mut self) -> &mut Process {
        // NOTE: The following call to unwrap is safe because there is always a running process.
        self.running.as_mut().unwrap()
    }
}

//==================================================================================================
// Process Manager
//==================================================================================================

pub struct ProcessManager(Rc<RefCell<ProcessManagerInner>>);

static mut PROCESS_MANAGER: Option<ProcessManager> = None;

impl ProcessManager {
    /// Creates a new process.
    pub fn create_process(&self, mm: &mut VirtMemoryManager) -> Result<ProcessIdentifier, Error> {
        match self.0.try_borrow_mut() {
            Ok(ref mut pm) => pm.create_process(mm),
            Err(_) => Err(Error::new(ErrorCode::ResourceBusy, "failed to borrow process manager")),
        }
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        pid: ProcessIdentifier,
        elf: &Elf32Fhdr,
    ) -> Result<(), Error> {
        match self.0.try_borrow_mut() {
            Ok(mut pm) => pm.exec(mm, pid, elf),
            Err(_) => Err(Error::new(ErrorCode::ResourceBusy, "cannot borrow process manager")),
        }
    }

    pub fn find_process(&self, pid: ProcessIdentifier) -> Result<Process, Error> {
        match self.0.try_borrow() {
            Ok(pm) => {
                let proc = pm.find_process(pid)?;
                Ok(proc.clone())
                // todo!();
            },
            Err(_) => Err(Error::new(ErrorCode::ResourceBusy, "cannot borrow process manager")),
        }
    }

    ///
    /// # Description
    ///
    /// Returns the ID of the calling process.
    ///
    /// # Returns
    ///
    /// Upon successful completion, the ID of the calling process is returned. Otherwise, an error
    /// code is returned instead.
    ///
    pub fn get_pid() -> Result<ProcessIdentifier, Error> {
        unsafe {
            match PROCESS_MANAGER {
                Some(ref pm) => match pm.0.try_borrow() {
                    Ok(pm) => Ok(pm.get_pid()),
                    Err(_) => {
                        Err(Error::new(ErrorCode::ResourceBusy, "cannot borrow process manager"))
                    },
                },
                None => Err(Error::new(ErrorCode::TryAgain, "process manager not initialized")),
            }
        }
    }

    ///
    /// # Description
    ///
    /// Returns the ID of the calling thread.
    ///
    /// # Returns
    ///
    /// Upon successful completion, the ID of the calling thread is returned. Otherwise, an error
    /// code is returned instead.
    ///
    pub fn get_tid() -> Result<ThreadIdentifier, Error> {
        unsafe {
            match PROCESS_MANAGER {
                Some(ref pm) => match pm.0.try_borrow() {
                    Ok(pm) => Ok(pm.get_tid()),
                    Err(_) => {
                        Err(Error::new(ErrorCode::ResourceBusy, "cannot borrow process manager"))
                    },
                },
                None => Err(Error::new(ErrorCode::TryAgain, "process manager not initialized")),
            }
        }
    }

    ///
    /// # Description
    ///
    /// Puts the calling thread to sleep.
    ///
    /// # Returns
    ///
    /// Upon successful completion, empty is returned. Otherwise, an error code is returned instead.
    ///
    pub fn sleep() -> Result<(), Error> {
        let (from, to): (*mut ContextInformation, *mut ContextInformation) = unsafe {
            match PROCESS_MANAGER {
                Some(ref pm) => match pm.0.try_borrow_mut() {
                    Ok(mut pm) => pm.sleep(),
                    Err(_) => {
                        let reason: &str = "cannot borrow process manager";
                        error!("sleep(): {}", reason);
                        return Err(Error::new(ErrorCode::ResourceBusy, reason));
                    },
                },
                None => {
                    let reason: &str = "process manager not initialized";
                    error!("sleep(): {}", reason);
                    return Err(Error::new(ErrorCode::TryAgain, reason));
                },
            }
        };

        unsafe { ContextInformation::switch(from, to) }

        Ok(())
    }

    ///
    /// # Description
    ///
    /// Wakes up a thread.
    ///
    /// # Parameters
    ///
    /// - `tid`: ID of the thread to wake up.
    ///
    /// # Returns
    ///
    /// Upon successful completion, empty is returned. Otherwise, an error code is returned instead.
    ///
    pub fn wakeup(tid: ThreadIdentifier) -> Result<(), Error> {
        unsafe {
            match PROCESS_MANAGER {
                Some(ref pm) => match pm.0.try_borrow_mut() {
                    Ok(mut pm) => pm.wakeup(tid),
                    Err(_) => {
                        let reason: &str = "cannot borrow process manager";
                        error!("sleep(): {}", reason);
                        Err(Error::new(ErrorCode::ResourceBusy, reason))
                    },
                },
                None => {
                    let reason: &str = "process manager not initialized";
                    error!("sleep(): {}", reason);
                    Err(Error::new(ErrorCode::TryAgain, reason))
                },
            }
        }
    }

    pub fn switch() -> Result<(), Error> {
        let (from, to): (*mut ContextInformation, *mut ContextInformation) = unsafe {
            match PROCESS_MANAGER {
                Some(ref pm) => match pm.0.try_borrow_mut() {
                    Ok(mut pm) => pm.schedule(),
                    Err(_) => {
                        return Err(Error::new(
                            ErrorCode::ResourceBusy,
                            "cannot borrow process manager",
                        ))
                    },
                },
                None => {
                    return Err(Error::new(ErrorCode::TryAgain, "process manager not initialized"))
                },
            }
        };

        unsafe { ContextInformation::switch(from, to) }

        Ok(())
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

/// Initializes the process manager.
pub fn init(kernel: ReadyThread, root: Vmem, tm: ThreadManager) -> ProcessManager {
    // TODO: check for double initialization.

    let pm: Rc<RefCell<ProcessManagerInner>> =
        Rc::new(RefCell::new(ProcessManagerInner::new(kernel, root, tm)));

    unsafe { PROCESS_MANAGER = Some(ProcessManager(pm.clone())) };

    ProcessManager(pm)
}

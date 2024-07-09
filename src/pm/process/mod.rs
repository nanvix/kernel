// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod process;

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
            ThreadManager,
        },
    },
};
use ::alloc::{
    collections::LinkedList,
    rc::Rc,
};
use ::core::cell::RefCell;
use ::kcall::{
    ProcessIdentifier,
    ThreadIdentifier,
};

//==================================================================================================
// Exports
//==================================================================================================

pub use process::{
    RunningProcess,
    SuspendedProcess,
};

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
    running: Option<RunningProcess>,
    /// Ready processes.
    ready: LinkedList<SuspendedProcess>,
    /// Thread manager.
    tm: ThreadManager,
}

impl ProcessManagerInner {
    /// Initializes the process manager.
    pub fn new(kernel: ReadyThread, root: Vmem, tm: ThreadManager) -> Self {
        let kernel: SuspendedProcess =
            SuspendedProcess::new(ProcessIdentifier::from(0), kernel, root);

        let kernel: RunningProcess = match kernel.run() {
            Ok((kernel, _ctx)) => kernel,
            Err(_kernel) => {
                unreachable!("failed to run kernel thread");
            },
        };

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
        from: RunningProcess,
        mm: &mut VirtMemoryManager,
    ) -> Result<ProcessIdentifier, Error> {
        extern "C" {
            pub fn __leave_kernel_to_user_mode();
        }

        trace!("create_process()");

        // Create a new memory address space for the process.
        let mut vmem: Vmem = from.clone_vmem(mm)?;

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
        let process: SuspendedProcess = SuspendedProcess::new(pid, thread, vmem);

        self.ready.push_back(process);

        Ok(pid)
    }

    /// Schedule a process to run.
    pub fn schedule(&mut self) -> (*mut ContextInformation, *mut ContextInformation) {
        // Reschedule running process.
        let previous_process: RunningProcess = self.running.take().unwrap();
        let (previous_process, previous_context): (SuspendedProcess, *mut ContextInformation) =
            previous_process.schedule();
        self.ready.push_back(previous_process);

        // Select next ready process to run.
        loop {
            let next_process: SuspendedProcess = self.ready.pop_front().unwrap();
            match next_process.run() {
                Ok((next_process, next_context)) => {
                    self.running = Some(next_process);
                    return (previous_context, next_context);
                },
                Err(next_process) => {
                    self.ready.push_back(next_process);
                },
            }
        }
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        pid: ProcessIdentifier,
        elf: &Elf32Fhdr,
    ) -> Result<(), Error> {
        // Find corresponding process.
        let process: &mut SuspendedProcess = match self.ready.iter_mut().find(|p| p.pid() == pid) {
            Some(p) => p,
            None => {
                let reason: &str = "process not found";
                error!("exec(): {}", reason);
                return Err(Error::new(ErrorCode::NoSuchProcess, reason));
            },
        };

        process.exec(mm, elf)?;

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
        self.get_running().pid()
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
        self.get_running().get_tid().unwrap()
    }

    pub fn find_suspended_process(&self, pid: ProcessIdentifier) -> Option<&SuspendedProcess> {
        self.ready.iter().find(|p| p.pid() == pid)
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
        let running_process: RunningProcess = self.running.take().unwrap();

        let (suspended_process, previous_context): (SuspendedProcess, *mut ContextInformation) =
            running_process.sleep();

        match suspended_process.run() {
            Ok((next_process, next_context)) => {
                self.running = Some(next_process);
                (previous_context, next_context)
            },
            Err(suspended_process) => {
                self.ready.push_back(suspended_process);
                // Safety: it is safe to call unwrap because there is always a process ready to be run.
                let suspended_process: SuspendedProcess = self.ready.pop_front().unwrap();

                // Safety: it is safe to call unwrap because there is always a thread ready to be run.
                let (next_process, next_context): (RunningProcess, *mut ContextInformation) =
                    match suspended_process.run() {
                        Ok((next_process, next_context)) => (next_process, next_context),
                        Err(_) => {
                            unreachable!("there should exist a runnable thread")
                        },
                    };

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
        // Check if thread belongs to a ready processes.
        for process in self.ready.iter_mut() {
            if process.wakeup_sleeping_thread(tid).is_ok() {
                return Ok(());
            }
        }

        // Check if thread belongs to the running process.
        if self.get_running_mut().wakeup_sleeping_thread(tid).is_ok() {
            return Ok(());
        }

        let reason: &str = "thread not found";
        error!("wake_up(): {}", reason);
        Err(Error::new(ErrorCode::NoSuchEntry, reason))
    }

    fn get_running(&self) -> &RunningProcess {
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
    fn get_running_mut(&mut self) -> &mut RunningProcess {
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
    pub fn create_process(
        &self,
        from: RunningProcess,
        mm: &mut VirtMemoryManager,
    ) -> Result<ProcessIdentifier, Error> {
        match self.0.try_borrow_mut() {
            Ok(ref mut pm) => pm.create_process(from, mm),
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

    pub fn get_running(&self) -> Result<RunningProcess, Error> {
        match self.0.try_borrow() {
            Ok(pm) => Ok(pm.get_running().clone()),
            Err(_) => Err(Error::new(ErrorCode::ResourceBusy, "cannot borrow process manager")),
        }
    }

    pub fn find_suspended_process(
        &self,
        pid: ProcessIdentifier,
    ) -> Result<SuspendedProcess, Error> {
        match self.0.try_borrow() {
            Ok(pm) => {
                if let Some(proc) = pm.find_suspended_process(pid) {
                    Ok(proc.clone())
                } else {
                    let reason: &str = "ready process not found";
                    error!("find_ready_process(): {}", reason);
                    Err(Error::new(ErrorCode::NoSuchEntry, &reason))
                }
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

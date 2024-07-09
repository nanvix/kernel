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

pub use process::Process;

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
        let mut kernel: Process = Process::new(ProcessIdentifier::from(0), kernel, root);

        let _ = kernel.run();

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
        from: Process,
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
        let process: Process = Process::new(pid, thread, vmem);

        self.ready.push_back(process);

        Ok(pid)
    }

    /// Schedule a process to run.
    pub fn schedule(&mut self) -> (*mut ContextInformation, *mut ContextInformation) {
        // Reschedule running process.
        let mut previous_process: Process = self.running.take().unwrap();
        let previous_context: *mut ContextInformation = previous_process.schedule();
        self.ready.push_back(previous_process);

        // Select next ready process to run.
        loop {
            let mut next_process: Process = self.ready.pop_front().unwrap();
            if let Some(next_context) = next_process.run() {
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
        let process: &mut Process = match self.ready.iter_mut().find(|p| p.pid() == pid) {
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

    pub fn find_process(&self, pid: ProcessIdentifier) -> Result<&Process, Error> {
        trace!("find_process(): pid={:?}", pid);

        // Search the list of ready processes.
        if let Some(process) = self.ready.iter().find(|p| p.pid() == pid) {
            return Ok(process);
        }

        // Search the list of sleeping processes.
        if let Some(process) = self.ready.iter().find(|p| p.pid() == pid) {
            return Ok(process);
        }

        // Check if the process is the running process.
        if let Some(process) = self.running.as_ref() {
            if process.pid() == pid {
                return Ok(process);
            }
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
        let previous_context: *mut ContextInformation = previous_process.sleep();

        match previous_process.run() {
            Some(next_context) => (previous_context, next_context),
            None => {
                self.ready.push_back(previous_process);

                // Safety: it is safe to call unwrap because there is always a process ready to be run.
                let mut next_process: Process = self.ready.pop_front().unwrap();
                // Safety: it is safe to call unwrap because there is always a thread ready to be run.
                let next_context: *mut ContextInformation = next_process.run().unwrap();
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
        if self.get_running_mut().wakeup_sleeping_thread(tid).is_ok() {
            return Ok(());
        }

        // Check if thread belongs to ready processes.
        for process in self.ready.iter_mut() {
            if process.wakeup_sleeping_thread(tid).is_ok() {
                return Ok(());
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
    pub fn create_process(
        &self,
        from: Process,
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

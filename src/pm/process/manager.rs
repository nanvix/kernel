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
        arch::ContextInformation,
        mem::{
            AccessPermission,
            Address,
            PageAddress,
            PageAligned,
            VirtualAddress,
        },
    },
    mm::{
        self,
        elf::Elf32Fhdr,
        KernelPage,
        VirtMemoryManager,
        Vmem,
    },
    pm::{
        process::{
            identity::ProcessIdentity,
            process::{
                InterruptedProcess,
                RunnableProcess,
                RunningProcess,
                SleepingProcess,
                ZombieProcess,
            },
        },
        thread::{
            ReadyThread,
            ThreadManager,
        },
    },
};
use ::alloc::{
    collections::LinkedList,
    rc::Rc,
    vec::Vec,
};
use ::core::cell::{
    Ref,
    RefCell,
    RefMut,
};
use ::kcall::{
    Capability,
    GroupIdentifier,
    ProcessIdentifier,
    ThreadIdentifier,
    UserIdentifier,
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
    /// Was the process interrupted?
    interrupted: bool,
    /// Next process identifier.
    next_pid: ProcessIdentifier,
    /// Running process.
    running: Option<RunningProcess>,
    /// Ready processes.
    ready: LinkedList<RunnableProcess>,
    /// Suspended processes.
    suspended: LinkedList<SleepingProcess>,
    /// Zombie processes.
    zombies: LinkedList<ZombieProcess>,
    /// Thread manager.
    tm: ThreadManager,
}

impl ProcessManagerInner {
    /// Initializes the process manager.
    pub fn new(kernel: ReadyThread, root: Vmem, tm: ThreadManager) -> Self {
        let kernel: RunnableProcess = RunnableProcess::new(
            ProcessIdentifier::from(0),
            ProcessIdentity::new(UserIdentifier::ROOT, GroupIdentifier::ROOT),
            kernel,
            root,
        );

        let (kernel, _): (RunningProcess, *mut ContextInformation) = kernel.run();

        Self {
            interrupted: false,
            next_pid: ProcessIdentifier::from(1),
            ready: LinkedList::new(),
            suspended: LinkedList::new(),
            zombies: LinkedList::new(),
            running: Some(kernel),
            tm,
        }
    }

    fn forge_user_context(
        vmem: &Vmem,
        user_stack: VirtualAddress,
        user_func: VirtualAddress,
        kernel_func: VirtualAddress,
        kernel_stack: VirtualAddress,
    ) -> Result<ContextInformation, Error> {
        let cr3: u32 = vmem.pgdir().physical_address()?.into_raw_value() as u32;
        let esp: u32 = unsafe {
            hal::arch::forge_user_stack(
                kernel_stack.into_raw_value() as *mut u8,
                user_stack.into_raw_value(),
                user_func.into_raw_value(),
                kernel_func.into_raw_value(),
            )
        } as u32;
        let esp0: u32 = kernel_stack.into_raw_value() as u32;

        trace!("forge_context(): cr3={:#x}, esp={:#x}, ebp={:#x}", cr3, esp, esp0);
        Ok(ContextInformation::new(cr3, esp, esp0))
    }

    /// Creates a new thread.
    pub fn create_thread(
        &mut self,
        mm: &mut VirtMemoryManager,
        vmem: &mut Vmem,
        user_stack: VirtualAddress,
        user_func: VirtualAddress,
        kernel_func: VirtualAddress,
    ) -> Result<ReadyThread, Error> {
        let mut kpages: Vec<KernelPage> =
            mm.alloc_kpages(true, config::KSTACK_SIZE / mem::PAGE_SIZE)?;

        let base: PageAddress = kpages[0].base();
        let size: usize = config::KSTACK_SIZE;
        let top = unsafe { (base.into_raw_value() as *mut u8).add(size) };
        let kernel_stack = VirtualAddress::from_raw_value(top as usize)?;

        let context: ContextInformation =
            Self::forge_user_context(vmem, user_stack, user_func, kernel_func, kernel_stack)?;

        while let Some(kpage) = kpages.pop() {
            vmem.add_private_kernel_page(kpage);
        }

        self.tm.create_thread(context)
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

        // Create a new memory address space for the process.
        let mut vmem: Vmem = self.get_running().clone_vmem(mm)?;

        let user_stack: VirtualAddress = mm::user_stack_top().into_inner();
        let user_func: VirtualAddress = mm::USER_BASE;
        let kernel_func: VirtualAddress =
            VirtualAddress::from_raw_value(__leave_kernel_to_user_mode as usize)?;
        let thread: ReadyThread =
            self.create_thread(mm, &mut vmem, user_stack, user_func, kernel_func)?;

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
        let identity: ProcessIdentity = self.get_running().clone_identity();
        let process: RunnableProcess = RunnableProcess::new(pid, identity, thread, vmem);

        self.ready.push_back(process);

        Ok(pid)
    }

    /// Schedule a process to run.
    pub fn schedule(&mut self) -> (*mut ContextInformation, *mut ContextInformation) {
        // Reschedule running process.
        let previous_process: RunningProcess = self.running.take().unwrap();
        let (previous_process, previous_context) = previous_process.schedule();
        self.ready.push_back(previous_process);

        // Select next ready process to run.
        let next_process: RunnableProcess = self.ready.pop_front().unwrap();
        let (next_process, next_context): (RunningProcess, *mut ContextInformation) =
            next_process.run();

        self.running = Some(next_process);
        (previous_context, next_context)
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        pid: ProcessIdentifier,
        elf: &Elf32Fhdr,
    ) -> Result<(), Error> {
        // Find corresponding process.
        let process: &mut RunnableProcess = match self.ready.iter_mut().find(|p| p.pid() == pid) {
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

    pub fn find_suspended_process(&self, pid: ProcessIdentifier) -> Option<&SleepingProcess> {
        self.suspended.iter().find(|p| p.pid() == pid)
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

        match running_process.sleep() {
            Ok((runnable_process, previous_context)) => {
                let (next_process, next_context) = runnable_process.run();
                self.running = Some(next_process);
                (previous_context, next_context)
            },
            Err((suspended_process, previous_context)) => {
                self.suspended.push_back(suspended_process);
                let next_process: RunnableProcess = self.ready.pop_front().unwrap();
                let (next_process, next_context) = next_process.run();
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
        let mut suspended: LinkedList<SleepingProcess> = LinkedList::new();
        while let Some(process) = self.suspended.pop_front() {
            match process.wakeup_sleeping_thread(tid) {
                Ok(runnable_process) => {
                    self.ready.push_back(runnable_process);
                    while let Some(process) = suspended.pop_front() {
                        self.suspended.push_back(process);
                    }
                    return Ok(());
                },
                Err(suspended_process) => suspended.push_back(suspended_process),
            }
        }

        let reason: &str = "thread not found";
        error!("wake_up(): {}", reason);
        Err(Error::new(ErrorCode::NoSuchEntry, reason))
    }

    pub fn exit(
        &mut self,
        status: i32,
    ) -> Result<(*mut ContextInformation, *mut ContextInformation), Error> {
        // Safety: it is safe to call unwrap because there is always a process running.
        let running_process: RunningProcess = self.running.take().unwrap();

        match running_process.exit(status) {
            Ok((runnable_process, previous_context)) => {
                let (running_process, next_context) = runnable_process.run();
                self.running = Some(running_process);
                Ok((previous_context, next_context))
            },
            Err((zombie_process, previous_context)) => {
                self.zombies.push_back(zombie_process);

                match self.ready.pop_front() {
                    Some(runnable_process) => {
                        let (running_process, next_context) = runnable_process.run();
                        self.running = Some(running_process);
                        Ok((previous_context, next_context))
                    },
                    None => {
                        if self.suspended.len() == 1 {
                            let suspended_process: SleepingProcess =
                                self.suspended.pop_front().unwrap();
                            let interrupted_process: InterruptedProcess =
                                suspended_process.interrupt();
                            let (next_process, next_context) = interrupted_process.resume();
                            self.interrupted = true;
                            self.running = Some(next_process);
                            return Ok((previous_context, next_context));
                        }
                        trace!("number of suspended: {:?}", self.suspended.len());
                        trace!("number of zombies: {:?}", self.zombies.len());
                        todo!();
                    },
                }
            },
        }
    }

    fn get_running(&self) -> &RunningProcess {
        // NOTE: The following call to unwrap is safe because there is always a running process.
        self.running.as_ref().unwrap()
    }

    pub fn capctl(
        &mut self,
        pid: ProcessIdentifier,
        capability: Capability,
        value: bool,
    ) -> Result<(), Error> {
        let process: &mut RunnableProcess = self.find_runnable_process_mut(pid)?;

        if value {
            process.set_capability(capability);
        } else {
            process.clear_capability(capability);
        }

        Ok(())
    }

    fn interrupt_flag(&mut self) -> bool {
        let interrupted: bool = self.interrupted;
        self.interrupted = false;
        interrupted
    }

    pub fn harvest_zombies(&mut self) {
        while let Some(mut zombie) = self.zombies.pop_front() {
            let (thread, state, status) = zombie.bury();
            trace!(
                "haversint resources (pid={:?}, tid={:?}, status={:?})",
                state.pid(),
                thread.id(),
                status
            );
        }
    }

    fn find_runnable_process(&self, pid: ProcessIdentifier) -> Result<&RunnableProcess, Error> {
        match self.ready.iter().find(|p| p.pid() == pid) {
            Some(p) => Ok(p),
            None => {
                let reason: &str = "process not found";
                error!("find_runnable_process(): {}", reason);
                Err(Error::new(ErrorCode::NoSuchProcess, reason))
            },
        }
    }

    fn find_runnable_process_mut(
        &mut self,
        pid: ProcessIdentifier,
    ) -> Result<&mut RunnableProcess, Error> {
        match self.ready.iter_mut().find(|p| p.pid() == pid) {
            Some(p) => Ok(p),
            None => {
                let reason: &str = "process not found";
                error!("find_runnable_process(): {}", reason);
                Err(Error::new(ErrorCode::NoSuchProcess, reason))
            },
        }
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
        &mut self,
        mm: &mut VirtMemoryManager,
    ) -> Result<ProcessIdentifier, Error> {
        Ok(self.try_borrow_mut()?.create_process(mm)?)
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        pid: ProcessIdentifier,
        elf: &Elf32Fhdr,
    ) -> Result<(), Error> {
        Ok(self.try_borrow_mut()?.exec(mm, pid, elf)?)
    }

    pub fn getuid(&self, pid: ProcessIdentifier) -> Result<UserIdentifier, Error> {
        Ok(self.try_borrow()?.find_runnable_process(pid)?.get_uid())
    }

    pub fn setuid(&mut self, pid: ProcessIdentifier, uid: UserIdentifier) -> Result<(), Error> {
        Ok(self
            .try_borrow_mut()?
            .find_runnable_process_mut(pid)?
            .set_uid(uid)?)
    }

    pub fn geteuid(&self, pid: ProcessIdentifier) -> Result<UserIdentifier, Error> {
        Ok(self.try_borrow()?.find_runnable_process(pid)?.get_euid())
    }

    pub fn seteuid(&mut self, pid: ProcessIdentifier, euid: UserIdentifier) -> Result<(), Error> {
        Ok(self
            .try_borrow_mut()?
            .find_runnable_process_mut(pid)?
            .set_euid(euid)?)
    }

    pub fn getgid(&self, pid: ProcessIdentifier) -> Result<GroupIdentifier, Error> {
        Ok(self.try_borrow()?.find_runnable_process(pid)?.get_gid())
    }

    pub fn setgid(&mut self, pid: ProcessIdentifier, gid: GroupIdentifier) -> Result<(), Error> {
        Ok(self
            .try_borrow_mut()?
            .find_runnable_process_mut(pid)?
            .set_gid(gid)?)
    }

    pub fn getegid(&self, pid: ProcessIdentifier) -> Result<GroupIdentifier, Error> {
        Ok(self.try_borrow()?.find_runnable_process(pid)?.get_egid())
    }

    pub fn setegid(&mut self, pid: ProcessIdentifier, egid: GroupIdentifier) -> Result<(), Error> {
        Ok(self
            .try_borrow_mut()?
            .find_runnable_process_mut(pid)?
            .set_egid(egid)?)
    }

    pub fn capctl(
        &mut self,
        pid: ProcessIdentifier,
        capability: Capability,
        value: bool,
    ) -> Result<(), Error> {
        Ok(self.try_borrow_mut()?.capctl(pid, capability, value)?)
    }

    pub fn exit(status: i32) -> Result<!, Error> {
        trace!("exit({:?})", status);
        let (from, to): (*mut ContextInformation, *mut ContextInformation) =
            Self::get_mut()?.try_borrow_mut()?.exit(status)?;

        unsafe {
            ContextInformation::switch(from, to);
            core::hint::unreachable_unchecked()
        }
    }

    pub fn vmcopy_from_user(
        &mut self,
        pid: ProcessIdentifier,
        dst: &mut KernelPage,
        src: VirtualAddress,
        size: usize,
    ) -> Result<(), Error> {
        match self.0.try_borrow() {
            Ok(pm) => {
                if let Some(proc) = pm.find_suspended_process(pid) {
                    proc.copy_from_user_unaligned(dst, src, size)
                } else {
                    let reason: &str = "process not found";
                    error!("vmcopy_from_user(): {}", reason);
                    Err(Error::new(ErrorCode::NoSuchEntry, &reason))
                }
            },
            Err(_) => {
                let reason: &str = "failed to borrow process manager";
                error!("vmcopy_from_user(): {}", reason);
                Err(Error::new(ErrorCode::ResourceBusy, "cannot borrow process manager"))
            },
        }
    }

    pub fn harvest_zombies(&mut self) {
        self.try_borrow_mut().unwrap().harvest_zombies();
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
        Ok(Self::get()?.try_borrow()?.get_pid())
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
        Ok(Self::get()?.try_borrow()?.get_tid())
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
        let (from, to): (*mut ContextInformation, *mut ContextInformation) =
            Self::get_mut()?.try_borrow_mut()?.sleep();

        unsafe { ContextInformation::switch(from, to) }

        let interrupted_flag: bool = Self::get_mut()?.try_borrow_mut()?.interrupt_flag();

        if interrupted_flag {
            let reason: &str = "interrupted";
            error!("sleep(): {}", reason);
            return Err(Error::new(ErrorCode::Interrupted, reason));
        }

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
        Self::get_mut()?.try_borrow_mut()?.wakeup(tid)
    }

    pub fn switch() -> Result<(), Error> {
        let (from, to): (*mut ContextInformation, *mut ContextInformation) =
            { Self::get_mut()?.try_borrow_mut()?.schedule() };

        unsafe { ContextInformation::switch(from, to) }

        Ok(())
    }

    fn try_borrow(&self) -> Result<Ref<ProcessManagerInner>, Error> {
        match self.0.try_borrow() {
            Ok(pm) => Ok(pm),
            Err(_) => {
                let reason: &str = "cannot borrow process manager";
                error!("try_borrow(): {}", reason);
                Err(Error::new(ErrorCode::ResourceBusy, reason))
            },
        }
    }

    fn try_borrow_mut(&mut self) -> Result<RefMut<ProcessManagerInner>, Error> {
        match self.0.try_borrow_mut() {
            Ok(pm) => Ok(pm),
            Err(_) => {
                let reason: &str = "cannot borrow process manager";
                error!("try_borrow_mut(): {}", reason);
                Err(Error::new(ErrorCode::ResourceBusy, reason))
            },
        }
    }

    fn get() -> Result<&'static ProcessManager, Error> {
        unsafe {
            match PROCESS_MANAGER {
                Some(ref pm) => Ok(pm),
                None => {
                    let reason: &str = "process manager not initialized";
                    error!("get(): {}", reason);
                    Err(Error::new(ErrorCode::TryAgain, reason))
                },
            }
        }
    }

    fn get_mut() -> Result<&'static mut ProcessManager, Error> {
        unsafe {
            match PROCESS_MANAGER {
                Some(ref mut pm) => Ok(pm),
                None => {
                    let reason: &str = "process manager not initialized";
                    error!("get_mut(): {}", reason);
                    Err(Error::new(ErrorCode::TryAgain, reason))
                },
            }
        }
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

// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

use crate::{
    hal::arch::x86::cpu::context::ContextInformation,
    mm::{
        VirtMemoryManager,
        Vmem,
    },
    pm::{
        process::{
            identity::ProcessIdentity,
            process::state::ProcessState,
        },
        thread::RunningThread,
    },
};
use ::alloc::rc::Rc;
use ::core::cell::RefCell;
use ::kcall::{
    Error,
    GroupIdentifier,
    ProcessIdentifier,
    ThreadIdentifier,
    UserIdentifier,
};

use super::{
    suspended::SleepingProcess,
    RunnableProcess,
    ZombieProcess,
};

///
/// # Description
///
/// A type that represents a running process.
///
pub struct RunningProcess {
    state: Option<ProcessState>,
    /// Running thread.
    running: Rc<RefCell<Option<RunningThread>>>,
}

impl RunningProcess {
    pub fn from_state(process: ProcessState, running: RunningThread) -> Self {
        Self {
            state: Some(process),
            running: Rc::new(RefCell::new(Some(running))),
        }
    }

    pub fn schedule(mut self) -> (RunnableProcess, *mut ContextInformation) {
        let running_thread = self.running.borrow_mut().take().unwrap();
        let (ready_thread, ctx) = running_thread.schedule();

        (RunnableProcess::from_state(self.state.take().unwrap(), ready_thread), ctx)
    }

    pub fn sleep(
        mut self,
    ) -> Result<
        (RunnableProcess, *mut ContextInformation),
        (SleepingProcess, *mut ContextInformation),
    > {
        let running_thread = self.running.borrow_mut().take().unwrap();
        let (sleeping_thread, ctx) = running_thread.sleep();

        Err((SleepingProcess::form_state(self.state.take().unwrap(), sleeping_thread), ctx))
    }

    pub fn exit(
        mut self,
        status: i32,
    ) -> Result<(RunnableProcess, *mut ContextInformation), (ZombieProcess, *mut ContextInformation)>
    {
        let running_thread = self.running.borrow_mut().take().unwrap();
        let (zombie_thread, ctx) = running_thread.exit();

        Err((ZombieProcess::new(self.state.take().unwrap(), zombie_thread, status), ctx))
    }

    pub fn get_tid(&self) -> Option<ThreadIdentifier> {
        self.running.borrow().as_ref().map(|thread| thread.id())
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
        self.state.as_ref().unwrap().pid()
    }

    pub fn clone_vmem(&self, mm: &VirtMemoryManager) -> Result<Vmem, Error> {
        mm.new_vmem(self.state.as_ref().unwrap().vmem())
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
        self.state.as_ref().unwrap().identity()
    }

    pub fn get_uid(&self) -> UserIdentifier {
        self.state.as_ref().unwrap().get_uid()
    }

    pub fn set_uid(&mut self, uid: UserIdentifier) -> Result<(), Error> {
        self.state.as_mut().unwrap().set_uid(uid)
    }

    pub fn get_euid(&self) -> UserIdentifier {
        self.state.as_ref().unwrap().get_euid()
    }

    pub fn set_euid(&mut self, euid: UserIdentifier) -> Result<(), Error> {
        self.state.as_mut().unwrap().set_euid(euid)
    }

    pub fn get_gid(&self) -> GroupIdentifier {
        self.state.as_ref().unwrap().get_gid()
    }

    pub fn set_gid(&mut self, gid: GroupIdentifier) -> Result<(), Error> {
        self.state.as_mut().unwrap().set_gid(gid)
    }

    pub fn get_egid(&self) -> GroupIdentifier {
        self.state.as_ref().unwrap().get_egid()
    }

    pub fn set_egid(&mut self, egid: GroupIdentifier) -> Result<(), Error> {
        self.state.as_mut().unwrap().set_egid(egid)
    }
}

// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

use crate::{
    hal::{
        arch::x86::cpu::context::ContextInformation,
        mem::VirtualAddress,
    },
    mm::{
        elf::Elf32Fhdr,
        VirtMemoryManager,
        Vmem,
    },
    pm::{
        process::{
            identity::ProcessIdentity,
            process::state::ProcessState,
        },
        thread::ReadyThread,
    },
};
use ::kcall::{
    Capability,
    Error,
    GroupIdentifier,
    ProcessIdentifier,
    UserIdentifier,
};

use super::RunningProcess;

//==================================================================================================
// Runnable Process
//==================================================================================================

pub struct RunnableProcess {
    state: Option<ProcessState>,
    thread: Option<ReadyThread>,
}

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
        Self {
            state: Some(ProcessState::new(pid, identity, vmem)),
            thread: Some(thread),
        }
    }

    pub fn from_state(state: ProcessState, thread: ReadyThread) -> Self {
        Self {
            state: Some(state),
            thread: Some(thread),
        }
    }

    pub fn run(mut self) -> (RunningProcess, *mut ContextInformation) {
        let next_thread: ReadyThread = self.thread.take().unwrap();
        let (next_thread, next_context) = next_thread.resume();
        (RunningProcess::from_state(self.state.take().unwrap(), next_thread), next_context)
    }

    pub fn exec(
        &mut self,
        mm: &mut VirtMemoryManager,
        elf: &Elf32Fhdr,
    ) -> Result<VirtualAddress, Error> {
        mm.load_elf(self.state.as_mut().unwrap().vmem_mut(), elf)
    }

    pub fn pid(&self) -> ProcessIdentifier {
        self.state.as_ref().unwrap().pid()
    }

    pub fn set_capability(&mut self, capability: Capability) {
        self.state.as_mut().unwrap().set_capability(capability);
    }

    pub fn clear_capability(&mut self, capability: Capability) {
        self.state.as_mut().unwrap().clear_capability(capability);
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

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
    Error,
    ProcessIdentifier,
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
}

// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use super::{
    interrupted::InterruptedProcess,
    runnable::RunnableProcess,
};
use crate::{
    hal::mem::VirtualAddress,
    mm::KernelPage,
    pm::{
        process::process::state::ProcessState,
        thread::SleepingThread,
    },
};
use ::kcall::{
    Error,
    ProcessIdentifier,
    ThreadIdentifier,
};

//==================================================================================================
// Suspended Process
//==================================================================================================

///
/// # Description
///
/// A type that represents a suspended process. A suspended process is a process that has all its
/// threads in either the ready or sleeping states.
///
pub struct SleepingProcess {
    state: ProcessState,
    thread: Option<SleepingThread>,
}

impl SleepingProcess {
    pub fn form_state(process: ProcessState, thread: SleepingThread) -> Self {
        Self {
            state: process,
            thread: Some(thread),
        }
    }

    pub fn wakeup_sleeping_thread(
        self,
        tid: ThreadIdentifier,
    ) -> Result<RunnableProcess, SleepingProcess> {
        match self.thread {
            Some(thread) if thread.id() == tid => {
                let ready_thread = thread.wakeup();
                Ok(RunnableProcess::from_state(self.state, ready_thread))
            },
            _ => Err(self),
        }
    }

    pub fn interrupt(mut self) -> InterruptedProcess {
        let interrupted_thread = self.thread.take().unwrap();
        let interrupted_thread = interrupted_thread.interrupt();
        InterruptedProcess::from_state(self.state, interrupted_thread)
    }

    pub fn pid(&self) -> ProcessIdentifier {
        self.state.pid()
    }

    pub fn copy_from_user_unaligned(
        &self,
        dst: &mut KernelPage,
        src: VirtualAddress,
        size: usize,
    ) -> Result<(), Error> {
        self.state.vmem().copy_from_user_unaligned(dst, src, size)
    }
}

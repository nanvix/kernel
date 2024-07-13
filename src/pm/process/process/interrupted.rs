// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use super::RunningProcess;
use crate::{
    hal::arch::x86::cpu::context::ContextInformation,
    pm::{
        process::process::state::ProcessState,
        thread::InterruptedThread,
    },
};

//==================================================================================================
// Exports
//==================================================================================================

pub struct InterruptedProcess {
    state: ProcessState,
    thread: Option<InterruptedThread>,
}

impl InterruptedProcess {
    pub fn from_state(process: ProcessState, thread: InterruptedThread) -> Self {
        Self {
            state: process,
            thread: Some(thread),
        }
    }

    pub fn resume(mut self) -> (RunningProcess, *mut ContextInformation) {
        let thread = self.thread.take().unwrap();
        let (thread, ctx) = thread.resume();
        (RunningProcess::from_state(self.state, thread), ctx)
    }
}

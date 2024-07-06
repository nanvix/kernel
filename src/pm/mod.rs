// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

pub mod process;
mod stack;
pub mod sync;
pub mod thread;

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    config,
    error::Error,
    hal,
    hal::Hal,
    mm::Vmem,
    pm::{
        process::ProcessManager,
        thread::{
            ReadyThread,
            ThreadManager,
        },
    },
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn timer_handler() {
    static mut TIMER_TICKS: usize = 0;

    unsafe { TIMER_TICKS = TIMER_TICKS.wrapping_add(1) };

    if unsafe { TIMER_TICKS } % config::SCHEDULER_FREQ == 0 {
        if let Err(e) = ProcessManager::switch() {
            error!("context switch failed: {:?}", e);
        }
    }
}

/// Initializes the processor manager.
pub fn init(hal: &mut Hal, root: Vmem, kstack: &mut u8) -> Result<ProcessManager, Error> {
    info!("initializing the processor manager...");

    // Register the timer handler.
    info!("registering timer interrupt handler...");
    hal.intman
        .register_handler(hal::arch::InterruptNumber::Timer, timer_handler)?;

    // Initialize the thread manager.
    info!("initializing the thread manager...");
    let (kernel, tm): (ReadyThread, ThreadManager) = thread::init(kstack);
    let pm: ProcessManager = process::init(kernel, root, tm);

    Ok(pm)
}

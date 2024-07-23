// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod kcall;
mod process;
pub mod sync;
pub mod thread;

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    config,
    error::Error,
    hal::{
        arch::InterruptNumber,
        mem::{
            Address,
            VirtualAddress,
        },
        Hal,
    },
    mm::Vmem,
    pm::thread::{
        ReadyThread,
        ThreadManager,
    },
};
use ::kcall::ProcessIdentifier;

//==================================================================================================
// Exports
//==================================================================================================

pub use kcall::*;
pub use process::ProcessManager;

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn copy_from_user<T>(pid: ProcessIdentifier, dst: &mut T, src: *const T) -> Result<(), Error> {
    let dst: VirtualAddress = VirtualAddress::from_raw_value(dst as *mut T as usize)?;
    let src: VirtualAddress = VirtualAddress::from_raw_value(src as *const T as usize)?;
    let size: usize = core::mem::size_of::<T>();

    ProcessManager::vmcopy_from_user(pid, dst, src, size)
}

pub fn copy_to_user<T>(pid: ProcessIdentifier, dst: *mut T, src: &T) -> Result<(), Error> {
    let dst: VirtualAddress = VirtualAddress::from_raw_value(dst as *mut T as usize)?;
    let src: VirtualAddress = VirtualAddress::from_raw_value(src as *const T as usize)?;
    let size: usize = core::mem::size_of::<T>();

    ProcessManager::vmcopy_to_user(pid, dst, src, size)
}

pub fn timer_handler(_intnum: InterruptNumber) {
    static mut TIMER_TICKS: usize = 0;

    unsafe { TIMER_TICKS = TIMER_TICKS.wrapping_add(1) };

    if unsafe { TIMER_TICKS } % config::SCHEDULER_FREQ == 0 {
        if let Err(e) = ProcessManager::switch() {
            error!("context switch failed: {:?}", e);
        }
    }
}

/// Initializes the processor manager.
pub fn init(hal: &mut Hal, root: Vmem) -> Result<ProcessManager, Error> {
    info!("initializing the processor manager...");

    // Register the timer handler.
    info!("registering timer interrupt handler...");
    hal.intman
        .register_handler(InterruptNumber::Timer, timer_handler)?;

    // Initialize the thread manager.
    info!("initializing the thread manager...");
    let (kernel, tm): (ReadyThread, ThreadManager) = thread::init();
    let pm: ProcessManager = process::init(kernel, root, tm);

    Ok(pm)
}

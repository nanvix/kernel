// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod idt;
mod interrupt;

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    config,
    error::Error,
    hal::{
        arch::x86::{
            cpu::tss::TssRef,
            mem::gdt::{
                self,
                Gdt,
                GdtPtr,
            },
            pit::Pit,
        },
        io::allocator::IoPortAllocator,
    },
};
use madt::madt::MadtInfo;

//==================================================================================================
// Exports
//==================================================================================================

pub use interrupt::{
    forge_user_stack,
    InterruptController,
    InterruptHandlersRef,
    InterruptNumber,
};

pub mod acpi;
pub mod context;
mod exception;
pub mod madt;
pub mod pit;
pub mod tss;

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn init(
    ioports: &mut IoPortAllocator,
    madt: Option<MadtInfo>,
) -> Result<(Gdt, GdtPtr, TssRef, InterruptController, Pit), Error> {
    extern "C" {
        static kstack: u8;
    }

    let (gdt, gdtr, tss) = unsafe { gdt::init(&kstack)? };
    unsafe { idt::init() };

    let pit: Pit = Pit::new(ioports, config::TIMER_FREQ)?;

    let controller: InterruptController = interrupt::init(ioports, madt)?;

    Ok((gdt, gdtr, tss, controller, pit))
}

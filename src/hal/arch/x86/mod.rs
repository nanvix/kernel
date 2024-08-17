// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

pub mod cpu;
pub mod mem;

//==================================================================================================
// Imports
//==================================================================================================

use crate::hal::{
    arch::x86::{
        cpu::{
            madt::MadtInfo,
            pit,
            tss::TssRef,
        },
        mem::gdt::{
            Gdt,
            GdtPtr,
        },
        pit::Pit,
    },
    io::{
        IoMemoryAllocator,
        IoPortAllocator,
    },
    platform::cmos::{
        Cmos,
        ShutdownStatus,
    },
};
use ::error::Error;

//==================================================================================================
// Exports
//==================================================================================================

pub use cpu::{
    forge_user_stack,
    ContextInformation,
    ExceptionInformation,
    InterruptController,
    InterruptHandler,
    InterruptNumber,
};

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that describes the architecture-specific components.
///
pub struct Arch {
    /// CMOS memory.
    _cmos: Option<Cmos>,
    /// Global Descriptor Table (GDT).
    _gdt: Option<Gdt>,
    /// Global Descriptor Table Register (GDTR).
    pub _gdtr: Option<GdtPtr>,
    /// Task State Segment (TSS).
    pub _tss: Option<TssRef>,
    /// Interrupt controller.
    pub controller: Option<InterruptController>,
    /// Programmable Interval Timer (PIT).
    pub _pit: Option<Pit>,
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn init(
    ioports: &mut IoPortAllocator,
    ioaddresses: &mut IoMemoryAllocator,
    madt: &Option<MadtInfo>,
) -> Result<Arch, Error> {
    info!("initializing architecture-specific components...");

    // Enable warm reset. It allows the INIT signal to be asserted without actually causing the
    // processor to run through its entire BIOS initialization procedure (POST).
    let mut cmos: Cmos = Cmos::init(ioports)?;
    cmos.write_shutdown_status(ShutdownStatus::JmpDwordRequestWithoutIntInit);

    // Initialize interrupt controller.
    let (gdt, gdtr, tss, controller, pit) = cpu::init(ioports, ioaddresses, madt)?;

    Ok(Arch {
        // Keep CMOS to prevent others from using the same I/O ports.
        _cmos: Some(cmos),
        _gdt: Some(gdt),
        _gdtr: Some(gdtr),
        _tss: Some(tss),
        controller: Some(controller),
        _pit: Some(pit),
    })
}

pub fn initialize_application_core(kstack: *const u8) -> Result<Arch, Error> {
    let (gdt, gdtr, tss): (Gdt, GdtPtr, TssRef) = cpu::initialize_application_core(kstack)?;

    Ok(Arch {
        _cmos: None,
        _gdt: Some(gdt),
        _gdtr: Some(gdtr),
        _tss: Some(tss),
        controller: None,
        _pit: None,
    })
}

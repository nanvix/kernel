// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

pub mod arch;
pub mod cpu;
pub mod io;
pub mod mem;
pub mod platform;

//==================================================================================================
// Imports
//==================================================================================================

use crate::hal::{
    arch::x86::{
        cpu::ExceptionController,
        Arch,
    },
    cpu::InterruptManager,
    io::{
        IoMemoryAllocator,
        IoPortAllocator,
    },
    mem::{
        MemoryRegion,
        TruncatedMemoryRegion,
        VirtualAddress,
    },
    platform::{
        madt::MadtInfo,
        Platform,
    },
};
use ::alloc::collections::linked_list::LinkedList;
use ::error::Error;

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that describes components of the hardware abstraction layer.
///
pub struct Hal {
    pub _platform: Platform,
    pub ioports: IoPortAllocator,
    pub ioaddresses: IoMemoryAllocator,
    pub intman: Option<cpu::InterruptManager>,
    pub excpman: ExceptionController,
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn init(
    memory_regions: &mut LinkedList<MemoryRegion<VirtualAddress>>,
    mmio_regions: &mut LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
    madt: &Option<MadtInfo>,
) -> Result<Hal, Error> {
    info!("initializing hardware abstraction layer...");

    let mut ioports: IoPortAllocator = IoPortAllocator::new();
    let mut ioaddresses: IoMemoryAllocator = IoMemoryAllocator::new();
    let mut platform: Platform =
        platform::init(&mut ioports, &mut ioaddresses, memory_regions, mmio_regions, madt)?;

    // Initialize the interrupt manager.
    #[cfg(feature = "enable-pic")]
    let intman: Option<InterruptManager> = match platform.arch.controller.take() {
        Some(controller) => Some(InterruptManager::new(controller)?),
        None => {
            warn!("no interrupt controller found");
            None
        },
    };

    #[cfg(not(feature = "enable-pic"))]
    let intman: Option<InterruptManager> = {
        info!("PIC support is disabled by feature flag");
        None
    };

    // Initialize exception manager.
    // TODO: add comments about safety.
    let excpman: ExceptionController = unsafe { ExceptionController::init()? };

    Ok(Hal {
        _platform: platform,
        ioports,
        ioaddresses,
        intman,
        excpman,
    })
}

pub fn initialize_application_core(kstack: *const u8) -> Result<Arch, Error> {
    info!("initializing application core...");

    let arch: Arch = arch::initialize_application_core(kstack)?;

    Ok(arch)
}

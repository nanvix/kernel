// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::hal::{
    arch::x86::{
        self,
        Arch,
    },
    io::{
        IoMemoryAllocator,
        IoPortAllocator,
    },
    mem::{
        MemoryRegion,
        TruncatedMemoryRegion,
    },
    platform::madt::MadtInfo,
};
use ::alloc::collections::linked_list::LinkedList;
use ::arch::cpu::pic;
use ::error::Error;
use ::sys::mm::VirtualAddress;

//==================================================================================================
// Structures
//==================================================================================================

pub struct Platform {
    pub arch: Arch,
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// Writes the 8-bit value `b` to the platform's standard output device.
///
/// # Parameters
///
/// - `b`: Value to write.
///
/// # Safety
///
/// This function is unsafe for multiple reasons:
/// - It assumes that the standard output device is present.
/// - It assumes that the standard output device was properly initialized.
/// - It does not prevent concurrent access to the standard output device.
///
pub unsafe fn putb(b: u8) {
    ::arch::io::out8(0xe9, b);
}

///
/// # Description
///
/// Reads an 8-bit value from the platform's standard input device.
///
/// # Return
///
/// The 8-bit value read from the standard input device.
///
/// # Safety
///
/// This function is unsafe for multiple reasons:
/// - It assumes that the standard input device is present.
/// - It assumes that the standard input device was properly initialized.
/// - It does not prevent concurrent access to the standard input device.
///
pub unsafe fn getb() -> u8 {
    ::arch::io::in8(0xe9)
}

///
/// # Description
///
/// Shutdowns the machine.
///
/// # Return
///
/// This function never returns.
///
pub fn shutdown() -> ! {
    unsafe { arch::io::out16(0x604, 0x2000) };
    loop {
        core::hint::spin_loop();
    }
}

pub fn init(
    ioports: &mut IoPortAllocator,
    ioaddresses: &mut IoMemoryAllocator,
    _memory_regions: &mut LinkedList<MemoryRegion<VirtualAddress>>,
    _mmio_regions: &mut LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
    madt: &Option<MadtInfo>,
) -> Result<Platform, Error> {
    // Register I/O ports for 8259 PIC.
    ioports.register_read_write(pic::PIC_CTRL_MASTER as u16)?;
    ioports.register_read_write(pic::PIC_DATA_MASTER as u16)?;
    ioports.register_read_write(pic::PIC_CTRL_SLAVE as u16)?;
    ioports.register_read_write(pic::PIC_DATA_SLAVE as u16)?;

    Ok(Platform {
        arch: x86::init(ioports, ioaddresses, madt)?,
    })
}

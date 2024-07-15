// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

pub mod x86;

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::cpu::{
        pic,
        pit,
    },
    error::Error,
    hal::{
        arch::x86::{
            cpu::madt::madt::MadtInfo,
            Arch,
        },
        io::allocator::IoPortAllocator,
    },
};

//==================================================================================================
// Exports
//==================================================================================================

pub use x86::{
    forge_user_stack,
    ContextInformation,
    ExceptionHandler,
    ExceptionInformation,
    InterruptController,
    InterruptHandler,
    InterruptNumber,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn init(ioports: &mut IoPortAllocator, madt: Option<MadtInfo>) -> Result<Arch, Error> {
    // Register I/O ports for 8259 PIC.
    ioports.register_read_write(pic::PIC_CTRL_MASTER as u16)?;
    ioports.register_read_write(pic::PIC_DATA_MASTER as u16)?;
    ioports.register_read_write(pic::PIC_CTRL_SLAVE as u16)?;
    ioports.register_read_write(pic::PIC_DATA_SLAVE as u16)?;

    // Register I/O ports from 0x3f8 to 0x3fc as read/write.
    for base in [0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x3E0, 0x2E0, 0x3F0, 0x2F0].iter() {
        for p in [0, 1, 2, 3, 4, 7].iter() {
            ioports.register_read_write(base + p)?;
        }

        // Register read-only ports.
        for p in [5, 6].iter() {
            ioports.register_read_only(base + p)?;
        }
    }

    // Register ports for the PIT.
    ioports.register_read_write(pit::PIT_CTRL)?;
    ioports.register_read_write(pit::PIT_DATA)?;

    x86::init(ioports, madt)
}

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

use crate::{
    config,
    error::Error,
    hal::{
        arch::x86::{
            cpu::{
                idt,
                pic,
                pic::{
                    Pic,
                    PicRef,
                },
                pit,
                tss::TssRef,
            },
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

extern "C" {
    static kstack: u8;
}

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that describes the architecture-specific components.
///
pub struct Arch {
    /// Global Descriptor Table (GDT).
    pub gdt: Option<Gdt>,
    /// Global Descriptor Table Register (GDTR).
    pub gdtr: Option<GdtPtr>,
    /// Task State Segment (TSS).
    pub tss: Option<TssRef>,
    /// Programmable Interrupt Controller (PIC).
    pub pic: Option<PicRef>,
    /// Programmable Interval Timer (PIT).
    pub pit: Option<Pit>,
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn init(ioports: &mut IoPortAllocator) -> Result<Arch, Error> {
    // Register I/O ports for 8259 PIC.
    ioports.register_read_write(Pic::PIC_CTRL_MASTER as u16)?;
    ioports.register_read_write(Pic::PIC_DATA_MASTER as u16)?;
    ioports.register_read_write(Pic::PIC_CTRL_SLAVE as u16)?;
    ioports.register_read_write(Pic::PIC_DATA_SLAVE as u16)?;

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

    let (gdt, gdtr, tss) = unsafe { gdt::init(&kstack)? };
    unsafe { idt::init() };

    let pic: PicRef = pic::init(ioports, idt::INT_OFF)?;

    let pit: Pit = Pit::new(ioports, config::TIMER_FREQ)?;

    Ok(Arch {
        gdt: Some(gdt),
        gdtr: Some(gdtr),
        tss: Some(tss),
        pic: Some(pic),
        pit: Some(pit),
    })
}

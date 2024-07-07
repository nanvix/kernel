// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    error::{
        Error,
        ErrorCode,
    },
    hal::arch::x86::cpu::interrupt::{
        ioapic::{
            Ioapic,
            UninitIoapic,
        },
        map::InterruptMap,
        pic::{
            Pic,
            UninitPic,
        },
        xapic::{
            UninitXapic,
            Xapic,
        },
        InterruptNumber,
    },
};

//==================================================================================================
// Interrupt Vector
//==================================================================================================

/// Number of entries in the interrupt vector.
pub const INTERRUPT_VECTOR_LENGTH: usize = 256;

pub type InterruptHandler = fn();

#[no_mangle]
static mut INTERRUPT_VECTOR: [Option<InterruptHandler>; INTERRUPT_VECTOR_LENGTH] =
    [None; INTERRUPT_VECTOR_LENGTH];

//==================================================================================================
// Interrupt Controller
//==================================================================================================

enum InterruptControllerType {
    Legacy(Pic),
    Xapic(Xapic, Ioapic),
}

pub struct InterruptController {
    intmap: InterruptMap,
    intctrl: InterruptControllerType,
}

impl InterruptController {
    pub fn new(
        pic: Option<UninitPic>,
        xapic: Option<UninitXapic>,
        ioapic: Option<UninitIoapic>,
        intmap: InterruptMap,
    ) -> Result<Self, Error> {
        // If legacy PIC is available, initialize it.
        let pic: Option<Pic> = if let Some(mut pic) = pic {
            Some(pic.init()?)
        } else {
            None
        };

        // Check if xAPIC is available.
        if let Some(mut xapic) = xapic {
            // Check if IOAPIC is available.
            match ioapic {
                Some(mut ioapic) => {
                    info!("using xapic and ioapic");
                    let xapic: Xapic = xapic.init()?;
                    let ioapic: Ioapic = ioapic.init()?;
                    return Ok(Self {
                        intmap,
                        intctrl: InterruptControllerType::Xapic(xapic, ioapic),
                    });
                },
                None => {
                    warn!("ioapic not found, falling back to legacy pic");
                },
            }
        }

        // If legacy PIC is available, use it.
        if let Some(pic) = pic {
            info!("using legacy pic");
            return Ok(Self {
                intmap,
                intctrl: InterruptControllerType::Legacy(pic),
            });
        }

        let reason: &str = "no interrupt controller found";
        error!("{}", reason);
        Err(Error::new(ErrorCode::InvalidArgument, reason))
    }

    pub fn ack(&mut self, intnum: InterruptNumber) -> Result<(), Error> {
        let intnum: u8 = self.intmap[intnum];
        match self.intctrl {
            InterruptControllerType::Legacy(ref mut pic) => Ok(pic.ack(intnum as u32)),
            InterruptControllerType::Xapic(ref mut xapic, _) => Ok(xapic.ack()),
        }
    }

    pub fn mask(&mut self, intnum: InterruptNumber) -> Result<(), Error> {
        let intnum: u8 = self.intmap[intnum];
        match self.intctrl {
            InterruptControllerType::Legacy(ref mut pic) => Ok(pic.mask(intnum as u16)),
            // FIXME: enable interrupt on right CPU.
            InterruptControllerType::Xapic(_, ref mut ioapic) => ioapic.enable(intnum as u8, 0),
        }
    }

    pub fn set_handler(
        &mut self,
        intnum: InterruptNumber,
        handler: Option<InterruptHandler>,
    ) -> Result<(), Error> {
        let intnum: u8 = self.intmap[intnum];
        unsafe { INTERRUPT_VECTOR[intnum as usize] = handler };
        Ok(())
    }

    pub fn get_handler(&self, intnum: InterruptNumber) -> Result<Option<InterruptHandler>, Error> {
        let intnum: u8 = self.intmap[intnum];
        unsafe { Ok(INTERRUPT_VECTOR[intnum as usize]) }
    }
}
// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use super::{
    ioapic::IoapicPtr,
    pic::Pic,
    xapic::XapicRef,
};
use crate::{
    error::{
        Error,
        ErrorCode,
    },
    hal::arch::x86::cpu::interrupt::InterruptNumber,
};

//==================================================================================================
// Structures
//==================================================================================================

pub enum InterruptController {
    Legacy(Pic),
    Xapic(XapicRef, IoapicPtr),
}

//==================================================================================================
// Implementations
//==================================================================================================

impl InterruptController {
    pub fn new(
        pic: Option<Pic>,
        xapic: Option<XapicRef>,
        ioapic: Option<IoapicPtr>,
    ) -> Result<Self, Error> {
        if let Some(xapic) = xapic {
            match ioapic {
                Some(ioapic) => {
                    info!("using xapic and ioapic");
                    return Ok(InterruptController::Xapic(xapic, ioapic));
                },
                None => {
                    warn!("ioapic not found, falling back to legacy pic");
                },
            }
        }

        if let Some(pic) = pic {
            info!("using legacy pic");
            return Ok(InterruptController::Legacy(pic));
        }

        let reason: &str = "no interrupt controller found";
        error!("{}", reason);
        Err(Error::new(ErrorCode::InvalidArgument, reason))
    }

    pub fn ack(&mut self, intnum: InterruptNumber) -> Result<(), Error> {
        match self {
            InterruptController::Legacy(pic) => Ok(pic.ack(intnum as u32)),
            InterruptController::Xapic(xapic, _) => Ok(xapic.ack()),
        }
    }

    pub fn mask(&mut self, intnum: InterruptNumber) -> Result<(), Error> {
        match self {
            InterruptController::Legacy(pic) => Ok(pic.mask(intnum as u16)),
            // FIXME: enable interrupt on right CPU.
            InterruptController::Xapic(_, ioapic) => ioapic.enable(intnum as u8, 0),
        }
    }
}

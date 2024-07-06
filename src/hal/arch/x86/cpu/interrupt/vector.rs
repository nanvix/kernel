// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use crate::{
    error::{
        Error,
        ErrorCode,
    },
    hal::arch,
};

//==================================================================================================
// Structures
//==================================================================================================

pub type InterruptHandler = fn();

pub struct InterruptHandlersRef(&'static mut [Option<InterruptHandler>; 256]);

//==================================================================================================
// Global Variables
//==================================================================================================

#[no_mangle]
static mut INTERRUPT_HANDLERS: [Option<InterruptHandler>; 256] = [None; 256];

//==================================================================================================
// Implementations
//==================================================================================================

static mut BORROWED: bool = false;

impl InterruptHandlersRef {
    pub fn try_borrow() -> Result<InterruptHandlersRef, Error> {
        unsafe {
            if BORROWED {
                Err(Error::new(ErrorCode::ResourceBusy, "interrupts already initialized"))
            } else {
                Ok(InterruptHandlersRef(&mut INTERRUPT_HANDLERS))
            }
        }
    }

    pub fn get_handler(&self, intnum: arch::InterruptNumber) -> Option<InterruptHandler> {
        self.0[intnum as usize]
    }

    pub fn set_handler(
        &mut self,
        intnum: arch::InterruptNumber,
        handler: Option<InterruptHandler>,
    ) {
        self.0[intnum as usize] = handler;
    }
}

impl Drop for InterruptHandlersRef {
    fn drop(&mut self) {
        unsafe {
            BORROWED = false;
        }
    }
}

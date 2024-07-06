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
    hal::{
        arch,
        cpu::interrupt::InterruptController,
    },
};

//==================================================================================================
// Structures
//==================================================================================================

pub type InterruptHandler = fn();

pub struct InterruptManager {
    controller: InterruptController,
}

impl InterruptManager {
    pub fn new(controller: arch::InterruptController) -> Result<Self, Error> {
        let mut intman: InterruptManager = InterruptManager {
            controller: InterruptController::new(controller)?,
        };

        intman.init()?;

        Ok(intman)
    }

    ///
    /// # Description
    ///
    /// Registers an interrupt handler.
    ///
    /// # Parameters
    ///
    /// - `intnum`: Interrupt number.
    /// - `handler`: Interrupt handler.
    ///
    /// # Returns
    ///
    /// Upon success, the interrupt handler is registered. Upon failure, an error code is returned.
    ///
    pub fn register_handler(
        &mut self,
        intnum: arch::InterruptNumber,
        handler: InterruptHandler,
    ) -> Result<(), Error> {
        trace!("register_handler(): intnum={:?}, handler={:?}", intnum, handler);
        let mut handlers: arch::InterruptHandlersRef = arch::InterruptHandlersRef::try_borrow()?;

        // Check if another handler is already registered.
        if handlers.get_handler(intnum).is_some() {
            let reason: &str = "interrupt handler already registered";
            error!(
                "register_handler(): intnum={:?}, handler={:?}, reason={:?}",
                intnum, handler, reason
            );
            return Err(Error::new(ErrorCode::ResourceBusy, reason));
        }

        handlers.set_handler(intnum, Some(handler));

        Ok(())
    }

    pub fn mask(&mut self, intnum: arch::InterruptNumber) -> Result<(), Error> {
        self.controller.mask(intnum)
    }

    fn init(&mut self) -> Result<(), Error> {
        trace!("initializing interrupt manager");
        let mut handlers: arch::InterruptHandlersRef = arch::InterruptHandlersRef::try_borrow()?;
        for intnum in arch::InterruptNumber::VALUES {
            trace!("registering default handler for interrupt {:?}", intnum);
            handlers.set_handler(intnum, None);
        }
        Ok(())
    }

    ///
    /// # Description
    ///
    /// High-level interrupt dispatcher.
    ///
    /// # Parameters
    ///
    /// - `intnum`: Number of the interrupt.
    ///
    #[no_mangle]
    extern "C" fn do_interrupt(intnum: arch::InterruptNumber) {
        match InterruptController::try_get() {
            Ok(pic) => {
                if let Err(e) = pic.ack(intnum) {
                    error!("failed to acknowledge interrupt: {:?}", e);
                }
            },
            Err(e) => error!("failed to get pic: {:?}", e),
        }

        match arch::InterruptHandlersRef::try_borrow() {
            Ok(handlers) => match handlers.get_handler(intnum) {
                Some(handler) => handler(),
                None => error!("no handler for interrupt {:?}", intnum as u32),
            },
            Err(e) => error!("failed to borrow interrupt handlers (error={:?})", e),
        }
    }
}

// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

use crate::{
    error::{
        Error,
        ErrorCode,
    },
    hal::arch::x86::cpu::{
        interrupt::{
            InterruptHandlersRef,
            InterruptNumber,
        },
        pic::{
            self,
            PicRef,
        },
    },
};

pub type InterruptHandler = fn();

pub struct InterruptManager {
    pic: PicRef,
}

impl InterruptManager {
    pub fn new(pic: PicRef) -> Result<Self, Error> {
        let mut intman: InterruptManager = InterruptManager { pic };

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
        intnum: InterruptNumber,
        handler: InterruptHandler,
    ) -> Result<(), Error> {
        trace!("register_handler(): intnum={:?}, handler={:?}", intnum, handler);
        let mut handlers: InterruptHandlersRef = InterruptHandlersRef::try_borrow()?;

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

    pub fn mask(&mut self, intnum: InterruptNumber) -> Result<(), Error> {
        self.pic.mask(intnum as u16)
    }

    fn init(&mut self) -> Result<(), Error> {
        trace!("initializing interrupt manager");
        let mut handlers: InterruptHandlersRef = InterruptHandlersRef::try_borrow()?;
        for intnum in InterruptNumber::VALUES {
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
    extern "C" fn do_interrupt(intnum: InterruptNumber) {
        match pic::try_get() {
            Ok(pic) => {
                if let Err(e) = pic.ack(intnum as u32) {
                    error!("failed to acknowledge interrupt: {:?}", e);
                }
            },
            Err(e) => error!("failed to get pic: {:?}", e),
        }

        match InterruptHandlersRef::try_borrow() {
            Ok(handlers) => match handlers.get_handler(intnum) {
                Some(handler) => handler(),
                None => error!("no handler for interrupt {:?}", intnum),
            },
            Err(e) => error!("failed to borrow interrupt handlers (error={:?})", e),
        }
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

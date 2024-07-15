// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

pub mod arch;
pub mod cpu;
pub mod io;
pub mod mem;

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    error::{
        Error,
        ErrorCode,
    },
    hal::{
        arch::x86::{
            cpu::madt::madt::MadtInfo,
            Arch,
        },
        cpu::InterruptManager,
        io::allocator::IoPortAllocator,
    },
    stdout,
    uart::{
        self,
        Uart,
    },
};
use alloc::boxed::Box;
use arch::x86::cpu::ExceptionController;

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that describes components of the hardware abstraction layer.
///
pub struct Hal {
    pub arch: Arch,
    pub ioports: IoPortAllocator,
    pub intman: cpu::InterruptManager,
    pub excpman: ExceptionController,
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn init(madt: Option<MadtInfo>) -> Result<Hal, Error> {
    info!("initializing hardware abstraction layer...");

    let mut ioports: IoPortAllocator = IoPortAllocator::new();
    let mut arch: Arch = arch::init(&mut ioports, madt)?;

    let uart: Box<Uart> = match Uart::new(&mut ioports, uart::BaudRate::Baud38400) {
        Ok(uart) => Box::new(uart),
        Err(e) => panic!("Failed to initialize UART: {:?}", e),
    };

    // Initialize the interrupt manager.
    let intman: InterruptManager = match arch.controller.take() {
        Some(controller) => InterruptManager::new(controller)?,
        None => {
            let reason: &str = "no interrupt controller found";
            error!("{}", reason);
            return Err(Error::new(ErrorCode::NoSuchDevice, reason));
        },
    };

    // Initialize exception manager.
    let excpman: ExceptionController = ExceptionController::init()?;

    unsafe { stdout::init(uart) };

    Ok(Hal {
        arch,
        ioports,
        intman,
        excpman,
    })
}

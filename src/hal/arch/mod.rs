// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

pub mod x86;

//==================================================================================================
// Imports
//==================================================================================================

use crate::hal::arch::x86::Arch;
use ::error::Error;

//==================================================================================================
// Exports
//==================================================================================================

pub use x86::{
    forge_user_stack,
    ContextInformation,
    ExceptionInformation,
    InterruptController,
    InterruptHandler,
    InterruptNumber,
};

pub fn initialize_application_core(kstack: *const u8) -> Result<Arch, Error> {
    x86::initialize_application_core(kstack)
}

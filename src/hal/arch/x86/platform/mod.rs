// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use ::sys::mm::VirtualAddress;

//==================================================================================================
// Modules
//==================================================================================================

#[cfg(feature = "baremetal")]
mod baremetal;

#[cfg(any(feature = "isapc", feature = "pc"))]
mod qemu;

pub mod bios;
pub mod cmos;

//==================================================================================================
// Exports
//==================================================================================================

#[cfg(any(feature = "isapc", feature = "pc"))]
pub use qemu::{
    putb,
    shutdown,
};

#[cfg(feature = "baremetal")]
pub use baremetal::{
    putb,
    shutdown,
};

///
/// # Description
///
/// Start address of application cores.
///
/// # Notes
///
/// This address was carefully chosen to avoid conflicts with the kernel.
///
pub const TRAMPOLINE_ADDRESS: VirtualAddress = VirtualAddress::new(0x00008000);

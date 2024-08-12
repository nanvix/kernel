// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

#[cfg(feature = "baremetal")]
mod baremetal;

#[cfg(any(feature = "isapc", feature = "pc"))]
mod qemu;

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

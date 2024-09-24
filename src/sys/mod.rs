// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

/// Architecture-specific definitions.
pub use ::arch;

/// System configuration constants.
pub mod config;

/// System constants.
pub mod constants;

/// Error codes.
pub mod error;

/// Events.
pub mod event;

/// Inter process communication.
pub mod ipc;

/// Kernel calls.
#[cfg(feature = "kcall")]
pub mod kcall;

/// Helper macros.
pub mod macros;

/// Memory management.
pub mod mm;

/// Numbers for kernel calls.
pub mod number;

/// Process management.
pub mod pm;

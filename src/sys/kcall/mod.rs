// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

/// Architecture-specific symbols.
#[cfg(target_arch = "x86")]
#[path = "arch/x86.rs"]
pub mod arch;

/// Debug facilities.
pub mod debug;

/// Event handling kernel calls.
pub mod event;

/// Inter-Process Communication (IPC) kernel calls.
pub mod ipc;

/// Memory management kernel calls.
pub mod mm;

/// Process management kernel calls.
pub mod pm;

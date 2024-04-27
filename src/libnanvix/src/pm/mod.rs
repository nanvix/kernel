/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Modules
//==============================================================================

mod constants;
mod kcall;
mod types;

//==============================================================================
// Exports
//==============================================================================

pub use self::{
    constants::*,
    kcall::*,
    types::*,
};
pub use core::ffi;

//==============================================================================
// Imports
//==============================================================================

use crate::kcall::{
    kcall1,
    KcallNumbers,
};

//==============================================================================
// Private Standalone Functions
//==============================================================================

/// **Description**
///
/// Executes the return of a user-created thread.
///
pub(crate) fn thread_caller(
    start: fn(*mut ffi::c_void) -> *mut ffi::c_void,
    arg: *mut ffi::c_void,
) -> ! {
    let ret = start(arg);
    unsafe { kcall1(KcallNumbers::ThreadExit as u32, ret as u32) };
    loop {}
}

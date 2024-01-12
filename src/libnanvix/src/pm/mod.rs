/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::kcall::{
    self,
    KcallNumbers,
};
use core::ffi;

//==============================================================================
// Public Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Spawns a new process.
///
/// **Parameters**
///
/// - `image` - Image of the target process.
///
/// **Return**
///
/// Upon successful completion, the PID of the spawned process is returned.
/// Upon failure, a negative error code is returned instead.
///
pub fn spawn(image: *const ffi::c_void) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::Spawn as u32, image as u32) as i32 }
}

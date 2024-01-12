/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::{
    kcall,
    kcall::KcallNumbers,
    misc::KernelModule,
};

//==============================================================================
// Public Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Gets information on a kernel module.
///
/// **Parameters**
///
/// - `kmod` - Storage location for kernel module information.
/// - `index` - Index of the target kernel module.
///
/// **Return**
///
/// Upon successful completion, zero is returned. Upon failure, a negative error
/// code is returned instead.
///
pub fn kmod_get(kmod: &mut KernelModule, index: u32) -> i32 {
    unsafe {
        kcall::kcall2(
            KcallNumbers::KmodGet as u32,
            kmod as *mut KernelModule as u32,
            index,
        ) as i32
    }
}

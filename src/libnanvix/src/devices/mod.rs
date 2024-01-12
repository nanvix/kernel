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
};

//==============================================================================
// Public Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Writes a buffer to the kernel's standard output device.
///
/// **Parameters**
/// - `fd` - File descriptor.
/// - `buf` - Buffer to write.
/// - `size` - Number of bytes to write.
///
/// **Return**
///
/// The number of bytes written is returned.
///
pub fn write(fd: u32, buf: *const u8, size: usize) -> u32 {
    unsafe {
        kcall::kcall3(KcallNumbers::Write as u32, fd, buf as u32, size as u32)
    }
}

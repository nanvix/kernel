// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::hal::arch;

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// Writes the string `s` to the platform's standard output device.
///
/// # Parameters
///
/// - `s`: String to write.
///
/// # Safety
///
/// This function is unsafe for multiple reasons:
///
/// - It assumes that the standard output device is present.
/// - It assumes that the standard output device was properly initialized.
/// - It does not prevent concurrent access to the standard output device.
///
pub unsafe fn puts(s: &str) {
    // Write each byte of the string to the standard output device.
    for b in s.bytes() {
        arch::putb(b);
    }
}

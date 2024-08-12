// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use ::arch::dbg;

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// Writes a string to the platform's standard output device.
///
/// # Parameters
///
/// - `s`: The string to write.
///
/// # Safety
///
/// This function is unsafe for multiple reasons:
/// - It assumes that the standard output device is present.
/// - It assumes that the standard output device was properly initialized.
/// - It does not prevent concurrent access to the standard output device.
///
pub unsafe fn puts(s: &str) {
    dbg::puts(s);
}

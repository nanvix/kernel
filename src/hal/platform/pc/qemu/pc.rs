// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// Shutdowns the machine.
///
/// # Return
///
/// This function never returns.
///
pub fn shutdown() -> ! {
    unsafe {
        arch::cpu::halt();
    };
    loop {
        core::hint::spin_loop();
    }
}

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#![no_std]
#![feature(panic_info_message)]

//==============================================================================
// Imports
//==============================================================================

extern crate nanvix;

//==============================================================================
// Standalone Functions
//==============================================================================

#[no_mangle]
pub fn main() {
    nanvix::log!("Running init server...");
    loop {}
}

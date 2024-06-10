/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#![no_std]
#![feature(panic_info_message)]

//==============================================================================
// Modules
//==============================================================================

pub mod ipc;
pub mod kcall;
pub mod misc;
pub mod mm;
pub mod pm;
pub mod readerswriters;

//==============================================================================
// Imports
//==============================================================================

extern crate nanvix;

use nanvix::power;

//==============================================================================
// Macros
//==============================================================================

///
/// **Description**
///
/// Runs test and prints whether it passed or failed on the standard output.
///
#[macro_export]
macro_rules! test {
    ($fn_name:ident($($arg:expr),*)) => {{
        match $fn_name($($arg),*) {
            true =>
                nanvix::log!("{} {}", "passed", stringify!($fn_name)),
            false =>
                panic!("{} {}", "FAILED", stringify!($fn_name)),
        }
    }};
}

//==============================================================================
// Standalone Functions
//==============================================================================

#[no_mangle]
pub fn main() {
    nanvix::log!("Running test server...");
    kcall::test();
    pm::test();
    mm::test();
    misc::test();
    ipc::test();
    readerswriters::test();
    power::shutdown();
}

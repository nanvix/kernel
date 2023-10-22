/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#![no_std]
#![feature(panic_info_message)]

//==============================================================================
// Modules
//==============================================================================

#[macro_use]
mod macros;

mod logging;
mod nanvix;
mod unwind;

//==============================================================================
// Imports
//==============================================================================

use nanvix::kcall::{self,};

//==============================================================================
// Standalone Functions
//==============================================================================

#[no_mangle]
pub fn main() {
    kcall::void0();
    kcall::void1(1);
    kcall::void2(1, 2);
    kcall::void3(1, 2, 3);
    kcall::void4(1, 2, 3, 4);

    log!("Hello, world!\n");

    kcall::shutdown();
}

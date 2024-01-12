/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#![no_std]
#![feature(panic_info_message)]

//==============================================================================
// Modules
//==============================================================================

pub mod logging;

mod unwind;

pub mod devices;
pub mod kcall;
pub mod memory;
pub mod misc;
pub mod pm;
pub mod power;
pub mod security;

#[macro_export]
macro_rules! log{
	( $($arg:tt)* ) => ({
		use core::fmt::Write;
		use $crate::logging::Logger;
		let _ = write!(&mut Logger::get(module_path!()), $($arg)*);
	})
}

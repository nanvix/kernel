/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

macro_rules! log{
	( $($arg:tt)* ) => ({
		use core::fmt::Write;
		let _ = write!(&mut ::logging::Logger::get(module_path!()), $($arg)*);
	})
}

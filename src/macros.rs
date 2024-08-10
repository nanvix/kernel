// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

///
/// # Description
///
/// Logs an INFO-level formatted message.
///
/// # Parameters
///
/// - `$($arg:tt)*`: Formatted message to be logged.
///
macro_rules! info{
	( $($arg:tt)* ) => ({
		#[allow(unused_imports)]
		use ::core::fmt::Write;
		if crate::klog::MAX_LEVEL >= crate::klog::KlogLevel::Info {
			let _ = write!(
				&mut crate::klog::Klog::get(module_path!(), crate::klog::KlogLevel::Info),
				$($arg)*
			);
		}
	})
}

///
/// # Description
///
/// Logs a TRACE-level formatted message.
///
/// # Parameters
///
/// - `$($arg:tt)*`: Formatted message to be logged.
///
macro_rules! trace{
	( $($arg:tt)* ) => ({
		#[allow(unused_imports)]
		use ::core::fmt::Write;
		if crate::klog::MAX_LEVEL >= crate::klog::KlogLevel::Trace {
			let _ = write!(
				&mut crate::klog::Klog::get(module_path!(), crate::klog::KlogLevel::Trace),
				$($arg)*
			);
		}
	})
}

///
/// # Description
///
/// Logs a WARN-level formatted message.
///
/// # Parameters
///
/// - `$($arg:tt)*`: Formatted message to be logged.
///
macro_rules! warn{
	( $($arg:tt)* ) => ({
		#[allow(unused_imports)]
		use ::core::fmt::Write;
		if crate::klog::MAX_LEVEL >= crate::klog::KlogLevel::Warn {
			let _ = write!(
				&mut crate::klog::Klog::get(module_path!(), crate::klog::KlogLevel::Warn),
				$($arg)*
			);
		}
	})
}

///
/// # Description
///
/// Logs an ERROR-level formatted message.
///
/// # Parameters
///
/// - `$($arg:tt)*`: Formatted message to be logged.
///
macro_rules! error{
	( $($arg:tt)* ) => ({
		#[allow(unused_imports)]
		use ::core::fmt::Write;
		if crate::klog::MAX_LEVEL >= crate::klog::KlogLevel::Error {
			let _ = write!(
				&mut crate::klog::Klog::get(module_path!(), crate::klog::KlogLevel::Error),
				$($arg)*
			);
		}
	})
}

macro_rules! run_test {
    ($test_func:ident) => {{
        let result: bool = $test_func();
        #[cfg(test)]
        info!("{}: {}", if result { "passed" } else { "FAILED" }, stringify!($test_func));
        assert!(result);
        result
    }};
}

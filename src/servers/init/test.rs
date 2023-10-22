/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::nanvix::kcall;

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
                log!("{} {}", stringify!($fn_name), "passed"),
            false =>
                log!("{} {}", stringify!($fn_name), "failed"),
        }
    }};
}

//==============================================================================
// Private Standalone Functions
//==============================================================================

/// Issues a void0 kernel call.
fn issue_void0_kcall() -> bool {
    kcall::void0() == 0
}

/// Issues a void1 kernel call.
fn issue_void1_kcall() -> bool {
    kcall::void1(1) == 1
}

/// Issues a void2 kernel call.
fn issue_void2_kcall() -> bool {
    kcall::void2(1, 2) == 3
}

/// Issues a void3 kernel call.
fn issue_void3_kcall() -> bool {
    kcall::void3(1, 2, 3) == 6
}

/// Issues a void4 kernel call.
fn issue_void4_kcall() -> bool {
    kcall::void4(1, 2, 3, 4) == 10
}

//==============================================================================
// Public Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Tests if we can issue kernel calls.
///
pub fn test_kernel_calls() {
    test!(issue_void0_kcall());
    test!(issue_void1_kcall());
    test!(issue_void2_kcall());
    test!(issue_void3_kcall());
    test!(issue_void4_kcall());
}

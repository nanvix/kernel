/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::{
    nanvix::{
        self,
        kcall,
    },
    test,
};

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
                log!("{} {}", "passed", stringify!($fn_name)),
            false =>
                log!("{} {}", "FAILED", stringify!($fn_name)),
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

/// Attempts to allocate and release page frame.
fn alloc_free_frame() -> bool {
    // Attempt to allocate a page frame.
    let frame: u32 = kcall::fralloc();

    // Check if we failed to allocate a page frame.
    if frame == nanvix::NULL_FRAME {
        log!("failed to allocate a page frame");
        return false;
    }

    // Check if the page frame lies on a valid range.
    if (frame * nanvix::PAGE_SIZE) < nanvix::USER_BASE_ADDRESS {
        log!("succeded to allocate an invalid page frame");
        return false;
    }

    // Attempt to release the page frame.
    let result: u32 = kcall::frfree(frame);

    // Check if we failed to release the page frame.
    if result != 0 {
        log!("failed to release a valid page frame");
        return false;
    }

    true
}

/// Attempts release the null page frame.
fn free_null_frame() -> bool {
    // Attempt to release the null page frame.
    let result: u32 = kcall::frfree(nanvix::NULL_FRAME);

    // Check if we succeeded to release the null page frame.
    if result == 0 {
        log!("succeded to release null page frame");
        return false;
    }

    true
}

/// Attempts release an invalid page frame.
fn free_invalid_frame() -> bool {
    // Attempt to release an invalid page frame.
    for frame_addr in nanvix::KERNEL_BASE_ADDRESS..nanvix::USER_BASE_ADDRESS {
        let result: u32 = kcall::frfree(frame_addr / nanvix::PAGE_SIZE);

        // Check if we succeeded to release an invalid page frame.
        if result == 0 {
            log!("succeded to release an invalid page frame");
            return false;
        }
    }

    true
}

/// Attempts to release a page frame twice.
fn double_free_frame() -> bool {
    // Attempt to allocate a page frame.
    let frame: u32 = kcall::fralloc();

    // Check if we failed to allocate a page frame.
    if frame == nanvix::NULL_FRAME {
        log!("failed to allocate a page frame");
        return false;
    }

    // Attempt to release the page frame.
    let result: u32 = kcall::frfree(frame);

    // Check if we failed to release the page frame.
    if result != 0 {
        log!("failed to release a valid page frame");
        return false;
    }

    // Attempt to release the page frame again.
    let result: u32 = kcall::frfree(frame);

    // Check if we succeeded to release the page frame again.
    if result == 0 {
        log!("succeded to release a page frame twice");
        return false;
    }

    true
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
    test!(alloc_free_frame());
    test!(free_null_frame());
    test!(free_invalid_frame());
    test!(double_free_frame());
}

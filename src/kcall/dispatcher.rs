// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use crate::kcall::ScoreBoard;

//==================================================================================================
//  Standalone Functions
//==================================================================================================

///
/// # Description
///
/// High-level kernel call dispatcher.
///
/// # Parameters
///
/// - `arg0`: First kernel call argument.
/// - `arg1`: Second kernel call argument.
/// - `arg2`: Third kernel call argument.
/// - `arg3`: Fourth kernel call argument.
/// - `arg4`: Fifth kernel call argument.
/// - `number`: Number of the kernel call.
///
#[no_mangle]
pub extern "C" fn do_kcall(number: u32, arg0: u32, arg1: u32, arg2: u32, arg3: u32) -> i32 {
    // todo: copy buffer from user space.
    // Dispatch kernel call.
    match ScoreBoard::get_mut() {
        Ok(scoreboard) => match scoreboard.dispatch(number, arg0, arg1, arg2, arg3) {
            Ok(result) => result,
            Err(e) => e.code.into_errno(),
        },

        Err(e) => e.code.into_errno(),
    }
}

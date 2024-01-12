/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use super::{
    kcall0,
    kcall1,
    kcall2,
    kcall3,
    kcall4,
    KcallNumbers,
};

//==============================================================================
// Public Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Issues a void kernel call that takes no arguments.
///
/// **Return**
///
///  This function always returns zero.
///
pub fn void0() -> u32 {
    unsafe { kcall0(KcallNumbers::Void0 as u32) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes one argument.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
///
/// **Return**
///
///  This function returns `arg0`
///
pub fn void1(arg0: u32) -> u32 {
    unsafe { kcall1(KcallNumbers::Void1 as u32, arg0) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes two arguments.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
///
/// **Return**
///
/// This function returns `arg0 + arg1`
///
pub fn void2(arg0: u32, arg1: u32) -> u32 {
    unsafe { kcall2(KcallNumbers::Void2 as u32, arg0, arg1) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes three arguments.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
/// - `arg2` - Third argument for the kernel call.
///
/// **Return**
///
/// This function returns `arg0 + arg1 + arg2`
///
pub fn void3(arg0: u32, arg1: u32, arg2: u32) -> u32 {
    unsafe { kcall3(KcallNumbers::Void3 as u32, arg0, arg1, arg2) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes four arguments.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
/// - `arg2` - Third argument for the kernel call.
/// - `arg3` - Fourth argument for the kernel call.
///
/// **Return**
///
/// This function returns `arg0 + arg1 + arg2 + arg3`
///
pub fn void4(arg0: u32, arg1: u32, arg2: u32, arg3: u32) -> u32 {
    unsafe { kcall4(KcallNumbers::Void4 as u32, arg0, arg1, arg2, arg3) }
}

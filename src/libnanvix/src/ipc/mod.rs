/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::kcall::{
    self,
    KcallNumbers,
};

///
/// **Description**
///
/// Get mailbox tag.
///
/// **Parameters**
///
/// - `image` - Image of the target process.
///
/// **Return**
///
/// Upon successful completion, the mailbox tag is returned.
/// Upon failure, a negative error code is returned instead.
///

pub fn mailbox_tag(mbxid: i32) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::Boxtag as u32, mbxid as u32) as i32 }
}

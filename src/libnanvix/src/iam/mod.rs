/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

pub mod types;

//==============================================================================
// Imports
//==============================================================================

use self::types::{
    GroupID,
    UserID,
};
use crate::kcall::{
    self,
    KcallNumbers,
};

//==============================================================================
// Public Functions
//==============================================================================

///
/// **Description**
///
/// Get the user ID of the calling process.
///
/// **Return**
///
/// The user ID of the calling process is returned.
///
pub fn getuid() -> UserID {
    unsafe { kcall::kcall0(KcallNumbers::GetUserID as u32) as UserID }
}

///
/// **Description**
///
/// Get the effective user ID of the calling process.
///
/// **Return**
///
/// The effective user ID of the calling process is returned.
///
pub fn geteuid() -> UserID {
    unsafe { kcall::kcall0(KcallNumbers::GetEffectiveUserID as u32) as UserID }
}

///
/// **Description**
///
/// Get the group ID of the calling process.
///
/// **Return**
///
/// The group ID of the calling process is returned.
///
pub fn getgid() -> GroupID {
    unsafe { kcall::kcall0(KcallNumbers::GetUserGroupID as u32) as GroupID }
}

///
/// **Description**
///
/// Get the effective group ID of the calling process.
///
/// **Return**
///
/// The effective group ID of the calling process is returned.
///
pub fn getegid() -> GroupID {
    unsafe { kcall::kcall0(KcallNumbers::GetEffectiveUserID as u32) as GroupID }
}

///
/// **Description**
///
/// Set the user ID of the calling process.
///
/// **Parameters**
///
/// - `uid` - New user ID.
///
/// **Return**
///
/// Upon successful completion, zero is returned. Upon failure, a negative error code is returned instead.
///
pub fn setuid(uid: UserID) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::SetUserID as u32, uid as u32) as i32 }
}

///
/// **Description**
///
/// Set the effective user ID of the calling process.
///
/// **Parameters**
///
/// - `uid` - New effective user ID.
///
/// **Return**
///
/// Upon successful completion, zero is returned. Upon failure, a negative error code is returned instead.
///
pub fn seteuid(uid: UserID) -> i32 {
    unsafe {
        kcall::kcall1(KcallNumbers::SetEffectiveUserID as u32, uid as u32)
            as i32
    }
}

///
/// **Description**
///
/// Set the group ID of the calling process.
///
/// **Parameters**
///
/// - `gid` - New group ID.
///
/// **Return**
///
/// Upon successful completion, zero is returned. Upon failure, a negative error code is returned instead.
///
pub fn setgid(gid: GroupID) -> i32 {
    unsafe {
        kcall::kcall1(KcallNumbers::SetUserGroupID as u32, gid as u32) as i32
    }
}

///
/// **Description**
///
/// Set the effective group ID of the calling process.
///
/// **Parameters**
///
/// - `gid` - New effective group ID.
///
/// **Return**
///
/// Upon successful completion, zero is returned. Upon failure, a negative error code is returned instead.
///
pub fn setegid(gid: GroupID) -> i32 {
    unsafe {
        kcall::kcall1(KcallNumbers::SetEffectiveUserGroupID as u32, gid as u32)
            as i32
    }
}

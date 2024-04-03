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
use core::ffi;

//==============================================================================
// Public Standalone Functions
//==============================================================================

pub fn do_mailbox_create(owner : u32, tag : u32) -> i32 {
    unsafe { kcall::kcall2(KcallNumbers::Mbxcreate as u32, owner, tag) as i32}
}

pub fn do_mailbox_open(owner : u32, tag : u32) -> i32 {
    unsafe { kcall::kcall2(KcallNumbers::Mbxopen as u32, owner, tag) as i32}
}

pub fn do_mailbox_unlink(ombxid : u32) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::Mbxunlink as u32, ombxid) as i32}
}

pub fn do_mailbox_close(ombxid : u32) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::Mbxclose as u32, ombxid) as i32}
}

pub fn do_mailbox_write(ombxid : u32, buffer : *const ffi::c_void, sz : u64) -> i32 {
    unsafe { kcall::kcall3(KcallNumbers::Mbxwrite as u32, ombxid, buffer as u32, sz as u32) as i32}
}

pub fn do_mailbox_read(ombxid : u32, buffer : *const ffi::c_void, sz : u64) -> i32 {
    unsafe { kcall::kcall3(KcallNumbers::Mbxread as u32, ombxid, buffer as u32, sz as u32) as i32}
}
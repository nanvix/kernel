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
//use core::ffi;

//==============================================================================
// Public Standalone Functions
//==============================================================================

pub fn mailbox_create(owner: u32, tag: u32) -> i32 {
    unsafe {
        kcall::kcall2(KcallNumbers::MailboxCreate as u32, owner, tag) as i32
    }
}

pub fn mailbox_open(owner: u32, tag: u32) -> i32 {
    unsafe {
        kcall::kcall2(KcallNumbers::MailboxOpen as u32, owner, tag) as i32
    }
}

pub fn mailbox_unlink(ombxid: u32) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::MailboxUnlink as u32, ombxid) as i32 }
}

pub fn mailbox_close(ombxid: u32) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::MailboxClose as u32, ombxid) as i32 }
}

pub fn mailbox_write(ombxid: u32, buffer: *const u32, sz: u64) -> i32 {
    unsafe {
        kcall::kcall3(
            KcallNumbers::MailboxWrite as u32,
            ombxid,
            buffer as u32,
            sz as u32,
        ) as i32
    }
}

pub fn mailbox_read(ombxid: u32, buffer: *const u32, sz: u64) -> i32 {
    unsafe {
        kcall::kcall3(
            KcallNumbers::Mailboxread as u32,
            ombxid,
            buffer as u32,
            sz as u32,
        ) as i32
    }
}

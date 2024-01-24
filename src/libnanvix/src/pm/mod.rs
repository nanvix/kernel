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

///
/// **Description**
///
/// Spawns a new process.
///
/// **Parameters**
///
/// - `image` - Image of the target process.
///
/// **Return**
///
/// Upon successful completion, the PID of the spawned process is returned.
/// Upon failure, a negative error code is returned instead.
///
pub fn spawn(image: *const ffi::c_void) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::Spawn as u32, image as u32) as i32 }
}

///
/// **Description**
///
/// If key already exist, associate current process at semaphore, else, create a semaphore with the key.
///
/// **Parameters**
///
/// - `Key` - Key to semaphore.
///
/// **Return**
///
/// Upon successful, the ID of the semaphore associated with the key.
/// Upon failure, a negative error code is returned instead.
///
pub fn semget(key: u32) -> i32 {
    unsafe { kcall::kcall1(KcallNumbers::Semget as u32, key as u32) as i32 }
}

///
/// **Description**
///
/// Verify if process can operate in semaphore (id), and operate (op):
///
/// **Parameters**
///
/// - `id` - Semaphore Identifier.
/// - `op` - Semaphore Operation.
///
/// **Return**
///
/// Upon successful, return a zero value.
/// Upon failure, a negative error code is returned instead.
///
pub fn semop(id: u32, op: u32) -> i32 {
    unsafe { kcall::kcall2(KcallNumbers::Semop as u32, id as u32, op as u32) as i32 }
}

///
/// **Description**
///
/// Verify if process can control semaphore (id), and exec (cmd).
///
/// **Parameters**
///
/// - `id` - Semaphore Identifier.
/// - `cmd` - Command.
/// - `val` - Value.
///
/// **Return**
///
/// Upon successful, a zero value.
/// Upon failure, a negative error code is returned instead.
/// Upon GETVALUE, semaphore value.
///
pub fn semctl(id: u32, cmd: u32, val: u32) -> i32 {
    unsafe { kcall::kcall3(KcallNumbers::Semctl as u32, id as u32, cmd as u32, val as u32) as i32 }
}

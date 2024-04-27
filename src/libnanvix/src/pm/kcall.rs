/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::{
    kcall::{
        kcall0,
        kcall1,
        kcall2,
        kcall3,
        KcallNumbers,
    },
    pm::*,
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
    unsafe { kcall1(KcallNumbers::Spawn as u32, image as u32) as i32 }
}

///
/// **Description**
///
/// Gets various information about a process.
///
/// **Parameters**
///
/// - `pid` - PID of the target process.
/// - `buffer` - Buffer to store the information about the calling process.
///
/// **Return**
///
/// On successful completion, zero is returned and information about the calling
/// process is stored in the buffer pointed to by `buf`. On error, a negative
/// error code is returned instead.
///
pub fn pinfo(pid: i32, buffer: *mut ProcessInfo) -> i32 {
    unsafe {
        kcall2(KcallNumbers::ProcessInfo as u32, pid as u32, buffer as u32)
            as i32
    }
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
    unsafe { kcall1(KcallNumbers::Semget as u32, key as u32) as i32 }
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
    unsafe { kcall2(KcallNumbers::Semop as u32, id as u32, op as u32) as i32 }
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
    unsafe {
        kcall3(
            KcallNumbers::Semctl as u32,
            id as u32,
            cmd as u32,
            val as u32,
        ) as i32
    }
}

///
/// **Description**
///
/// Returns the ID of the calling thread.
///
/// **Return**
///
/// The ID of the calling thread is returned.
///
pub fn thread_getid() -> Tid {
    unsafe { kcall0(KcallNumbers::ThreadGet as u32) as Tid }
}

///
/// **Description**
///
/// Exits the calling thread.
///
pub fn thread_exit() -> ! {
    unsafe {
        kcall0(KcallNumbers::ThreadExit as u32);
    }
    // Unreachable
    loop {}
}

///
/// **Description**
///
/// Yields the CPU to another thread.
///
pub fn thread_yield() -> () {
    unsafe {
        kcall0(KcallNumbers::ThreadYield as u32);
    }
}

///
/// **Description**
///
/// Creates a new thread.
///
/// **Parameters**
///
/// - `func` - Function to run.
/// - `arg`  - Argument to pass to the function.
///
/// **Return**
///
/// Upon successful completion, the ID of the spawned thread is returned.
/// Upon failure, a negative error code is returned instead.
///
pub fn thread_create(
    func: fn(*mut ffi::c_void) -> *mut ffi::c_void,
    arg: *mut ffi::c_void,
) -> Tid {
    unsafe {
        kcall3(
            KcallNumbers::ThreadCreate as u32,
            func as u32,
            arg as u32,
            thread_caller as u32,
        ) as Tid
    }
}

///
/// **Description**
///
/// Waits for a thread to terminate.
///
/// **Parameters**
///
/// - `tid` - ID of the target thread.
/// - `retval` - Location to store the return value of the target thread.
///
/// **Return**
///
/// Upon successful completion, the ID of the spawned thread is returned.
/// Upon failure, a negative error code is returned instead.
///
pub fn thread_join(tid: Tid, retval: *mut *mut ffi::c_void) -> i32 {
    unsafe {
        kcall2(KcallNumbers::ThreadJoin as u32, tid as u32, retval as u32)
            as i32
    }
}

///
/// **Description**
///
/// Detaches a thread.
///
/// **Parameters**
///
/// - `tid` - ID of the target thread.
///
/// **Return**
///
/// Upon successful completion, the ID of the spawned thread is returned.
/// Upon failure, a negative error code is returned instead.
///
pub fn thread_detach(tid: Tid) -> i32 {
    unsafe { kcall1(KcallNumbers::ThreadDetach as u32, tid as u32) as i32 }
}

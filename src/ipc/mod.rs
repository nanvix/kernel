// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
//  Modules
//==================================================================================================

mod kcall;
mod manager;
mod mbx;

//==================================================================================================
//  Imports
//==================================================================================================

use crate::{
    hal::mem::{
        Address,
        VirtualAddress,
    },
    ipc::manager::IpcManager,
    pm::ProcessManager,
};
use ::kcall::{
    Error,
    ProcessIdentifier,
};

//==================================================================================================
//  Exports
//==================================================================================================

pub use kcall::{
    recv,
    send,
};
pub use mbx::Mailbox;

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn copy_from_user<T>(pid: ProcessIdentifier, dst: &mut T, src: *const T) -> Result<(), Error> {
    let dst: VirtualAddress = VirtualAddress::from_raw_value(dst as *mut T as usize)?;
    let src: VirtualAddress = VirtualAddress::from_raw_value(src as *const T as usize)?;
    let size: usize = core::mem::size_of::<T>();

    ProcessManager::vmcopy_from_user(pid, dst, src, size)?;
    todo!();
}

pub fn copy_to_user<T>(pid: ProcessIdentifier, dst: *mut T, src: &T) -> Result<(), Error> {
    let dst: VirtualAddress = VirtualAddress::from_raw_value(dst as *mut T as usize)?;
    let src: VirtualAddress = VirtualAddress::from_raw_value(src as *const T as usize)?;
    let size: usize = core::mem::size_of::<T>();

    ProcessManager::vmcopy_to_user(pid, dst, src, size)?;
    todo!();
}

pub fn init() {
    IpcManager::init();
}

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::nanvix::{
    kcall::{
        kcall0,
        kcall1,
        kcall2,
        kcall3,
        kcall4,
        KcallNumbers,
    },
    memory::{
        FrameNumber,
        VirtualAddress,
        VirtualMemory,
    },
    security::AccessMode,
};

//==============================================================================
// Types
//==============================================================================

/// Requests for `vmctrl`.
pub enum VmCtrlRequest {
    /// Creates a virtual memory space.
    ChangePermissions(VirtualAddress, AccessMode),
}

impl Into<u32> for VmCtrlRequest {
    fn into(self) -> u32 {
        match self {
            VmCtrlRequest::ChangePermissions(_, _) => 0,
        }
    }
}

#[derive(Clone, Debug, Default)]
#[repr(C)]
pub struct PageInfo {
    pub frame: FrameNumber,
    pub mode: AccessMode,
}

//==============================================================================
// Public Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Allocates a page frame.
///
/// **Return**
///
/// On successful completion, the number of the allocated page frame
/// is returned. On error, `FRAME_NULL` is returned instead.
///
pub fn fralloc() -> u32 {
    unsafe { kcall0(KcallNumbers::FrameAlloc as u32) }
}

///
///
/// **Description**
///
/// Frees a page frame.
///
/// **Parameters**
/// - `frame` - Number of the target page frame.
///
/// **Return**
///
/// On successful completion, zero is returned. On error, a negative
/// error code is returned instead.
///
pub fn frfree(frame: u32) -> u32 {
    unsafe { kcall1(KcallNumbers::FrameFree as u32, frame) }
}

///
/// **Description**
///
/// Creates a virtual memory space.
///
/// **Return**
///
/// On successful completion, the number of the created virtual memory
/// space is returned. On error, `VMEM_NULL` is returned instead.
///
pub fn vmcreate() -> VirtualMemory {
    unsafe { kcall0(KcallNumbers::VmemCreate as u32) as VirtualMemory }
}

///
/// **Description**
///
/// Removes a virtual memory space.
///
/// **Parameters**
///
/// - `vmem` - Number of the target virtual memory space.
///
/// **Return**
///
/// On successful completion, zero is returned. On error, a negative
/// error code is returned instead.
///
pub fn vmremove(vmem: VirtualMemory) -> u32 {
    unsafe { kcall1(KcallNumbers::VmemRemove as u32, vmem as u32) }
}

///
/// **Description**
///
/// Maps a page frame into a virtual memory space.
///
/// **Parameters**
///
/// - `vmem` - Number of the target virtual memory space.
/// - `vaddr` - Target virtual address.
/// - `frame` - Number of the target page frame.
///
/// **Return**
///
/// On successful completion, zero is returned. On error, a negative
/// error code is returned instead.
///
pub fn vmmap(
    vmem: VirtualMemory,
    vaddr: VirtualAddress,
    frame: FrameNumber,
) -> u32 {
    unsafe { kcall3(KcallNumbers::VmemMap as u32, vmem as u32, vaddr, frame) }
}

///
/// **Description**
///
/// Unmaps a page frame from a virtual memory space.
///
/// **Parameters**
///
/// - `vmem` - Number of the target virtual memory space.
/// - `vaddr` - Target virtual address.
///
/// **Return**
///
/// On successful completion, zero is returned. On error, a negative
/// error code is returned instead.
///
pub fn vmunmap(vmem: VirtualMemory, vaddr: VirtualAddress) -> u32 {
    unsafe { kcall2(KcallNumbers::VmemUnmap as u32, vmem as u32, vaddr) }
}

///
/// **Description**
///
/// Manipulates various parameters from a virtual memory space.
///
/// **Parameters**
///
/// - `vmem` - Number of the target virtual memory space.
/// - `request` - Request code.
/// - `arg0` - First argument for the request.
/// - `arg1` - Second argument for the request.
///
/// **Return**
///
/// On successful completion, zero is returned. On error, a negative
/// error code is returned instead.
///
pub fn vmctrl(vmem: VirtualMemory, request: VmCtrlRequest) -> u32 {
    unsafe {
        match request {
            VmCtrlRequest::ChangePermissions(address, mode) => kcall4(
                KcallNumbers::VmemControl as u32,
                vmem as u32,
                request.into(),
                address,
                mode.into(),
            ),
        }
    }
}

///
/// **Description**
///
/// Gets information about a page frame.
///
/// **Parameters**
///
/// - `vmem` - Number of the target virtual memory space.
/// - `vaddr` - Target virtual address.
/// - `buf` - Buffer to store information about the page frame.
///
/// **Return**
///
/// On successful completion, zero is returned. On error, a negative
/// error code is returned instead.
///
pub fn vminfo(
    vmem: VirtualMemory,
    vaddr: VirtualAddress,
    buf: *mut PageInfo,
) -> u32 {
    unsafe {
        kcall3(
            KcallNumbers::VmemInfo as u32,
            vmem as u32,
            vaddr,
            buf as u32,
        )
    }
}

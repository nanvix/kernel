/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Types
//==============================================================================

use crate::memory::{
    self,
};

/// Process ID
pub type Pid = i32;

/// Thread ID
pub type Tid = i32;

//==============================================================================
// Structures
//==============================================================================

///
/// **Description**
///
/// Process Information
///
/// **Notes**
///
/// - The size of this structure should match the size of the `struct
/// process_info` structure in the kernel space. See `src/kernel/pm/process.h`
/// for more information.
///
#[derive(Clone, Debug, Default)]
#[repr(C)]
pub struct ProcessInfo {
    pub pid: Pid,
    pub tid: Tid,
    pub vmem: memory::VirtualMemory,
}

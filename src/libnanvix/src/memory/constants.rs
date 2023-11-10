/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Constants
//==============================================================================

/// Base address for kernel memory.
pub const KERNEL_BASE_ADDRESS: u32 = 0x00000000;

/// Base address for user memory.
pub const USER_BASE_ADDRESS: u32 = 0x04000000;

/// Page size (in bytes).
pub const PAGE_SIZE: u32 = 4096;

/// Null page frame.
pub const NULL_FRAME: u32 = u32::MAX;

/// Null virtual memory space.
pub const NULL_VMEM: i32 = i32::MIN;

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

pub mod kcall;

/// Base address for kernel memory.
pub const KERNEL_BASE_ADDRESS: u32 = 0x00000000;

/// Base address for user memory.
pub const USER_BASE_ADDRESS: u32 = 0x00800000;

/// Page size (in bytes).
pub const PAGE_SIZE: u32 = 4096;

/// Null page frame.
pub const NULL_FRAME: u32 = u32::MAX;

/// Null virtual memory space.
pub const NULL_VMEM: i32 = i32::MIN;

#[derive(Clone, Copy, Default)]
#[repr(C)]
pub struct AccessMode {
    mode: u32,
}

impl AccessMode {
    pub fn new(read: bool, write: bool, exec: bool) -> Self {
        let mut mode = 0;

        if read {
            mode |= 1 << 6;
        }

        if write {
            mode |= 1 << 7;
        }

        if exec {
            mode |= 1 << 8;
        }

        Self { mode }
    }

    pub fn read(&self) -> bool {
        self.mode & (1 << 6) != 0
    }

    pub fn write(&self) -> bool {
        self.mode & (1 << 7) != 0
    }

    pub fn exec(&self) -> bool {
        self.mode & (1 << 8) != 0
    }
}

impl Into<u32> for AccessMode {
    fn into(self) -> u32 {
        self.mode
    }
}

impl core::fmt::Debug for AccessMode {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        write!(
            f,
            "AccessMode {{ {}{}{} }}",
            if self.read() { "r" } else { "-" },
            if self.write() { "w" } else { "-" },
            if self.exec() { "x" } else { "-" }
        )
    }
}

pub type VirtualMemory = i32;
pub type VirtualAddress = u32;
pub type FrameNumber = u32;

#[derive(Clone, Debug, Default)]
#[repr(C)]
pub struct PageInfo {
    pub frame: FrameNumber,
    pub mode: AccessMode,
}

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

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Structures
//==============================================================================

#[derive(Clone, Copy, Default)]
#[repr(C)]
pub struct AccessMode {
    mode: u32,
}

//==============================================================================
// Associated Functions
//==============================================================================

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

//==============================================================================
// Trait Implementations
//==============================================================================

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

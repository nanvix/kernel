/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::{
    memory::PhysicalAddress,
    misc::KMOD_CMDLINE_MAX,
};

//==============================================================================
// Structures
//==============================================================================

/// Kernel Module.
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct KernelModule {
    /// Start address.
    pub start: PhysicalAddress,
    /// End address.
    pub end: PhysicalAddress,
    /// Command line.
    pub cmdline: [u8; KMOD_CMDLINE_MAX],
}

//==============================================================================
// Trait Implementations
//==============================================================================

impl Default for KernelModule {
    fn default() -> Self {
        Self {
            start: 0,
            end: 0,
            cmdline: [0; KMOD_CMDLINE_MAX],
        }
    }
}

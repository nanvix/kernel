// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    hal::{
        arch::x86::cpu::madt::madt::MadtInfo,
        mem::{
            MemoryRegion,
            VirtualAddress,
        },
    },
    kmod::KernelModule,
};
use alloc::collections::LinkedList;

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that represents information collected by the bootloader.
///
pub struct BootInfo {
    /// ACPI MADT information.
    pub madt: Option<MadtInfo>,
    /// Memory layout information.
    pub memory_layout: LinkedList<MemoryRegion<VirtualAddress>>,
    /// Kernel modules.
    pub kernel_modules: LinkedList<KernelModule>,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl BootInfo {
    ///
    /// # Description
    ///
    /// Instantiates a new boot information structure.
    ///
    /// # Returns
    ///
    /// A new boot information structure.
    ///
    pub fn new(
        madt: Option<MadtInfo>,
        memory_layout: LinkedList<MemoryRegion<VirtualAddress>>,
        kernel_modules: LinkedList<KernelModule>,
    ) -> Self {
        Self {
            madt,
            memory_layout,
            kernel_modules,
        }
    }
}

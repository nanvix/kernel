// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::mem,
    config,
    error::Error,
    hal::mem::PageAddress,
    mm::{
        KernelPage,
        VirtMemoryManager,
    },
};
use alloc::vec::Vec;

//==================================================================================================
// Structures
//==================================================================================================

#[derive(Debug)]
enum StackStorage {
    /// Raw parts.
    RawParts(*mut u8, usize),
    /// Kernel pages.
    KernelPages(Vec<KernelPage>),
}

#[derive(Debug)]
pub struct Stack {
    /// Base address.
    base: *mut u8,
    /// Size.
    size: usize,
    /// Underlying storage.
    #[allow(dead_code)]
    storage: StackStorage,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl Stack {
    pub fn new(mm: &mut VirtMemoryManager) -> Result<Self, Error> {
        let kpages: Vec<KernelPage> = mm.alloc_kpages(config::KSTACK_SIZE / mem::PAGE_SIZE)?;

        let base: PageAddress = kpages[0].base();
        let size: usize = config::KSTACK_SIZE;

        Ok(Self {
            base: base.into_raw_value() as *mut u8,
            size,
            storage: StackStorage::KernelPages(kpages),
        })
    }

    pub fn from_raw_parts(base: *mut u8, size: usize) -> Self {
        Self {
            base,
            size,
            storage: StackStorage::RawParts(base, size),
        }
    }

    pub fn top(&self) -> *mut u8 {
        unsafe { self.base.add(self.size) }
    }
}

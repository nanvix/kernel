// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    error::Error,
    mm::phys::{
        kpool::{
            KernelFrame,
            Kpool,
        },
        upool::{
            Upool,
            UserFrame,
        },
    },
};
use alloc::vec::Vec;

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub struct PhysMemoryManager {
    kpool: Kpool,
    upool: Upool,
}

impl PhysMemoryManager {
    pub fn new(kpool: Kpool, upool: Upool) -> Self {
        PhysMemoryManager { kpool, upool }
    }

    pub fn alloc_user_frame(&mut self) -> Result<UserFrame, Error> {
        self.upool.alloc()
    }

    pub fn alloc_many_user_frames(&mut self, nframes: usize) -> Result<Vec<UserFrame>, Error> {
        self.upool.alloc_many(nframes)
    }

    pub fn alloc_kernel_frame(&mut self) -> Result<KernelFrame, Error> {
        self.kpool.alloc()
    }

    pub fn alloc_many_kernel_frames(&mut self, nframes: usize) -> Result<Vec<KernelFrame>, Error> {
        self.kpool.alloc_many(nframes)
    }
}

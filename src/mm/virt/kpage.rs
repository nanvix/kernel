// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    hal::mem::{
        FrameAddress,
        PageAddress,
    },
    mm::phys::KernelFrame,
};

//==================================================================================================
// Structures
//==================================================================================================

#[derive(Debug)]
pub struct KernelPage {
    /// Underlying kernel frame.
    frame: KernelFrame,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl KernelPage {
    /// Initializes a new kernel page.
    pub fn new(frame: KernelFrame) -> Self {
        Self { frame }
    }

    /// Gets the base address of the kernel page.
    pub fn base(&self) -> PageAddress {
        PageAddress::new(self.frame.base().into_page_address().into_virtual_address())
    }

    pub fn frame_address(&self) -> FrameAddress {
        self.frame.base()
    }
}

// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    error::Error,
    hal::mem::{
        AccessPermission,
        Address,
        MemoryRegionType,
        PageAligned,
        TruncatedMemoryRegion,
        VirtualAddress,
    },
    klib::Alignment,
};
use alloc::string::String;

//==================================================================================================
// Memory-Mapped I/O Address
//==================================================================================================

///
/// # Description
///
/// A type that represents a memory-mapped I/O address.
///
#[derive(Debug, Clone, Copy, PartialEq, Eq, Ord, PartialOrd)]
pub struct MemoryMappedIoAddress(VirtualAddress);

impl Address for MemoryMappedIoAddress {
    fn from_raw_value(raw_addr: usize) -> Result<Self, Error> {
        Ok(Self(VirtualAddress::from_raw_value(raw_addr)?))
    }

    fn into_raw_value(self) -> usize {
        self.0.into_raw_value()
    }

    fn align_up(&self, align: Alignment) -> Result<Self, Error> {
        Ok(Self(self.0.align_up(align)?))
    }

    fn align_down(&self, align: crate::klib::Alignment) -> Result<Self, Error> {
        Ok(Self(self.0.align_down(align)?))
    }

    fn is_aligned(&self, align: crate::klib::Alignment) -> Result<bool, Error> {
        Ok(self.0.is_aligned(align)?)
    }

    fn max_addr() -> usize {
        VirtualAddress::max_addr()
    }

    fn as_ptr(&self) -> *const u8 {
        self.0.as_ptr()
    }

    fn as_mut_ptr(&self) -> *mut u8 {
        self.0.as_mut_ptr()
    }
}

//==================================================================================================
// Memory-Mapped I/O Region
//==================================================================================================

///
/// # Description
///
/// A structure that represents a memory-mapped I/O region.
///
#[derive(Debug)]
pub struct MemoryMappedIoRegion(TruncatedMemoryRegion<MemoryMappedIoAddress>);

impl MemoryMappedIoRegion {
    pub fn new(
        name: &str,
        start: MemoryMappedIoAddress,
        size: usize,
        perm: AccessPermission,
    ) -> Result<Self, Error> {
        Ok(Self(TruncatedMemoryRegion::new(
            name,
            PageAligned::from_address(start)?,
            size,
            MemoryRegionType::Usable,
            perm,
        )?))
    }

    pub fn name(&self) -> String {
        self.0.name()
    }

    pub fn start(&self) -> PageAligned<MemoryMappedIoAddress> {
        self.0.start()
    }

    pub fn size(&self) -> usize {
        self.0.size()
    }

    pub fn typ(&self) -> MemoryRegionType {
        self.0.typ()
    }

    pub fn perm(&self) -> AccessPermission {
        self.0.perm()
    }
}

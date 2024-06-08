// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

pub mod elf;
mod phys;
mod virt;

//==================================================================================================
// Exports
//==================================================================================================

pub mod kheap;
pub use virt::{
    KernelPage,
    VirtMemoryManager,
    Vmem,
    USER_BASE,
    USER_STACK_TOP,
};

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    error::Error,
    hal::{
        arch::x86::mem::mmu::page_table::PageTable,
        mem::{
            MemoryRegion,
            MemoryRegionType,
            PageAligned,
            PageTableAddress,
            PhysicalAddress,
            TruncatedMemoryRegion,
            VirtualAddress,
        },
    },
    kimage::KernelImage,
    mm::phys::PhysMemoryManager,
};
use alloc::{
    collections::LinkedList,
    vec::Vec,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

// Splits memory regions into virtual and physical.
fn parse_memory_regions(
    memory_regions: LinkedList<MemoryRegion<VirtualAddress>>,
) -> Result<
    (
        LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
        LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
        LinkedList<TruncatedMemoryRegion<PhysicalAddress>>,
    ),
    Error,
> {
    let mut memory_regions: LinkedList<MemoryRegion<VirtualAddress>> = {
        let mut memory_regions: Vec<_> = memory_regions.into_iter().collect();
        memory_regions.sort();
        memory_regions.into_iter().collect()
    };

    let mut virtual_memory_regions: LinkedList<TruncatedMemoryRegion<VirtualAddress>> =
        LinkedList::new();
    let mut other_virtual_memory_regions: LinkedList<TruncatedMemoryRegion<VirtualAddress>> =
        LinkedList::new();
    let mut physical_memory_regions: LinkedList<TruncatedMemoryRegion<PhysicalAddress>> =
        LinkedList::new();

    while let Some(region) = memory_regions.pop_front() {
        if region.typ() == MemoryRegionType::Reserved {
            if PhysicalAddress::from_virtual_address(region.start()).is_ok() {
                if region.typ() != MemoryRegionType::Usable {
                    if let Ok(region) =
                        TruncatedMemoryRegion::from_virtual_memory_region(region.clone())
                    {
                        physical_memory_regions.push_back(region);
                    }
                }
                virtual_memory_regions
                    .push_back(TruncatedMemoryRegion::from_memory_region(region)?);
            } else {
                other_virtual_memory_regions
                    .push_back(TruncatedMemoryRegion::from_memory_region(region)?);
            }
        }
    }

    Ok((other_virtual_memory_regions, virtual_memory_regions, physical_memory_regions))
}

/// Initializes the memory manager.
pub fn init(
    kimage: &KernelImage,
    memory_regions: LinkedList<MemoryRegion<VirtualAddress>>,
) -> Result<(Vmem, VirtMemoryManager), Error> {
    info!("initializing the memory manager ...");

    let (other_virtual_memory_regions, virtual_memory_regions, physical_memory_regions): (
        LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
        LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
        LinkedList<TruncatedMemoryRegion<PhysicalAddress>>,
    ) = parse_memory_regions(memory_regions)?;

    let physman: PhysMemoryManager = phys::init(
        TruncatedMemoryRegion::from_virtual_memory_region(kimage.kpool())?,
        physical_memory_regions,
    )?;

    // TODO: Map other memory regions.

    let root: LinkedList<(PageTableAddress, PageTable)> = virt::init(virtual_memory_regions)?;

    VirtMemoryManager::new(root, physman)
}

// Returns the user base stack address.
pub fn user_stack_top() -> PageAligned<VirtualAddress> {
    PageAligned::from_address(USER_STACK_TOP).unwrap()
}

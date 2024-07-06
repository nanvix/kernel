// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod kpage;
mod manager;
mod upage;
mod vmem;

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::mem,
    config,
    error::{
        Error,
        ErrorCode,
    },
    hal::{
        arch::x86::mem::mmu::{
            self,
            page_table::{
                PageTable,
                PageTableStorage,
            },
        },
        io::mmio::{
            MemoryMappedIoAddress,
            MemoryMappedIoRegion,
        },
        mem::{
            AccessPermission,
            Address,
            FrameAddress,
            PageAddress,
            PageAligned,
            PageTableAddress,
            PageTableAligned,
            PhysicalAddress,
            TruncatedMemoryRegion,
            VirtualAddress,
        },
    },
    klib,
};
use alloc::{
    collections::LinkedList,
    vec::Vec,
};
use core::cmp::Ordering;

//==================================================================================================
// Exports
//==================================================================================================

pub use kpage::KernelPage;
pub use manager::VirtMemoryManager;
pub use vmem::{
    Vmem,
    USER_BASE,
    USER_STACK_TOP,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

#[derive(Debug)]
enum Region {
    GeneralPurpose(TruncatedMemoryRegion<VirtualAddress>),
    Io(MemoryMappedIoRegion),
}

impl Region {
    fn size(&self) -> usize {
        match self {
            Region::GeneralPurpose(region) => region.size(),
            Region::Io(region) => region.size(),
        }
    }
}

// FIXME: this function is too long and complex.
pub fn init(
    mut mmio_regions: LinkedList<MemoryMappedIoRegion>,
    mut virtual_memory_regions: LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
) -> Result<LinkedList<(PageTableAddress, PageTable)>, Error> {
    info!("booking virtual memory regions ...");

    let mut root_pagetables: LinkedList<(PageTableAddress, PageTable)> = LinkedList::new();

    let mut regions: LinkedList<Region> = LinkedList::new();
    while let Some(region) = mmio_regions.pop_front() {
        regions.push_back(Region::Io(region));
    }
    while let Some(region) = virtual_memory_regions.pop_front() {
        regions.push_back(Region::GeneralPurpose(region));
    }

    // TODO: sort memory regions by start address.
    let mut regions: LinkedList<Region> = {
        let mut regions: Vec<Region> = regions.into_iter().collect();
        regions.sort_by(|a, b| match (a, b) {
            (Region::GeneralPurpose(a), Region::GeneralPurpose(b)) => a.start().cmp(&b.start()),
            (Region::Io(a), Region::Io(b)) => a.start().cmp(&b.start()),
            _ => Ordering::Less,
        });
        regions.into_iter().collect()
    };

    // Identity map memory regions.
    while let Some(region) = regions.pop_front() {
        info!("booking: {:?}", region);
        assert!(region.size() <= mem::PGTAB_SIZE);

        let raw_vaddr: usize = match &region {
            Region::GeneralPurpose(region) => region.start().into_raw_value(),
            Region::Io(region) => region.start().into_raw_value(),
        };

        let (page_table_addr, mut page_table): (PageTableAddress, PageTable) = if let Some(last) =
            root_pagetables.pop_back()
        {
            let page_table_addr: PageTableAddress =
                PageTableAddress::new(PageTableAligned::from_address(VirtualAddress::new(
                    klib::align_down(raw_vaddr, mmu::PGTAB_ALIGNMENT),
                ))?);

            match page_table_addr.cmp(&last.0) {
                Ordering::Greater => {
                    root_pagetables.push_back(last);
                    let page_table: PageTable = PageTable::new(PageTableStorage::new());
                    let page_table_addr: PageTableAligned<VirtualAddress> =
                        PageTableAligned::from_address(VirtualAddress::new(klib::align_down(
                            raw_vaddr,
                            mmu::PGTAB_ALIGNMENT,
                        )))?;
                    (PageTableAddress::new(page_table_addr), page_table)
                },
                Ordering::Equal => last,
                Ordering::Less => {
                    let reason: &str = "overlapping memory regions";
                    error!("{}: {:#010x}", reason, raw_vaddr);
                    return Err(Error::new(ErrorCode::InvalidArgument, reason));
                },
            }
        } else {
            trace!("creating new page table for {:#010x}", raw_vaddr);
            let page_table: PageTable = PageTable::new(PageTableStorage::new());
            let page_table_addr: PageTableAligned<VirtualAddress> = PageTableAligned::from_address(
                VirtualAddress::new(klib::align_down(raw_vaddr, mmu::PGTAB_ALIGNMENT)),
            )?;
            (PageTableAddress::new(page_table_addr), page_table)
        };

        let mut paddr: FrameAddress = match &region {
            Region::GeneralPurpose(_) => FrameAddress::new(PageAligned::from_address(
                PhysicalAddress::from_raw_value(raw_vaddr)?,
            )?),
            Region::Io(region) => {
                let mmio_addr: MemoryMappedIoAddress = region.start().into_inner();
                let phys_addr: PhysicalAddress =
                    // FIXME: ensure safety here.
                    unsafe { PhysicalAddress::from_mmio_address(mmio_addr)? };
                let page_aligned_phys_addr: PageAligned<PhysicalAddress> =
                    PageAligned::from_address(phys_addr)?;
                FrameAddress::new(page_aligned_phys_addr)
            },
        };

        let mut raw_vaddr: usize = raw_vaddr;
        let end: usize = raw_vaddr + (region.size() - 1);

        while raw_vaddr < end {
            // FIXME: do not be so open about permissions and caching.
            page_table.map(
                PageAddress::new(PageAligned::from_raw_value(raw_vaddr)?),
                paddr,
                true,
                true,
                false,
                AccessPermission::RDWR,
            )?;
            if raw_vaddr == (config::MEMORY_SIZE - mem::PAGE_SIZE) {
                break;
            }
            raw_vaddr = raw_vaddr + mem::PAGE_SIZE;
            paddr = match &region {
                Region::GeneralPurpose(_) => FrameAddress::new(PageAligned::from_address(
                    PhysicalAddress::from_raw_value(raw_vaddr)?,
                )?),
                Region::Io(region) => {
                    let mmio_addr: MemoryMappedIoAddress = region.start().into_inner();
                    let phys_addr: PhysicalAddress =
                    // FIXME: ensure safety here.
                    unsafe { PhysicalAddress::from_mmio_address(mmio_addr)? };
                    let page_aligned_phys_addr: PageAligned<PhysicalAddress> =
                        PageAligned::from_address(phys_addr)?;
                    FrameAddress::new(page_aligned_phys_addr)
                },
            };
        }

        root_pagetables.push_back((page_table_addr, page_table));
    }

    Ok(root_pagetables)
}

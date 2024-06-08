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
use alloc::collections::LinkedList;
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

pub fn init(
    mut virtual_memory_regions: LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
) -> Result<LinkedList<(PageTableAddress, PageTable)>, Error> {
    info!("booking virtual memory regions ...");

    let mut root_pagetables: LinkedList<(PageTableAddress, PageTable)> = LinkedList::new();

    // Identity map memory regions.
    while let Some(region) = virtual_memory_regions.pop_front() {
        info!("booking: {:?}", region);
        assert!(region.size() <= mem::PGTAB_SIZE);

        let mut vaddr: PageAligned<VirtualAddress> = region.start();
        let end: VirtualAddress =
            VirtualAddress::new(region.start().into_raw_value() + (region.size() - 1));

        let (page_table_addr, mut page_table): (PageTableAddress, PageTable) = if let Some(last) =
            root_pagetables.pop_back()
        {
            let page_table_addr: PageTableAddress =
                PageTableAddress::new(PageTableAligned::from_address(VirtualAddress::new(
                    klib::align_down(vaddr.into_raw_value(), mmu::PGTAB_ALIGNMENT),
                ))?);

            match page_table_addr.cmp(&last.0) {
                Ordering::Greater => {
                    root_pagetables.push_back(last);
                    let page_table: PageTable = PageTable::new(PageTableStorage::new());
                    let page_table_addr: PageTableAligned<VirtualAddress> =
                        PageTableAligned::from_address(VirtualAddress::new(klib::align_down(
                            vaddr.into_raw_value(),
                            mmu::PGTAB_ALIGNMENT,
                        )))?;
                    (PageTableAddress::new(page_table_addr), page_table)
                },
                Ordering::Equal => last,
                Ordering::Less => {
                    return Err(Error::new(
                        ErrorCode::InvalidArgument,
                        "overlapping memory regions",
                    ))
                },
            }
        } else {
            trace!("creating new page table for {:?}", vaddr);
            let page_table: PageTable = PageTable::new(PageTableStorage::new());
            let page_table_addr: PageTableAligned<VirtualAddress> = PageTableAligned::from_address(
                VirtualAddress::new(klib::align_down(vaddr.into_raw_value(), mmu::PGTAB_ALIGNMENT)),
            )?;
            (PageTableAddress::new(page_table_addr), page_table)
        };

        let mut paddr: FrameAddress = FrameAddress::new(PageAligned::from_address(
            PhysicalAddress::from_virtual_address(vaddr.into_inner())?,
        )?);

        while vaddr.into_inner() < end {
            // FIXME: do not be so open about permissions.
            page_table.map(PageAddress::new(vaddr), paddr, true, AccessPermission::RDWR)?;
            if vaddr.into_raw_value() == (config::MEMORY_SIZE - mem::PAGE_SIZE) {
                break;
            }
            vaddr = PageAligned::from_raw_value(vaddr.into_raw_value() + mem::PAGE_SIZE)?;
            paddr = FrameAddress::from_raw_value(paddr.into_raw_value() + mem::PAGE_SIZE)?;
        }

        root_pagetables.push_back((page_table_addr, page_table));
    }

    Ok(root_pagetables)
}

// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod frame;
mod kpool;
mod manager;
mod upool;

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::mem,
    config,
    error::Error,
    hal::mem::{
        PhysicalAddress,
        TruncatedMemoryRegion,
    },
    klib::raw_array::RawArray,
    mm::phys::{
        frame::FrameAllocator,
        upool::Upool,
    },
};
use alloc::collections::LinkedList;

//==================================================================================================
// Exports
//==================================================================================================

pub use self::{
    kpool::{
        KernelFrame,
        Kpool,
    },
    manager::PhysMemoryManager,
    upool::UserFrame,
};

//==================================================================================================
// Global Variables
//==================================================================================================

/// Frame allocator storage.
static mut FRAME_ALLOCATOR_STORAGE: [u8; config::MEMORY_SIZE
    / (mem::FRAME_SIZE * u8::BITS as usize)] =
    [0; config::MEMORY_SIZE / (mem::FRAME_SIZE * u8::BITS as usize)];

//==================================================================================================
// Standalone Functions
//==================================================================================================

fn book_physical_memory_regions(
    frame_allocator: &mut FrameAllocator,
    physical_memory_regions: LinkedList<TruncatedMemoryRegion<PhysicalAddress>>,
) -> Result<(), Error> {
    info!("booking physical memory regions ...");

    // Book physical memory that is not usable.
    for region in physical_memory_regions.iter() {
        info!("booking: {:?}", region);
        frame_allocator.alloc_range(region)?;
    }

    Ok(())
}
pub fn init(
    kpool: TruncatedMemoryRegion<PhysicalAddress>,
    physical_memory_regions: LinkedList<TruncatedMemoryRegion<PhysicalAddress>>,
) -> Result<PhysMemoryManager, Error> {
    // Initialize frame allocator.
    info!("initializing the frame allocator ...");
    let mut frame_allocator: FrameAllocator = {
        // Safety: the frame allocator storage is valid and has a static lifetime.
        let storage: RawArray<u8> = unsafe {
            let (ptr, len): (*mut u8, usize) =
                (FRAME_ALLOCATOR_STORAGE.as_mut_ptr(), FRAME_ALLOCATOR_STORAGE.len());
            RawArray::from_raw_parts(ptr, len)?
        };
        FrameAllocator::from_raw_storage(storage)?
    };
    book_physical_memory_regions(&mut frame_allocator, physical_memory_regions)?;

    // Initialize kernel page pool.
    info!("initializing the kernel page pool ...");
    let kpool: Kpool = Kpool::new(kpool)?;

    // Initialize user page pool.
    info!("initializing the user page pool ...");
    let upool: Upool = Upool::new(frame_allocator);

    Ok(PhysMemoryManager::new(kpool, upool))
}

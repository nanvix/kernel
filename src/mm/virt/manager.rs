// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::mem,
    error::Error,
    hal::{
        arch::x86::mem::mmu::page_table::PageTable,
        mem::{
            AccessPermission,
            Address,
            PageAligned,
            PageTableAddress,
            VirtualAddress,
        },
    },
    mm::{
        elf::{
            self,
            Elf32Fhdr,
        },
        phys::{
            KernelFrame,
            PhysMemoryManager,
            UserFrame,
        },
        virt::{
            kpage::KernelPage,
            vmem::Vmem,
        },
    },
};
use alloc::{
    collections::LinkedList,
    rc::Rc,
    vec::Vec,
};
use core::cell::RefCell;

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// Memory manager.
///
pub struct VirtMemoryManager {
    /// Physical memory manager.
    physman: PhysMemoryManager,
}

impl VirtMemoryManager {
    ///
    /// # Description
    ///
    /// Instantiates a memory manager.
    ///
    /// # Parameters
    /// - `kernel_pages`: Kernel pages.
    /// - `kernel_page_tables`: Kernel page tables.
    /// - `physman`: Physical memory manager.
    ///
    pub fn new(
        kernel_pages: LinkedList<KernelPage>,
        kernel_page_tables: LinkedList<(PageTableAddress, PageTable)>,
        physman: PhysMemoryManager,
    ) -> Result<(Vmem, Self), Error> {
        let root: Vmem = Vmem::new(
            Rc::new(RefCell::new(kernel_pages)),
            Rc::new(RefCell::new(kernel_page_tables)),
        )?;

        // Load root root address space.
        root.load()?;

        Ok((root, Self { physman }))
    }

    /// Creates a new virtual address space, based on root.
    pub fn new_vmem(&self, vmem: &Vmem) -> Result<Vmem, Error> {
        let kernel_pages: Rc<RefCell<LinkedList<KernelPage>>> = vmem.kernel_pages();
        let kernel_page_tables: Rc<RefCell<LinkedList<(PageTableAddress, PageTable)>>> =
            vmem.kernel_page_tables();

        let new_vmem: Vmem = Vmem::new(kernel_pages, kernel_page_tables)?;

        trace!(
            "new_vmem(): new_vmem={:?}, old_vmem={:?}",
            new_vmem.pgdir().physical_address(),
            vmem.pgdir().physical_address()
        );

        Ok(new_vmem)
    }

    pub fn alloc_upage(
        &mut self,
        vmem: &mut Vmem,
        vaddr: PageAligned<VirtualAddress>,
        access: AccessPermission,
    ) -> Result<(), Error> {
        let uframe: UserFrame = self.physman.alloc_user_frame()?;

        vmem.map(uframe, vaddr, access)?;

        Ok(())
    }

    pub fn alloc_upages(
        &mut self,
        vmem: &mut Vmem,
        mut vaddr: PageAligned<VirtualAddress>,
        nframes: usize,
        access: AccessPermission,
    ) -> Result<(), Error> {
        trace!("alloc_upages(): vaddr={:?}, nframes={}", vaddr, nframes);

        let uframes: Vec<UserFrame> = self.physman.alloc_many_user_frames(nframes)?;

        // FIXME: check if range is not busy.

        for uframe in uframes {
            vaddr = PageAligned::from_raw_value(vaddr.into_raw_value() + mem::PAGE_SIZE)?;
            vmem.map(uframe, vaddr, access)?;
        }

        Ok(())
    }

    pub fn alloc_kpage(&mut self) -> Result<KernelPage, Error> {
        let kframe: KernelFrame = self.physman.alloc_kernel_frame()?;
        Ok(KernelPage::new(kframe))
    }

    pub fn alloc_kpages(&mut self, size: usize) -> Result<Vec<KernelPage>, Error> {
        let mut kpages: Vec<KernelFrame> = self.physman.alloc_many_kernel_frames(size)?;

        let mut pages: Vec<KernelPage> = Vec::new();
        while let Some(kframes) = kpages.pop() {
            pages.push(KernelPage::new(kframes));
        }

        Ok(pages)
    }

    /// Load an ELF image into a virtual address space.
    pub fn load_elf(&mut self, vmem: &mut Vmem, elf: &Elf32Fhdr) -> Result<VirtualAddress, Error> {
        let entry: VirtualAddress = elf::elf32_load(self, vmem, elf)?;

        Ok(entry)
    }
}

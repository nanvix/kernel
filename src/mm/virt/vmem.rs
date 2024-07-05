// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::mem::{
        self,
        paging::PageDirectoryEntry,
    },
    error::{
        Error,
        ErrorCode,
    },
    hal::{
        arch::x86::mem::mmu::{
            self,
            page_directory::{
                PageDirectory,
                PageDirectoryStorage,
            },
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
            VirtualAddress,
        },
    },
    klib,
    mm::{
        phys::UserFrame,
        virt::{
            kpage::KernelPage,
            upage::AttachedUserPage,
        },
    },
};
use alloc::{
    collections::LinkedList,
    rc::Rc,
};
use core::cell::RefCell;

//==================================================================================================
// Constants
//==================================================================================================

/// Base address of user space.
pub const USER_BASE: VirtualAddress = VirtualAddress::new(0x40000000);
/// End address of user space.
pub const USER_END: VirtualAddress = VirtualAddress::new(0xc0000000);
/// Base address of user stack.
pub const USER_STACK_TOP: VirtualAddress = VirtualAddress::new(0xc0000000);

// TODO: `USER_BASE` should be aligned to a page boundary.

// TODO: `USER_BASE` should be aligned to a page table boundary.

//==================================================================================================
// Virtual Memory Space
//==================================================================================================

/// A type that represents a virtual memory space.
pub struct Vmem {
    /// Underlying page directory.
    pgdir: PageDirectory,
    /// List of kernel page tables.
    kernel_page_tables: Rc<RefCell<LinkedList<(PageTableAddress, PageTable)>>>,
    /// List of kernel pages mapped in the virtual address space.
    /// NOTE: this currently excludes kernel pages that are identity mapped.
    kernel_pages: Rc<RefCell<LinkedList<KernelPage>>>,
    /// List of underling page tables holding user pages.
    user_page_tables: LinkedList<PageTable>,
    /// List of user pages in the virtual memory space.
    user_pages: LinkedList<AttachedUserPage>,
}

impl Vmem {
    /// Initializes a new virtual memory space.
    pub fn new(
        kernel_pages: Rc<RefCell<LinkedList<KernelPage>>>,
        kernel_page_tables: Rc<RefCell<LinkedList<(PageTableAddress, PageTable)>>>,
    ) -> Result<Self, Error> {
        trace!("new()");

        let mut pgdir: PageDirectory = PageDirectory::new(PageDirectoryStorage::new());
        pgdir.clean();

        // Map root page tables.
        for (vaddr, page_table) in kernel_page_tables.borrow().iter() {
            let page_table_address: FrameAddress = page_table.physical_address()?;
            // FIXME: do not be so open about permissions.
            pgdir.map(*vaddr, page_table_address, true, AccessPermission::RDWR)?;
        }

        Ok(Self {
            pgdir,
            kernel_page_tables,
            kernel_pages,
            user_page_tables: LinkedList::new(),
            user_pages: LinkedList::new(),
        })
    }

    pub fn load(&self) -> Result<(), Error> {
        let pgdir_addr: FrameAddress = self.pgdir.physical_address()?;
        unsafe { mmu::load_page_directory(pgdir_addr.into_raw_value()) };
        Ok(())
    }

    /// Returns a reference to the underlying page directory.
    pub fn pgdir(&self) -> &PageDirectory {
        &self.pgdir
    }

    ///
    /// # Description
    ///
    /// Maps a kernel page to the target virtual address space.
    ///
    /// # Parameters
    /// - `kpage`: Kernel page to be mapped.
    /// - `vaddr`: Virtual address of the target page.
    ///
    /// # Returns
    ///
    /// Upon success, empty is returned. Upon failure, an error code is returned instead.
    ///
    pub fn map_kpage(
        &mut self,
        kpage: KernelPage,
        vaddr: PageAligned<VirtualAddress>,
    ) -> Result<(), Error> {
        let pt_vaddr: PageTableAddress = PageTableAddress::new(PageTableAligned::from_raw_value(
            klib::align_down(vaddr.into_raw_value(), mmu::PGTAB_ALIGNMENT),
        )?);

        // Get the corresponding page directory entry.
        let pde: PageDirectoryEntry = match self.pgdir.read_pde(pt_vaddr) {
            Some(pde) => pde,
            None => {
                let reason: &str = "failed to read page directory entry";
                error!("map_kpage(): {}", reason);
                return Err(Error::new(ErrorCode::TryAgain, reason));
            },
        };

        // Check if page table does not exist.
        if !pde.is_present() {
            let pgtable_storage: PageTableStorage = PageTableStorage::new();
            let page_table: PageTable = PageTable::new(pgtable_storage);

            self.pgdir.map(
                pt_vaddr,
                page_table.physical_address()?,
                false,
                AccessPermission::RDWR,
            )?;

            //===================================================================
            // NOTE: if we fail beyond this point we should unmap the page table.
            //===================================================================

            // Add page table to the list of kernel page tables.
            self.kernel_page_tables
                .borrow_mut()
                .push_back((pt_vaddr, page_table));
        };

        // Get corresponding page table.
        for (pt_addr, pt) in self.kernel_page_tables.borrow_mut().iter_mut() {
            if pt_addr.into_raw_value() == pt_vaddr.into_raw_value() {
                // Map the page to the target virtual address space.
                pt.map(
                    PageAddress::new(vaddr),
                    kpage.frame_address(),
                    true,
                    AccessPermission::RDWR,
                )?;

                // Add the kernel page to the list of kernel pages.
                self.kernel_pages.borrow_mut().push_back(kpage);

                // Reload page directory to force a TLB flush.
                self.load()?;

                return Ok(());
            }
        }

        let reason: &str = "page table not found";
        error!("lookup_kernel_page_table(): {}", reason);
        return Err(Error::new(ErrorCode::NoSuchEntry, reason));
    }

    /// Maps a page to the target virtual address space.
    pub fn map(
        &mut self,
        uframe: UserFrame,
        vaddr: PageAligned<VirtualAddress>,
        access: AccessPermission,
    ) -> Result<(), Error> {
        // Check if the provided address lies outside the user space.
        if !Self::is_user_addr(vaddr)? {
            return Err(Error::new(ErrorCode::BadAddress, "address is not in user space"));
        }

        // Get corresponding page table.
        let page_table: &mut PageTable = {
            let vaddr: PageTableAligned<VirtualAddress> = PageTableAligned::from_raw_value(
                klib::align_down(vaddr.into_raw_value(), mmu::PGTAB_ALIGNMENT),
            )?;
            // Get the corresponding page directory entry.
            let mut pde: PageDirectoryEntry =
                match self.pgdir.read_pde(PageTableAddress::new(vaddr)) {
                    Some(pde) => pde,
                    None => {
                        let reason: &str = "failed to read page directory entry";
                        error!("map(): {}", reason);
                        return Err(Error::new(ErrorCode::TryAgain, reason));
                    },
                };

            // Get corresponding page table.
            // Check if corresponding page table does not exist.
            if !pde.is_present() {
                let pgtable_storage: PageTableStorage = PageTableStorage::new();
                let page_table: PageTable = PageTable::new(pgtable_storage);

                let page_table_address: FrameAddress = page_table.physical_address()?;
                self.pgdir
                    .map(PageTableAddress::new(vaddr), page_table_address, false, access)?;

                //===================================================================
                // NOTE: if we fail beyond this point we should unmap the page table.
                //===================================================================

                // Add the page table to the list of page tables.
                self.user_page_tables.push_back(page_table);

                // Get the corresponding page directory entry.
                pde = match self.pgdir.read_pde(PageTableAddress::new(vaddr)) {
                    Some(pde) => pde,
                    None => unreachable!("failed to read page directory entry"),
                };
            };

            self.lookup_page_table(&pde)?
        };

        // Map the page to the target virtual address space.
        page_table.map(PageAddress::new(vaddr), uframe.address(), false, access)?;

        //=============================================================
        // NOTE: if we fail beyond this point we should unmap the page.
        //=============================================================

        self.user_pages
            .push_back(AttachedUserPage::new(PageAddress::new(vaddr), uframe));

        Ok(())
    }

    pub fn kernel_pages(&self) -> Rc<RefCell<LinkedList<KernelPage>>> {
        self.kernel_pages.clone()
    }

    pub fn kernel_page_tables(&self) -> Rc<RefCell<LinkedList<(PageTableAddress, PageTable)>>> {
        self.kernel_page_tables.clone()
    }

    /// Asserts whether an address lies in the user space.
    fn is_user_addr(virt_addr: PageAligned<VirtualAddress>) -> Result<bool, Error> {
        Ok(virt_addr >= PageAligned::from_address(USER_BASE)?
            && virt_addr < PageAligned::from_address(USER_END)?)
    }

    /// Looks up a page table in the list of page tables.
    fn lookup_page_table(&mut self, pde: &PageDirectoryEntry) -> Result<&mut PageTable, Error> {
        // Check if corresponding page table does not exist.
        if !pde.is_present() {
            return Err(Error::new(ErrorCode::NoSuchEntry, "page table not present"));
        }

        // Get corresponding page table.
        let pgtab_addr: FrameAddress = FrameAddress::from_frame_number(pde.frame())?;

        // Find corresponding page table.
        let mut page_table: Option<&mut PageTable> = None;
        for pt in self.user_page_tables.iter_mut() {
            if pt.physical_address()? == pgtab_addr {
                page_table = Some(pt);
                break;
            }
        }

        match page_table {
            Some(pt) => Ok(pt),
            None => {
                let reason: &str = "page table not found";
                error!("lookup_page_table(): {}", reason);
                Err(Error::new(ErrorCode::NoSuchEntry, reason))
            },
        }
    }

    ///
    /// # Description
    ///
    /// Finds a user page in the target virtual memory space.
    ///
    /// # Parameters
    ///
    /// - `vaddr`: Virtual address of the target page.
    ///
    /// # Returns
    ///
    /// Upon success, a reference to the target user page is returned. Upon failure, an error code is
    /// returned instead.
    ///
    pub fn find_page(
        &self,
        vaddr: PageAligned<VirtualAddress>,
    ) -> Result<&AttachedUserPage, Error> {
        for page in self.user_pages.iter() {
            if page.vaddr().into_virtual_address() == vaddr {
                return Ok(page);
            }
        }

        let reason: &str = "page not found";
        error!("physcopy(): {}", reason);
        Err(Error::new(ErrorCode::NoSuchEntry, reason))
    }

    pub fn copy_from_user_unaligned(
        &self,
        dst: &mut KernelPage,
        src: VirtualAddress,
        size: usize,
    ) -> Result<(), Error> {
        extern "C" {
            fn __physcopy(dst: *mut u8, src: *const u8, size: usize);
        }

        // Check if size is too big.
        if size > mem::PAGE_SIZE {
            let reason: &str = "size is too big";
            error!("copy_from_user_unaligned(): {}", reason);
            return Err(Error::new(ErrorCode::InvalidArgument, reason));
        }

        let vaddr = PageAligned::from_address(src.align_down(mmu::PAGE_ALIGNMENT)?)?;

        let offset: usize = src.into_raw_value() - vaddr.into_raw_value();

        // Check if area spans across pages.
        if offset + size > mem::PAGE_SIZE {
            let reason: &str = "area spans across pages";
            error!("copy_from_user_unaligned(): {}", reason);
            return Err(Error::new(ErrorCode::InvalidArgument, reason));
        }

        let src = self.find_page(vaddr)?;

        let src_frame: FrameAddress = src.frame_address();
        let dst_frame: FrameAddress = dst.frame_address();

        unsafe {
            __physcopy(
                dst_frame.into_raw_value() as *mut u8,
                (src_frame.into_raw_value() + offset) as *const u8,
                size,
            )
        };

        Ok(())
    }

    pub unsafe fn physcopy(
        &mut self,
        dst: PageAligned<VirtualAddress>,
        src: PageAligned<PhysicalAddress>,
    ) -> Result<(), Error> {
        extern "C" {
            fn __physcopy(dst: *mut u8, src: *const u8, size: usize);
        }

        // Get corresponding user page.
        let page = self.find_page(dst)?;
        let uframe: FrameAddress = page.frame_address();
        let dst: PageAligned<PhysicalAddress> = uframe.into_physical_address();
        let dst: *mut u8 = dst.into_raw_value() as *mut u8;
        let src: *const u8 = (src.into_raw_value()) as *const u8;
        __physcopy(dst, src, mem::PAGE_SIZE);
        Ok(())
    }
}

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/kmod.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @name Memory Regions Constants
 */
/**@{*/
#define VMEM_REGION 5              /** Memory Regions number. */
#define ROOT_PGTAB_NUM VMEM_REGION /** Root page table size.  */
/**@}*/

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Physical memory region.
 */
struct phys_memory_region {
    paddr_t pbase;    /** Base physical address. */
    paddr_t pend;     /** End physical address.  */
    bool writable;    /** Writable?              */
    bool executable;  /** Executable?            */
    const char *desc; /** Description.           */
};

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Root page directory.
 *
 * @todo FIXME: make this variable private.
 */
struct pde root_pgdir[PGDIR_LENGTH] __attribute__((aligned(PAGE_SIZE)));

/**
 * @brief Root page tables.
 */
static struct pte root_pgtabs[ROOT_PGTAB_NUM][PGTAB_LENGTH]
    __attribute__((aligned(PAGE_SIZE)));

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

static void book_reserved_memory(void)
{
    kprintf(MODULE_NAME " INFO: booking reserved address ranges");
    for (unsigned i = 0; i < mmap_count(); i++) {
        struct mmap_entry entry = {0};

        // Assert should not fail because we request details of a valid
        // entry.
        KASSERT(mmap_get(&entry, i) == 0);

        // Skip entries that lie outside of managed memory.
        // FIXME: This is a workaround for the static bitmap page frames.
        if (entry.base >= MEMORY_SIZE) {
            continue;
        }

        switch (entry.type) {
            case MMAP_ENTRY_BADRAM:
                kprintf(MODULE_NAME " INFO: booking badram address range");
            // Fall through.
            case MMAP_ENTRY_RESERVED:
                // Assert should not fail because no page frame has been
                // allocated.
                KASSERT(frame_book_range(entry.base, entry.base + entry.size) ==
                        0);
                break;
            default:
                break;
        }
    }
}

static void book_kmods_memory(void)
{
    kprintf(MODULE_NAME " INFO: booking address ranges of kernel modules");
    for (unsigned i = 0; i < kmod_count(); i++) {
        struct kmod kmod = {0};

        // Assert should not fail because we request details of a valid
        // module.
        KASSERT(kmod_get(&kmod, i) == 0);

        kprintf(MODULE_NAME " INFO: booking address range for module %s",
                kmod.cmdline);

        // Assert should not fail because no page frame has been allocated.
        KASSERT(frame_book_range(kmod.start, kmod.end) == 0);
    }
}

static void book_kernel_memory(
    struct phys_memory_region phys_memory_layout[VMEM_REGION])
{
    // Book all page frames that lie in the kernel address range.
    kprintf(MODULE_NAME " INFO: booking kernel address range");

    // Physical Address Range: Kernel Text.
    KASSERT(frame_book_range(PADDR(&__TEXT_START), PADDR(&__TEXT_END)) == 0);
    phys_memory_layout[0].pbase = ALIGN(PADDR(&__TEXT_START), PAGE_SIZE);
    phys_memory_layout[0].pend = TRUNCATE(PADDR(&__TEXT_END), PAGE_SIZE);
    phys_memory_layout[0].writable = false;
    phys_memory_layout[0].executable = true;
    phys_memory_layout[0].desc = "kernel text";

    // Physical Address Range: Kernel Data.
    KASSERT(frame_book_range(PADDR(&__DATA_START), PADDR(&__DATA_END)) == 0);
    phys_memory_layout[1].pbase = ALIGN(PADDR(&__DATA_START), PAGE_SIZE);
    phys_memory_layout[1].pend = TRUNCATE(PADDR(&__DATA_END), PAGE_SIZE);
    phys_memory_layout[1].writable = true;
    phys_memory_layout[1].executable = false;
    phys_memory_layout[1].desc = "kernel data";

    // Physical Address Range: Kernel BSS.
    KASSERT(frame_book_range(PADDR(&__BSS_START), PADDR(&__BSS_END)) == 0);
    phys_memory_layout[2].pbase = ALIGN(PADDR(&__BSS_START), PAGE_SIZE);
    phys_memory_layout[2].pend = TRUNCATE(PADDR(&__BSS_END), PAGE_SIZE);
    phys_memory_layout[2].writable = true;
    phys_memory_layout[2].executable = false;
    phys_memory_layout[2].desc = "kernel bss";

    // Physical Address Range: Kernel Read Only Data.
    KASSERT(frame_book_range(PADDR(&__RODATA_START), PADDR(&__RODATA_END)) ==
            0);
    phys_memory_layout[3].pbase = ALIGN(PADDR(&__RODATA_START), PAGE_SIZE);
    phys_memory_layout[3].pend = TRUNCATE(PADDR(&__RODATA_END), PAGE_SIZE);
    phys_memory_layout[3].writable = false;
    phys_memory_layout[3].executable = false;
    phys_memory_layout[3].desc = "kernel rodata";
}

static void book_kpool_memory(
    struct phys_memory_region phys_memory_layout[VMEM_REGION])
{
    // Book all page frames that lie in the kernel pool address range.
    kprintf(MODULE_NAME " INFO: booking kpool address range");
    KASSERT(frame_book_range(KPOOL_BASE_PHYS, KPOOL_END_PHYS) == 0);

    phys_memory_layout[4].pbase = KPOOL_BASE_PHYS;
    phys_memory_layout[4].pend = KPOOL_END_PHYS;
    phys_memory_layout[4].writable = true;
    phys_memory_layout[4].executable = false;
    phys_memory_layout[4].desc = "kpool";
}

/**
 * @brief Prints information about memory layout.
 *
 * The memory_info() prints information about the virtual
 * memory layout.
 *
 * @author Davidson Francis
 */
static void memory_info(struct phys_memory_region mem_layout_[VMEM_REGION])
{
    const size_t KB = 1024;    // Kilobyte.
    const size_t MB = KB * KB; // Megabyte.

    for (int i = 0; i < VMEM_REGION; i++) {
        kprintf(MODULE_NAME " INFO: %s_base=%x %s_end=%x",
                mem_layout_[i].desc,
                mem_layout_[i].pbase,
                mem_layout_[i].desc,
                mem_layout_[i].pend);
    }
    kprintf(MODULE_NAME " INFO: user_base=%x user_end=%x",
            USER_BASE_VIRT,
            USER_END_VIRT);
    kprintf(MODULE_NAME
            " INFO: memsize=%d MB kmem=%d KB kpool=%d KB umem=%d KB",
            MEMORY_SIZE / MB,
            KMEM_SIZE / MB,
            KPOOL_SIZE / MB,
            UMEM_SIZE / MB);
}

/**
 * @brief Asserts the memory alignment.
 *
 * Checks the memory alignment for regions that are aligned at page
 * boundaries and regions that must be aligned at page table
 * boundaries.
 *
 * @author Davidson Francis
 */
static void memory_check_align(
    struct phys_memory_region mem_layout_[VMEM_REGION])
{
    /* These should be aligned at page boundaries. */
    for (int i = 0; i < VMEM_REGION; i++) {
        if (mem_layout_[i].pbase & (PAGE_SIZE - 1)) {
            kpanic("%s base address misaligned (vbase=%x)",
                   mem_layout_[i].desc,
                   mem_layout_[i].pbase);
        }
        if (mem_layout_[i].pend & (PAGE_SIZE - 1)) {
            kpanic("%s end address misaligned (vend=%x)",
                   mem_layout_[i].desc,
                   mem_layout_[i].pend);
        }
    }

    if (USER_BASE_VIRT & (PGTAB_SIZE - 1))
        kpanic("user base address misaligned");
    if (USER_END_VIRT & (PGTAB_SIZE - 1))
        kpanic("user end address misaligned");
}

/**
 * @brief Builds the memory layout.
 *
 * For each memory region, maps each page into the underlying
 * page table and maps the page table.
 *
 * @author Davidson Francis
 */
static void memory_map(struct phys_memory_region mem_layout_[VMEM_REGION])
{
    /* Clean root page directory. */
    for (int i = 0; i < PGDIR_LENGTH; i++) {
        pde_clear(&root_pgdir[i]);
    }

    /* Build root address space. */
    for (int i = 0; i < VMEM_REGION; i++) {
        paddr_t pbase = mem_layout_[i].pbase;
        paddr_t pend = mem_layout_[i].pend;
        int w = mem_layout_[i].writable;
        int x = mem_layout_[i].executable;
        int root_pgtab_num = pbase >> PGTAB_SHIFT;

        // We expect that kernel memory regions reside
        // in kernel memory. Panic if it does not.
        if (root_pgtab_num >= ROOT_PGTAB_NUM) {
            kpanic("kernel memory region does not reside in kernel memory");
        }

        /* Map underlying pages. */
        for (paddr_t j = pbase, k = pbase; k < pend;
             j += PAGE_SIZE, k += PAGE_SIZE) {

            // We expect that kernel memory regions do not
            // overlap with user memory. Panic if it does.
            if (k >= USER_BASE_VIRT) {
                kpanic("kernel memory region overlaps with user memory");
            }

            mmu_page_map(root_pgtabs[root_pgtab_num], j, k, w, x);
        }

        /*
         * Map underlying page table.
         *
         * It is important to note that there are no problems to
         * map multiple times the same page table.
         */
        mmu_pgtab_map(root_pgdir,
                      PADDR(root_pgtabs[root_pgtab_num]),
                      ALIGN(pbase, PGTAB_SIZE));
    }

    for (unsigned i = 0; i < kmod_count(); i++) {
        struct kmod kmod = {0};

        // Assert shouldn't fail because we request details of a valid module.
        KASSERT(kmod_get(&kmod, i) == 0);

        kprintf(MODULE_NAME " INFO: booking address range for module %s",
                kmod.cmdline);

        paddr_t pbase = ALIGN(kmod.start, PAGE_SIZE);
        paddr_t pend = TRUNCATE(kmod.end, PAGE_SIZE);
        int w = false;
        int x = false;
        int root_pgtab_num = pbase >> PGTAB_SHIFT;

        // We expect that kernel modules resides
        // in kernel memory. Panic if it does not.
        if (root_pgtab_num >= ROOT_PGTAB_NUM) {
            kpanic("kernel modules do not reside in kernel memory");
        }

        /* Map underlying pages. */
        for (paddr_t j = pbase, k = pbase; k < pend;
             j += PAGE_SIZE, k += PAGE_SIZE) {

            // We assume that kernel modules do not
            // overlap with user memory. Panic if it does.
            if (k >= USER_BASE_VIRT) {
                kpanic("kernel modules overlaps with user memory");
            }

            mmu_page_map(root_pgtabs[root_pgtab_num], j, k, w, x);
        }

        /*
         * Map underlying page table.
         *
         * It is important to note that there are no problems to
         * map multiple times the same page table.
         */
        mmu_pgtab_map(root_pgdir,
                      PADDR(root_pgtabs[root_pgtab_num]),
                      ALIGN(pbase, PGTAB_SIZE));
    }

    /* Load virtual address space and enable MMU. */
    tlb_load(PADDR(root_pgdir));
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

static struct phys_memory_region phys_memory_layout[VMEM_REGION];

/**
 * @details This function initializes the memory interface.
 */
const void *memory_init(void)
{
    kprintf(MODULE_NAME " INFO: initializing memory layout...");

    mmap_print();
    kmod_print();

    book_reserved_memory();
    book_kmods_memory();

    book_kernel_memory(phys_memory_layout);

    book_kpool_memory(phys_memory_layout);

    memory_check_align(phys_memory_layout);

    frame_print();
    memory_info(phys_memory_layout);

    memory_map(phys_memory_layout);

    return (&root_pgdir);
}

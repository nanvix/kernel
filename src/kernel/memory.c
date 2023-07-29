/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>

#define KB 1024
#define MB (KB * KB)

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Root page directory.
 */
static struct pde cluster_root_pgdir[PGDIR_LENGTH]
    __attribute__((aligned(PAGE_SIZE)));

/**
 * @brief Root page tables.
 */
static struct pte cluster_root_pgtabs[ROOT_PGTAB_NUM][PGTAB_LENGTH]
    __attribute__((aligned(PAGE_SIZE)));

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Prints information about memory layout.
 *
 * The mem_info() prints information about the virtual
 * memory layout.
 *
 * @author Davidson Francis
 */
static void mem_info(void)
{
    int i; /* Loop index. */

    kprintf("[hal][cluster] text = %d KB data = %d KB bss = %d KB",
            __div((&__TEXT_END - &__TEXT_START), KB),
            __div((&__DATA_END - &__DATA_START), KB),
            __div((&__BSS_END - &__BSS_START), KB));
    for (i = 0; i < MEM_REGIONS; i++) {
        kprintf("[hal][cluster] %s_base=%x %s_end=%x",
                mem_layout[i].desc,
                mem_layout[i].vbase,
                mem_layout[i].desc,
                mem_layout[i].vend);
    }
    kprintf("[hal][cluster] user_base=%x   user_end=%x",
            USER_BASE_VIRT,
            USER_END_VIRT);
    kprintf("[hal][cluster] memsize=%d MB kmem=%d KB kpool=%d KB umem=%d KB",
            MEMORY_SIZE / MB,
            KMEM_SIZE / KB,
            KPOOL_SIZE / KB,
            UMEM_SIZE / KB);
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
static void mem_check_align(void)
{
    int i; /* Loop index. */

    /* These should be aligned at page boundaries. */
    for (i = MREGION_PG_ALIGN_START; i < MREGION_PG_ALIGN_END; i++) {
        if (mem_layout[i].vbase & (PAGE_SIZE - 1))
            kpanic("%s base address misaligned", mem_layout[i].desc);
        if (mem_layout[i].vend & (PAGE_SIZE - 1))
            kpanic("%s end address misaligned", mem_layout[i].desc);
    }

    /* These should be aligned at page table boundaries. */
    for (i = MREGION_PT_ALIGN_START; i < MREGION_PT_ALIGN_END; i++) {
        if (mem_layout[i].vbase & (PGTAB_SIZE - 1))
            kpanic("%s base address misaligned", mem_layout[i].desc);
        if (mem_layout[i].vend & (PGTAB_SIZE - 1))
            kpanic("%s end address misaligned", mem_layout[i].desc);
    }

    if (USER_BASE_VIRT & (PGTAB_SIZE - 1))
        kpanic("user base address misaligned");
    if (USER_END_VIRT & (PGTAB_SIZE - 1))
        kpanic("user end address misaligned");
}

/**
 * @brief Asserts the memory layout.
 *
 * Checks if the memory layout is as expected, i.e: if the virtual
 * and physical memories are identity mapped.
 */
static void mem_check_layout(void)
{
    int i; /* Loop index. */

    /*
     * These should be identity mapped, because the this is called
     * with paging disabled.
     */
    for (i = 0; i < MEM_REGIONS; i++) {
        if (mem_layout[i].vbase != mem_layout[i].pbase) {
            kpanic("%s base address is not identity mapped",
                   mem_layout[i].desc);
        }
        if (mem_layout[i].vend != mem_layout[i].pend) {
            kpanic("%s end address is not identity mapped", mem_layout[i].desc);
        }
    }
}

/**
 * @brief Builds the memory layout.
 *
 * For each memory region, maps each page into the underlying
 * page table and maps the page table.
 *
 * @author Davidson Francis
 */
static void mem_map(void)
{
    /* Clean root page directory. */
    for (int i = 0; i < PGDIR_LENGTH; i++)
        pde_clear(&cluster_root_pgdir[i]);

    /* Build root address space. */
    for (int i = 0; i < MEM_REGIONS; i++) {
        paddr_t pbase = mem_layout[i].pbase;
        vaddr_t vbase = mem_layout[i].vbase;
        size_t size = mem_layout[i].size;
        int w = mem_layout[i].writable;
        int x = mem_layout[i].executable;

        /* Map underlying pages. */
        for (paddr_t j = pbase, k = vbase; k < (pbase + size);
             j += PAGE_SIZE, k += PAGE_SIZE) {
            mmu_page_map(
                cluster_root_pgtabs[mem_layout[i].root_pgtab_num], j, k, w, x);
        }

        /*
         * Map underlying page table.
         *
         * It is important to note that there are no problems to
         * map multiple times the same page table.
         */
        mmu_pgtab_map(cluster_root_pgdir,
                      PADDR(cluster_root_pgtabs[mem_layout[i].root_pgtab_num]),
                      TRUNCATE(vbase, PGTAB_SIZE));
    }

    /* Load virtual address space and enable MMU. */
    tlb_load(PADDR(cluster_root_pgdir));
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details The memory_init() function initializes the memory interface.
 */
void memory_init(void)
{
    kprintf("[hal][memory] initializing memory layout...");

    mem_info();

    /* Check for memory layout. */
    mem_check_align();
    mem_check_layout();

    mem_map();
}

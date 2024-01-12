/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Maps a page.
 */
int mmu_page_map(struct pte *pgtab, paddr_t paddr, vaddr_t vaddr, int w, int x)
{
    UNUSED(x);

    // Check for invalid page table.
    if (UNLIKELY(pgtab == NULL)) {
        return (-1);
    }

    const int idx = pte_idx_get(vaddr);

    // Check for busy page table entry.
    if (pgtab[idx].present) {
        return (-1);
    }

    pgtab[idx].present = 1;
    pgtab[idx].writable = (w) ? 1 : 0;
    pgtab[idx].frame = FRAME(paddr >> PAGE_SHIFT);

    return (0);
}

/**
 * @details Maps a page table.
 */
int mmu_pgtab_map(struct pde *pgdir, paddr_t paddr, vaddr_t vaddr)
{
    // Check for invalid page directory.
    if (UNLIKELY(pgdir == NULL)) {
        return (-1);
    }

    const int idx = pde_idx_get(vaddr);

    // Check for busy page directory entry.
    if (pgdir[idx].present) {
        return (-1);
    }

    pgdir[idx].writable = 1;
    pgdir[idx].present = 1;
    pgdir[idx].frame = FRAME(paddr >> PAGE_SHIFT);

    return (0);
}

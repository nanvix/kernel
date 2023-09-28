/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
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
 * @todo TODO provide a detailed description for this function.
 */
int mmu_page_map(struct pte *pgtab, paddr_t paddr, vaddr_t vaddr, int w, int x)
{
    UNUSED(x);

    /* Invalid page table. */
    if (UNLIKELY(pgtab == NULL)) {
        return (-1);
    }

    const int idx = pte_idx_get(vaddr);

    pgtab[idx].present = 1;
    pgtab[idx].writable = (w) ? 1 : 0;
    pgtab[idx].frame = FRAME(paddr >> PAGE_SHIFT);

    return (0);
}

/**
 * @todo TODO provide a detailed description for this function.
 *
 * @author Pedro Henrique Penna
 */
int mmu_pgtab_map(struct pde *pgdir, paddr_t paddr, vaddr_t vaddr)
{
    /* Invalid page directory. */
    if (UNLIKELY(pgdir == NULL)) {
        return (-1);
    }

    const int idx = pde_idx_get(vaddr);

    pgdir[idx].writable = 1;
    pgdir[idx].present = 1;
    pgdir[idx].frame = FRAME(paddr >> PAGE_SHIFT);

    return (0);
}

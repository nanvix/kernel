/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Megabyte shift
 */
#define MEGABYTE_SHIFT 20

/**
 * @brief Page directory address offset.
 */
#define PGDIR_ADDR_OFFSET                                                      \
    (((1UL << (VADDR_BIT - MEGABYTE_SHIFT)) >> (VADDR_BIT - PGTAB_SHIFT))      \
     << MEGABYTE_SHIFT)

/**
 * @brief Page directory last valid address.
 */
#define PGDIR_ADDR_END                                                         \
    (((1UL << (VADDR_BIT - MEGABYTE_SHIFT)) -                                  \
      (PGDIR_ADDR_OFFSET >> MEGABYTE_SHIFT))                                   \
     << MEGABYTE_SHIFT)

/**
 * @brief Page table last valid address.
 */
#define PGTAB_ADDR_END ((PGDIR_ADDR_OFFSET) - (PAGE_SIZE))

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details The mmu_page_walk function does a page walk in the system
 * and returns the virtual address of the page belonging the
 * given physical address.
 */
void *mmu_page_walk(struct pde *pgdir, paddr_t paddr)
{
    paddr_t paddr_aligned; /* Physical address aligned.       */
    vaddr_t vaddr_pgdir;   /* Page dir loop index.            */
    vaddr_t vaddr_pgtab;   /* Page tab loop index.            */
    vaddr_t vaddr;         /* Virtual address found.          */

    struct pte *pte;   /* Working page table table entry. */
    struct pde *pde;   /* Working page directory entry.   */
    struct pte *pgtab; /* Working page table.             */

    vaddr = 0;
    paddr_aligned = (paddr & PAGE_MASK);

    for (vaddr_pgdir = 0; vaddr_pgdir < PGDIR_ADDR_END;
         vaddr_pgdir += PGDIR_ADDR_OFFSET) {
        pde = pde_get(pgdir, vaddr_pgdir);
        if (!pde_is_present(pde)) {
            continue;
        }

        pgtab = (struct pte *)(pde_frame_get(pde) << PAGE_SHIFT);

        for (vaddr_pgtab = 0; vaddr_pgtab < PGTAB_ADDR_END;
             vaddr_pgtab += PAGE_SIZE) {
            pte = pte_get(pgtab, vaddr_pgdir + vaddr_pgtab);
            if (!pte_is_present(pte))
                continue;

            if ((pte_frame_get(pte) << PAGE_SHIFT) == paddr_aligned) {
                vaddr = vaddr_pgdir + vaddr_pgtab;
                goto out;
            }
        }
    }

out:
    if (!vaddr)
        return (NULL);
    else
        return (void *)(vaddr + (paddr - paddr_aligned));
}

/**
 * @details This function initializes the MMU.
 */
void mmu_init(void)
{
    kprintf(MODULE_NAME "INFO: initializing...");
    test_mmu();
}

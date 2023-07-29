/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_MMU_H_
#define NANVIX_KERNEL_HAL_MMU_H_

#include <nanvix/kernel/hal/arch.h>

/*============================================================================*
 * Macros                                                                     *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @name Casters for Memory Types
 */
/**@{*/
#define PADDR(x) ((paddr_t)(x)) /** To Physical Address */
#define VADDR(x) ((vaddr_t)(x)) /** To Virtual Address  */
#define FRAME(x) ((frame_t)(x)) /** To Frame Number     */
/**@}*/

#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Maps a page.
 *
 * @param pgtab Target page table.
 * @param paddr Physical address of the target page frame.
 * @param vaddr Virtual address of the target page.
 * @param w     Writable page?
 * @param x     Executable page?
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int mmu_page_map(struct pte *pgtab, paddr_t paddr, vaddr_t vaddr, int w,
                        int x);

/**
 * @brief Maps a page table.
 *
 * @param pgdir Target page directory.
 * @param paddr Physical address of the target page table frame.
 * @param vaddr Virtual address of the target page table.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int mmu_pgtab_map(struct pde *pgdir, paddr_t paddr, vaddr_t vaddr);

/**
 * @brief Searches for a page belonging to a given physical
 * address.
 *
 * The mmu_page_walk function does a page walk in the system
 * and returns the virtual address of the page belonging the
 * given physical address.
 *
 * @param pgdir Page directory.
 * @param paddr Physical address.
 * @return Returns the virtual address of the page, if not found,
 * null.
 */
extern void *mmu_page_walk(struct pde *pgdir, paddr_t paddr);

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_MMU_H_ */

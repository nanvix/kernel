/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_MMU_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_MMU_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <arch/x86.h>
#include <nanvix/cc.h>
#include <nanvix/libcore.h>

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Clears a page directory entry.
 *
 * @param pde Target page directory entry.
 */
static inline int pde_clear(struct pde *pde)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    __memset(pde, 0, PTE_SIZE);

    return (0);
}

/**
 * @brief Sets the frame of a page table.
 *
 * @param pde Page directory entry of target page table.
 * @param frame Frame number.
 */
static inline int pde_frame_set(struct pde *pde, frame_t frame)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    /* Invalid frame. */
    if (frame > ~(frame_t)((1 << (VADDR_BIT - PAGE_SHIFT)) - 1)) {
        return (-1);
    }

    pde->frame = frame;

    return (0);
}

/**
 * @brief Sets/clears the present bit of a page table.
 *
 * @param pde Page directory entry of target page table.
 * @param set Set bit?
 */
static inline int pde_present_set(struct pde *pde, int set)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    pde->present = (set) ? 1 : 0;

    return (0);
}

/**
 * @brief Asserts if the present bit of a page table is set.
 *
 * @param pde Page directory entry of target page table.
 *
 * @returns If the present bit of the target page table is set,
 * non zero is returned. Otherwise, zero is returned instead.
 */
static inline int pde_is_present(struct pde *pde)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    return (pde->present);
}

/**
 * @brief Gets the frame number of a page directory entry.
 *
 * @param pde Target page directory entry.
 *
 * @returns The frame number of the target page directory entry.
 */
static inline frame_t pde_frame_get(struct pde *pde)
{
    return (pde->frame);
}

/**
 * @brief Sets/clears the read bit of a page table.
 *
 * @param pde Page directory entry of target page table.
 * @param set Set bit?
 */
static inline int pde_read_set(struct pde *pde, int set)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    UNUSED(set);

    return (0);
}

/**
 * @brief Asserts if the read bit of a page table is set.
 *
 * @param pde Page directory entry of target page table.
 *
 * @returns If the read bit of the target page table is set, non
 * zero is returned. Otherwise, zero is returned instead.
 */
static inline int pde_is_read(struct pde *pde)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    return (1);
}

/**
 * @brief Sets/clears the write bit of a page table.
 *
 * @param pde Page directory entry of target page table.
 * @param set Set bit?
 */
static inline int pde_write_set(struct pde *pde, int set)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    pde->writable = (set) ? 1 : 0;

    return (0);
}

/**
 * @brief Asserts if the write bit of a page table is set.
 *
 * @param pde Page directory entry of target page table.
 *
 * @returns If the write bit of the target page table is set, non
 * zero is returned. Otherwise, zero is returned instead.
 */
static inline int pde_is_write(struct pde *pde)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    return (pde->writable);
}

/**
 * @brief Sets/clears the exec bit of a page table.
 *
 * @param pde Page directory entry of target page table.
 * @param set Set bit?
 */
static inline int pde_exec_set(struct pde *pde, int set)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    UNUSED(set);

    return (0);
}

/**
 * @brief Asserts if the exec bit of a page table is set.
 *
 * @param pde Page directory entry of target page table.
 *
 * @returns If the exec bit of the target page table is set, non
 * zero is returned. Otherwise, zero is returned instead.
 */
static inline int pde_is_exec(struct pde *pde)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    return (1);
}

/**
 * @brief Sets/clears the user bit of a page table.
 *
 * @param pde Page directory entry of target page table.
 * @param set Set bit?
 */
static inline int pde_user_set(struct pde *pde, int set)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    pde->user = (set) ? 1 : 0;

    return (0);
}

/**
 * @brief Asserts if the user bit of a page table is set.
 *
 * @param pde Page directory entry of target page table.
 *
 * @returns If the user bit of the target page table is set, non
 * zero is returned. Otherwise, zero is returned instead.
 */
static inline int pde_is_user(struct pde *pde)
{
    /* Invalid PDE. */
    if (pde == NULL) {
        return (-1);
    }

    return (pde->user);
}

/**
 * @brief Clears a page table entry.
 *
 * @param pte Target page table entry.
 */
static inline int pte_clear(struct pte *pte)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    __memset(pte, 0, PTE_SIZE);

    return (0);
}

/**
 * @brief Sets/clears the present bit of a page.
 *
 * @param pte Page table entry of target page.
 * @param set Set bit?
 */
static inline int pte_present_set(struct pte *pte, int set)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    pte->present = (set) ? 1 : 0;

    return (0);
}

/**
 * @brief Asserts if the present bit of a page.
 *
 * @param pte Page table entry of target page.
 *
 * @returns If the present bit of the target page, non zero is
 * returned. Otherwise, zero is returned instead.
 */
static inline int pte_is_present(struct pte *pte)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    return (pte->present);
}

/**
 * @brief Sets the frame of a page.
 *
 * @param pte   Page table entry of target page.
 * @param frame Frame number.
 */
static inline int pte_frame_set(struct pte *pte, frame_t frame)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    /* Invalid frame. */
    if (frame > ~(frame_t)((1 << (VADDR_BIT - PAGE_SHIFT)) - 1)) {
        return (-1);
    }

    pte->frame = frame;

    return (0);
}

/**
 * @brief Gets the frame linked to page.
 *
 * @param pte   Page table entry of target page.
 *
 * @returns Number of the frame that is linked to the target page.
 */
static inline frame_t pte_frame_get(struct pte *pte)
{
    return (pte->frame);
}

/**
 * @brief Sets/clears the read bit of a page.
 *
 * @param pte Page table entry of target page.
 * @param set Set bit?
 */
static inline int pte_read_set(struct pte *pte, int set)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    UNUSED(set);

    return (0);
}

/**
 * @brief Asserts if the read bit of a page.
 *
 * @param pte Page table entry of target page.
 *
 * @returns If the read bit of the target page, non zero is
 * returned. Otherwise, zero is returned instead.
 */
static inline int pte_is_read(struct pte *pte)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    return (1);
}

/**
 * @brief Sets/clears the write bit of a page.
 *
 * @param pte Page table entry of target page.
 * @param set Set bit?
 */
static inline int pte_write_set(struct pte *pte, int set)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    pte->writable = (set) ? 1 : 0;

    return (0);
}

/**
 * @brief Asserts if the write bit of a page.
 *
 * @param pte Page table entry of target page.
 *
 * @returns If the write bit of the target page, non zero is
 * returned. Otherwise, zero is returned instead.
 */
static inline int pte_is_write(struct pte *pte)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    return (pte->writable);
}

/**
 * @brief Sets/clears the exec bit of a page.
 *
 * @param pte Page table entry of target page.
 * @param set Set bit?
 */
static inline int pte_exec_set(struct pte *pte, int set)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    UNUSED(set);

    return (0);
}

/**
 * @brief Asserts if the exec bit of a page.
 *
 * @param pte Page table entry of target page.
 *
 * @returns If the exec bit of the target page, non zero is
 * returned. Otherwise, zero is returned instead.
 */
static inline int pte_is_exec(struct pte *pte)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    return (1);
}

/**
 * @brief Sets/clears the user bit of a page.
 *
 * @param pte Page table entry of target page.
 * @param set Set bit?
 */
static inline int pte_user_set(struct pte *pte, int set)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    pte->user = (set) ? 1 : 0;

    return (0);
}

/**
 * @brief Asserts if the user bit of a page.
 *
 * @param pte Page table entry of target page.
 *
 * @returns If the user bit of the target page, non zero is
 * returned. Otherwise, zero is returned instead.
 */
static inline int pte_is_user(struct pte *pte)
{
    /* Invalid PTE. */
    if (pte == NULL) {
        return (-1);
    }

    return (pte->user);
}

/**
 * @brief Gets the page table index of a page.
 *
 * @param vaddr Target virtual address.
 *
 * @returns Returns the page table index of the page that lies at
 * address @p vaddr.
 */
static inline unsigned pte_idx_get(vaddr_t vaddr)
{
    return (((unsigned)(vaddr) & (PGTAB_MASK ^ PAGE_MASK)) >> PAGE_SHIFT);
}

/**
 * @brief Gets the page directory index of a page.
 *
 * @param vaddr Target virtual address.
 *
 * @returns Returns the page directory index of the page that lies
 * at address @p vaddr.
 */
static inline unsigned pde_idx_get(vaddr_t vaddr)
{
    return ((unsigned)(vaddr) >> PGTAB_SHIFT);
}

/**
 * @brief Gets a page directory entry.
 *
 * @param pgdir Target page directory.
 * @param vaddr  Target virtual address.
 *
 * @returns The requested page directory entry.
 */
static inline struct pde *pde_get(struct pde *pgdir, vaddr_t vaddr)
{
    /* Invalid page directory. */
    if (pgdir == NULL) {
        return (NULL);
    }

    return (&pgdir[pde_idx_get(vaddr)]);
}

/**
 * @brief Gets a page table entry.
 *
 * @param pgtab Target page table.
 * @param vaddr Target virtual address.
 *
 * @returns The requested page table entry.
 */
static inline struct pte *pte_get(struct pte *pgtab, vaddr_t vaddr)
{
    /* Invalid page table. */
    if (pgtab == NULL) {
        return (NULL);
    }

    return (&pgtab[pte_idx_get(vaddr)]);
}

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_MMU_H_ */

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
#include <nanvix/kernel/mm.h>
#include <stdbool.h>

#if ((PGDIR_LENGTH * PDE_SIZE) > PAGE_SIZE)
#error "page size too small"
#endif

#if ((PGTAB_LENGTH * PTE_SIZE) > PAGE_SIZE)
#error "page size too small"
#endif

/**
 * @brief Asserts an page address.
 *
 * The upool_is_upage() function asserts whether or not the
 * virtual address @p vaddr reffers to a user page.
 *
 * @returns If @p vaddr reffers to a user page, non zero is
 * returned. Otherwise, zero is returned instead.
 */
static inline int upool_is_upage(vaddr_t vaddr)
{
    return (mm_is_uaddr(vaddr));
}

/*============================================================================*
 * pgtab_map()                                                                *
 *============================================================================*/

/**
 * @brief Maps a page table.
 *
 * The pgtab_map() function maps a page table in the page directory
 * pointed to by @p pgdir at the virtual address @p vaddr. The target
 * address should lie in user space.
 *
 * This function fails either if a page table is already mapped for
 * the target virtual address, or a kernel page could not be allocated
 * to accommodate the page table.
 *
 * @param pgdir Target page directory.
 * @param vaddr Target virtual address.
 *
 * @returns Upon successful completion, a pointer to the page
 * directory entry of the mapped page table is returned. Upon failure,
 * @p NULL is returned instead.
 *
 * @retval NULL Target address is already in use.
 * @retval NULL Page table cannot be allocated.
 *
 * @note No error checking is performed on the parameters, because
 * this is an private function of the User Page Pool subsystem, and we
 * trust higher level routines.
 *
 * @see upage_map() and pgtab_unmap().
 */
static struct pde *pgtab_map(struct pde *pgdir, vaddr_t vaddr)
{
    frame_t frame;     /* Working page frame.           */
    struct pde *pde;   /* Working page table directory. */
    struct pte *pgtab; /* Page table.                   */

    /*
     * Align virtual address
     * to page page table boundary.
     */
    vaddr &= PGTAB_MASK;

    pde = pde_get(pgdir, vaddr);

    /* Busy page directory entry. */
    if (pde_is_present(pde)) {
        kprintf("[kernel][mm] busy page table directory entry");
        return (NULL);
    }

    /*
     * Allocate a kernel page to
     * accommodate the page table.
     */
    if ((pgtab = kpage_get(false)) == NULL) {
        kprintf("[kernel][mm] cannot allocate page table");
        return (NULL);
    }

    /*
     * Map kernel page.
     *
     * FIXME: in a multicore platform, we should
     * flush the TLB of each affected core.
     */
    frame = kpool_addr_to_frame(VADDR(pgtab));
    mmu_pgtab_map(pgdir, frame << PAGE_SHIFT, vaddr);
    KASSERT(pde_is_present(pde));

    tlb_flush();

    /*
     * Clear the page table.
     * Note that we do not kmemset(),
     * because the actual way of cleaning
     * PTEs is platform dependent.
     */
    for (unsigned long i = 0; i < PAGE_SIZE / PDE_SIZE; i++)
        pte_clear(&pgtab[i]);

    return (pde);
}

/*============================================================================*
 * pgtab_unmap()                                                              *
 *============================================================================*/

/**
 * @brief Releases a unmaps table.
 *
 * The pgtab_unmap() function unmaps the page table in the page
 * directory pointed to by @p pgdir at the virtual address @p vaddr.
 *
 * This function failes either if any page is yet in use in the target
 * page table, or the kernel page that accommodates the page table
 * itself cannot be released.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 *
 * @note No error checking is performed on the parameters, because
 * this is an private function of the User Page Pool subsystem, and we
 * trust higher level routines.
 *
 * @retval -1  Page table in use.
 * @retval -1    Cannot release underlying kernel page.
 *
 * @see upage_unmap() and pgtab_map().
 *
 * @bug Properly flush TLB in multicores.
 *
 * @author Pedro Henrique Penna
 */
static int pgtab_unmap(struct pde *pgdir, vaddr_t vaddr)
{
    frame_t frame;     /* Working page frame.           */
    struct pde *pde;   /* Working page directory entry. */
    struct pte *pgtab; /* Page table.                   */

    pde = pde_get(pgdir, vaddr);

    /* Nothing to do. */
    if (!pde_is_present(pde))
        return (0);

    /* Retrieve page table. */
    frame = pde_frame_get(pde);
    pgtab = (struct pte *)kpool_frame_to_addr(frame);

    /* Check if the page table is yet in use. */
    for (unsigned long i = 0; i < PAGE_SIZE / PDE_SIZE; i++) {
        /* Yes. */
        if (pte_is_present(&pgtab[i]))
            return (-1);
    }

    /*
     * Unmap page table before releasing the
     * kernel page, because we may fail bellow.
     *
     * FIXME: in a multicore platform, we should
     * flush the TLB of each affected core.
     */
    pde_present_set(pde, 0);
    tlb_flush();

    /* Cannot release kernel page. */
    if (kpage_put(pgtab) < 0) {
        kprintf("[kernel][mm] kernel page leak");
        return (-1);
    }

    return (0);
}

/*============================================================================*
 * upage_inval()                                                              *
 *============================================================================*/

/**
 * @todo Provide a detailed description for this function.
 */
int upage_inval(vaddr_t vaddr)
{
    vaddr &= PAGE_MASK;

    /* Bad virtual address. */
    if (!mm_is_uaddr(vaddr))
        return (-1);

    tlb_flush();

    return (0);
}

/*============================================================================*
 * upage_ctrl()                                                               *
 *============================================================================*/

/**
 * @details Changes access permissions of a user page.
 */
int upage_ctrl(struct pde *pgdir, vaddr_t vaddr, bool w, bool x)
{
    /* Invalid page directory. */
    if (pgdir == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid page directory");
        return (-1);
    }

    /* Bad virtual address. */
    if (!mm_is_uaddr(vaddr)) {
        kprintf(MODULE_NAME " ERROR: bad virtual address");
        return (-1);
    }

    /* Misaligned target address. */
    if (vaddr & (~PAGE_MASK)) {
        kprintf(MODULE_NAME " ERROR: misaligned virtual address");
        return (-1);
    }

    // Retrieve page directory entry of target page.
    struct pde *pde = pde_get(pgdir, vaddr);
    if (!pde_is_present(pde)) {
        kprintf(MODULE_NAME " ERROR: page directory not present");
        return (-1);
    }

    // Retrieve the page table entry of the target page.
    struct pte *pgtab = (struct pte *)(kpool_frame_to_addr(pde_frame_get(pde)));
    struct pte *pte = pte_get(pgtab, vaddr);
    if (!pte_is_present(pte)) {
        kprintf(MODULE_NAME " ERROR: page table not present");
        return (-1);
    }

    pte_write_set(pte, w);
    pte_exec_set(pte, x);

    tlb_flush();

    return (0);
}

/*============================================================================*
 * upage_map()                                                                *
 *============================================================================*/

/**
 * The upage_map() function maps the user page frame @p frame in the
 * page directory pointed to by @p pgdir at virtual address @p vaddr.
 *
 * This function fails either if @p pgdir does not point to a valid
 * page directory, @p vaddr does not lie in user space, @p vaddr is
 * not aligned at a page boundary, a page is already allocated at
 * address @p vaddr, or @p frame does not refer to a user page frame.
 *
 * @todo Check for bad page directory.
 * @todo Check for bad page frame.
 *
 * @bug Properly flush TLB in multicores.
 *
 * @author Pedro Henrique Penna
 */
int upage_map(struct pde *pgdir, vaddr_t vaddr, frame_t frame, bool w, bool x)
{
    struct pte *pte;   /* Working page table table entry. */
    struct pde *pde;   /* Working page directory entry.   */
    struct pte *pgtab; /* Working page table.             */

    /* Invalid page directory. */
    if (pgdir == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid page directory");
        return (-1);
    }

    /* TODO: check for bad page directory. */

    /* Bad virtual address. */
    if (!mm_is_uaddr(vaddr)) {
        kprintf(MODULE_NAME " ERROR: bad virtual address");
        return (-1);
    }

    /* Misaligned target address. */
    if (vaddr & (~PAGE_MASK)) {
        kprintf(MODULE_NAME " ERROR: misaligned virtual address");
        return (-1);
    }

    /* TODO: check for bad page frame. */

    /*
     * Retrieve page directory entry
     * of target page.
     */
    pde = pde_get(pgdir, vaddr);
    if (!pde_is_present(pde)) {
        if ((pde = pgtab_map(pgdir, vaddr)) == NULL)
            return (-1);
        pde_user_set(pde, 1);
    }

    /*
     * Retrieve the page table entry
     * of the target page.
     */
    pgtab = (struct pte *)(kpool_frame_to_addr(pde_frame_get(pde)));
    pte = pte_get(pgtab, vaddr);

    /*
     * The target page is already mapped.
     * Fortunately, we have nothing to rollback.
     * If the page is already present,
     * it means that the page table was
     * previously allocated (hopefully no one
     * has tempered the memory).
     */
    if (pte_is_present(pte))
        return (-1);

    /*
     * Map page.
     *
     * FIXME: in a multicore platform, we should
     * flush the TLB of each affected core core.
     */
    mmu_page_map(pgtab, frame << PAGE_SHIFT, vaddr, w, x);
    pte_user_set(pte, 1);
    KASSERT(pte_is_present(pte));

    tlb_flush();

    return (0);
}

/*============================================================================*
 * upage_unmap()                                                              *
 *============================================================================*/

/**
 * The upage_unmap() function unmaps the user page that was previously
 * mapped by a call to upage_map() in the page directory pointed to by
 * @p pgdir at virtual address @p vaddr.
 *
 * This function fails either if @p pgdir points to an invalid page
 * directory, @p vaddr does not lie in user space, @p vaddr is not
 * aligned at a page boundary, or no page is allocated at address @p
 * vaddr.
 *
 * @note If the kernel is compiled with @p NANVIX_FAST_MEMORY option,
 * the kernel does not try to release underlying page tables.
 *
 * @todo Check for bad page directory.
 *
 * @bug Properly flush TLB in multicores.
 *
 * @author Pedro Henrique Penna
 */
frame_t upage_unmap(struct pde *pgdir, vaddr_t vaddr)
{
    frame_t frame;     /* Frame.                                 */
    struct pte *pte;   /* Page table table entry of target page. */
    struct pde *pde;   /* Page directory entry of target page.   */
    struct pte *pgtab; /* Page table of target page.             */

    /* Invalid page directory. */
    if (pgdir == NULL)
        return (FRAME_NULL);

    /* TODO: check for bad page directory. */

    /* Bad virtual address. */
    if (!mm_is_uaddr(vaddr))
        return (FRAME_NULL);

    /* Misaligned target address. */
    if (vaddr & (~PAGE_MASK))
        return (FRAME_NULL);

    /* Look up page table. */
    pde = pde_get(pgdir, vaddr);

    /* Target address not in use. */
    if (!pde_is_present(pde))
        return (FRAME_NULL);

    /*
     * Retrieve page table entry
     * of target page.
     */
    pgtab = (struct pte *)(kpool_frame_to_addr(pde_frame_get(pde)));
    pte = pte_get(pgtab, vaddr);

    /* Target address not in use. */
    if (!pte_is_present(pte))
        return (FRAME_NULL);

    /*
     * Unmap page.
     *
     * FIXME: in a multicore platform, we should
     * flush the TLB of each affected core core.
     */
    frame = pte_frame_get(pte);
    pte_present_set(pte, 0);

    upage_inval(vaddr);

    /* Free underlying page tables. */
#ifndef NANVIX_FAST_MEMORY
    pgtab_unmap(pgdir, vaddr);
#endif

    return (frame);
}

/*============================================================================*
 * upage_alloc()                                                              *
 *============================================================================*/

/**
 * @details This function allocates a user page in the page
 * directory pointed to by @p pgdir at virtual address @p vaddr.
 *
 * This function fails either if @p pgdir does not point to a valid
 * page directory, @p vaddr does not lie in user space, @p vaddr is
 * not aligned at a page boundary, a page is already allocated at
 * address @p vaddr, or a page frame cannot be allocated to
 * accommodate the new page.
 */
int upage_alloc(struct pde *pgdir, vaddr_t vaddr, bool w, bool x)
{
    int err;       /* Error code. */
    frame_t frame; /* Page frame. */

    if (pgdir == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid page directory");
        return (-1);
    }

    /*
     * upage_map() checks the
     * other necessary requirements.
     */

    /* Allocate user frame. */
    if ((frame = frame_alloc_any()) == FRAME_NULL) {
        return (-1);
    }

    /* Map user page. */
    if ((err = upage_map(pgdir, vaddr, frame, w, x)) < 0) {
        if (frame_free(frame) < 0)
            kprintf("[kernel][mm] page frame leak");

        return (err);
    }

    __memset((void *)(vaddr), 0, PAGE_SIZE);

    return (0);
}

/*============================================================================*
 * upage_free()                                                               *
 *============================================================================*/

/**
 * @details The upage_free() function releases a user page that was previously
 * allocated by a call to upage_alloc() in the page directory pointed
 * to by @p pgdir at virtual address @p vaddr.
 *
 * This function fails either if @p pgdir points to an invalid page
 * directory, @p vaddr does not lie in user space, @p vaddr is not
 * aligned at a page boundary, no page is allocated at address @p
 * vaddr, or the underlying page frame cannot be released.
 */
int upage_free(struct pde *pgdir, vaddr_t vaddr)
{
    frame_t frame;

    if (pgdir == NULL)
        return (-1);

    /*
     * upage_map() checks the
     * other necessary requirements.
     */

    /* Unmap user page. */
    if ((frame = upage_unmap(pgdir, vaddr)) == FRAME_NULL)
        return (-1);

    /* Release page frame. */
    if (frame_free(frame) < 0) {
        kprintf("[kernel][mm] page frame leak");

        return (-1);
    }

    return (0);
}

/*============================================================================*
 * upage_link()                                                               *
 *============================================================================*/

/**
 * @todo TODO: provide a detailed description for this function.
 */
int upage_link(struct pde *pgdir, vaddr_t vaddr1, vaddr_t vaddr2)
{
    frame_t frame;     /* Underlying page frame.          */
    struct pte *pte1;  /* Working page table table entry. */
    struct pde *pde1;  /* Working page directory entry.   */
    struct pte *pgtab; /* Working page table.             */

    /* Invalid page directory. */
    if (pgdir == NULL)
        return (-1);

    /* TODO: check for bad page directory. */

    /* Bad virtual address. */
    if (!mm_is_uaddr(vaddr2))
        return (-1);

    /* Misaligned target address. */
    if (vaddr2 & (~PAGE_MASK))
        return (-1);

    /*
     * Retrieve page directory entry
     * of target page.
     */
    pde1 = pde_get(pgdir, vaddr1);
    if (!pde_is_present(pde1))
        return (-1);

    /*
     * Retrieve the page table entry
     * of the target page.
     */
    pgtab = (struct pte *)(kpool_frame_to_addr(pde_frame_get(pde1)));
    pte1 = pte_get(pgtab, vaddr1);

    /* Unampped source page. */
    if (!pte_is_present(pte1))
        return (-1);

    frame = pte_frame_get(pte1);
    const bool w = pte_is_write(pte1);
    const bool x = pte_is_exec(pte1);

    return (upage_map(pgdir, vaddr2, frame, w, x));
}

/*============================================================================*
 * upool_init()                                                               *
 *============================================================================*/

/**
 * The upool_init() function initializes internal structures of the
 * user page allocator. Additionally, if the kernel is compiled
 * without the @p NDEBUG build option, unit tests on the user page
 * allocator are launched once its initialization is completed.
 */
void upool_init(void)
{
    kprintf("[kernel][mm] initializing the user page allocator");
}

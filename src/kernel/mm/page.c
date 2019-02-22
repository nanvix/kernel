/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <nanvix/hal/hal.h>
#include <nanvix/const.h>
#include <nanvix/mm.h>
#include <nanvix/klib.h>
#include <errno.h>

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
 * @bug Properly flush TLB in multicores.
 *
 * @see upage_map() and pgtab_unmap().
 *
 * @author Pedro Henrique Penna
 */
PRIVATE inline struct pde *pgtab_map(struct pde *pgdir, vaddr_t vaddr)
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
	if (pde_is_present(pde))
	{
		kprintf("[mm] busy page table directory entry");
		return (NULL);
	}

	/*
	 * Allocate a kernel page to
	 * accommodate the page table.
	 */
	if ((pgtab = kpage_get(FALSE)) == NULL)
		return (NULL);

	/*
	 * Map kernel page.
	 *
	 * FIXME: in a multicore platform, we should
	 * flush the TLB of each affected core.
	 */
	frame = kpool_addr_to_frame(VADDR(pgtab));
	pde_frame_set(pde, frame);
	pde_present_set(pde, 1);
	pde_user_set(pde, 1);
	pde_write_set(pde, 1);
	tlb_flush();

	/*
	 * Clear the page table.
	 * Note that we do not kmemset(),
	 * because the actual way of cleaning
	 * PTEs is platform dependent.
	 */
	for (int i = 0; i < PAGE_SIZE/PDE_SIZE; i++)
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
 * @retval -EBUSY  Page table in use.
 * @retval -EIO    Cannot release underlying kernel page.
 *
 * @see upage_unmap() and pgtab_map().
 *
 * @bug Properly flush TLB in multicores.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE int pgtab_unmap(struct pde *pgdir, vaddr_t vaddr)
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
	pgtab = (struct pte *) kpool_frame_to_addr(frame);

	/* Check if the page table is yet in use. */
	for (int i = 0; i < PAGE_SIZE/PDE_SIZE; i++)
	{
		/* Yes. */
		if (pte_is_present(&pgtab[i]))
			return (-EBUSY);
	}

	/*
	 * Unmap page table before releasing the
	 * kernel page, because we may fail bellow.
	 *
	 * FIXME: in a multicore platform, we should
	 * flush the TLB of each affected core.
	 */
	pde_present_set(pde, 0);
	hal_dcache_invalidate();
	tlb_flush();

	/* Cannot release kernel page. */
	if (kpage_put(pgtab) < 0)
	{
		kprintf("[mm] kernel page leak");
		return (-EIO);
	}

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
 * @retval -EADDRINUSE Target address is already in use.
 * @retval -EINVAL     Target address is not properly aligned.
 * @retval -EFAULT     Target address does not lie in user space.
 * @retval -EINVAL     Invalid page directory.
 *
 * @todo Check for bad page directory.
 * @todo Check for bad page frame.
 *
 * @bug Properly flush TLB in multicores.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int upage_map(struct pde *pgdir, vaddr_t vaddr, frame_t frame)
{
	struct pte *pte;   /* Working page table table entry. */
	struct pde *pde;   /* Working page directory entry.   */
	struct pte *pgtab; /* Working page table.             */

	/* Invalid page directory. */
	if (pgdir == NULL)
		return (-EINVAL);

	/* TODO: check for bad page directory. */

	/* Bad virtual address. */
	if (!mm_is_uaddr(vaddr))
		return (-EFAULT);

	/* Misaligned target address. */
	if (vaddr & (~PAGE_MASK))
		return (-EINVAL);

	/* TODO: check for bad page frame. */

	/*
	 * Retrieve page directory entry
	 * of target page.
	 */
	pde = pde_get(pgdir, vaddr);
	if (!pde_is_present(pde))
		pde = pgtab_map(pgdir, vaddr);

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
		return (-EADDRINUSE);

	/*
	 * Map page.
	 *
	 * FIXME: in a multicore platform, we should
	 * flush the TLB of each affected core core.
	 */
	pte_present_set(pte, 1);
	pte_user_set(pte, 1);
	pte_write_set(pte, 0);
	pte_frame_set(pte, frame);
	hal_dcache_invalidate();
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
 * @retval FRAME_NULL Invalid page directory.
 * @retval FRAME_NULL Target address does not lie in user space.
 * @retval FRAME_NULL Target address is not properly aligned.
 * @retval FRAME_NULL Target address is not in use.
 *
 * @todo Check for bad page directory.
 *
 * @bug Properly flush TLB in multicores.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC frame_t upage_unmap(struct pde *pgdir, vaddr_t vaddr)
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
	tlb_inval(EXCP_DTLB_FAULT, vaddr);
	tlb_inval(EXCP_ITLB_FAULT, vaddr);
	tlb_flush();

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
 * The upage_alloc() function allocates a user page in the page
 * directory pointed to by @p pgdir at virtual address @p vaddr.
 *
 * This function fails either if @p pgdir does not point to a valid
 * page directory, @p vaddr does not lie in user space, @p vaddr is
 * not aligned at a page boundary, a page is already allocated at
 * address @p vaddr, or a page frame cannot be allocated to
 * accommodate the new page.
 *
 * @retval -EAGAIN     Cannot allocate a page frame.
 * @retval -EADDRINUSE Target address is already in use.
 * @retval -EINVAL     Target address is not properly aligned.
 * @retval -EFAULT     Target address does not lie in user space.
 * @retval -EINVAL     Invalid page directory.
 *
 * @note If the kernel is compiled with @p NANVIX_FAST_MEMORY option,
 * no memory clean is performed after a successful page allocation.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int upage_alloc(struct pde *pgdir, vaddr_t vaddr)
{
	int err;       /* Error code. */
	frame_t frame; /* Page frame. */

	/*
	 * We handle argument checking
	 * to upage_map().
	 */

	/* Allocate user frame. */
	if ((frame = frame_alloc()) == FRAME_NULL)
		return (-EAGAIN);

	/* Map user page. */
	if ((err = upage_map(pgdir, vaddr, frame)) < 0)
	{
		if (frame_free(frame) < 0)
			kprintf("[mm] page frame leak");

		return (err);
	}

#ifndef NANVIX_FAST_MEMORY
	kmemset((void *)(vaddr), 0, PAGE_SIZE);
#endif

	return (0);
}

/*============================================================================*
 * upage_free()                                                               *
 *============================================================================*/

/**
 * The upage_free() function releases a user page that was previously
 * allocated by a call to upage_alloc() in the page directory pointed
 * to by @p pgdir at virtual address @p vaddr.
 *
 * This function fails either if @p pgdir points to an invalid page
 * directory, @p vaddr does not lie in user space, @p vaddr is not
 * aligned at a page boundary, no page is allocated at address @p
 * vaddr, or the underlying page frame cannot be released.
 *
 * @retval -EFAULT Invalid page directory.
 * @retval -EFAULT Target address does not lie in user space.
 * @retval -EFAULT Target address is not properly aligned.
 * @retval -EFAULT Target address is not in use.
 * @retval -EIO    Cannot release underlying page frame.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int upage_free(struct pde *pgdir, vaddr_t vaddr)
{
	frame_t frame;

	/*
	 * We forward argument checking
	 * to upage_unmap().
	 */

	/* Unmap user page. */
	if ((frame = upage_unmap(pgdir, vaddr)) == FRAME_NULL)
		return (-EFAULT);

	/* Release page frame. */
	if (frame_free(frame) < 0)
	{
		kprintf("[mm] page frame leak");

		return (-EIO);
	}

	return (0);
}

/*============================================================================*
 * do_tlb_fault()                                                             *
 *============================================================================*/

#ifdef HAL_TLB_SOFTWARE

/**
 * @brief Handles a TLB fault.
 *
 * The do_tlb_fault() function handles a TLB fault. It checks the
 * current page directory for a virtual-to-physical address mapping,
 * and if it finds one, it writes this mapping to the TLB. If the
 * faulting address is not currently mapped in the current page
 * directory, this exception is forwarded to the page fault handler.
 *
 * @param excp Exception information.
 * @param ctx  Interrupted execution context.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void do_tlb_fault(
	const struct exception *excp,
	const struct context *ctx
)
{
	paddr_t paddr;     /* Physical address.               */
	vaddr_t vaddr;     /* Faulting address.               */
	struct pte *pte;   /* Working page table table entry. */
	struct pde *pde;   /* Working page directory entry.   */
	struct pte *pgtab; /* Working page table.             */

	UNUSED(ctx);

	/* Get page address of faulting address. */
	vaddr = exception_get_addr(excp);
	vaddr &= PAGE_MASK;

#ifdef NDEBUG

	/*
	 * The faulting address lies in kernel
	 * land and this is unlikely to happen.
	 * So, it's better to get some warning.
	 */
	if (!mm_is_uaddr(vaddr))
		kprintf("[mm] tlb fault in kernel land");

#endif

	/* Lookup PDE. */
	pde = pde_get(idle_pgdir, vaddr);
	if (!pde_is_present(pde))
	{
		forward_excp(EXCP_PAGE_FAULT, excp, ctx);
		return;
	}

	/* Lookup PTE. */
	pgtab = (struct pte *)(pde_frame_get(pde) << PAGE_SHIFT);
	pte = pte_get(pgtab, vaddr);
	if (!pte_is_present(pte))
	{
		forward_excp(EXCP_PAGE_FAULT, excp, ctx);
		return;
	}

	/* Writing mapping to TLB. */
	paddr = pte_frame_get(pte) << PAGE_SHIFT;
	if (tlb_write(excp->num, vaddr, paddr) < 0)
		kpanic("cannot write to tlb");
}

#endif

/*============================================================================*
 * upool_init()                                                               *
 *============================================================================*/

/**
 * The upool_init() function initializes internal structures of the
 * user page allocator. Additionally, if the kernel is compiled
 * without the @p NDEBUG build option, unit tests on the user page
 * allocator are launched once its initialization is completed.
 */
PUBLIC void upool_init(void)
{
	kprintf("[mm] initializing the user page allocator");

	/* Register handlers. */
#ifdef HAL_TLB_SOFTWARE
	exception_set_handler(EXCP_DTLB_FAULT, do_tlb_fault);
	exception_set_handler(EXCP_ITLB_FAULT, do_tlb_fault);
#endif

#ifndef NDEBUG
	kprintf("[mm] running tests on the user page allocator");
	upool_test_driver();
#endif
}

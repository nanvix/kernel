/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#include <arch/or1k/excp.h>
#include <arch/or1k/tlb.h>
#include <nanvix/mm.h>
#include <nanvix/const.h>

/**
 * @brief Handles a page fault.
 *
 * The or1k_do_page_fault() function is currently a dummy handler for a
 * page fault. It prints the faulting address and panics the kernel.
 *
 * @param excp Exception information.
 * @param ctx  Interrupted execution context.
 *
 * @todo Implement a rich handler.
 */
PRIVATE void or1k_do_page_fault(
	const struct exception *excp,
	const struct context *ctx
)
{
	UNUSED(ctx);

	kpanic("[mm] page fault at %x", exception_get_addr(excp));
}

/**
 * @brief Handles a TLB fault.
 *
 * The or1k_do_tlb_fault() function handles a TLB fault. It checks the
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
PRIVATE void or1k_do_tlb_fault(
	const struct exception *excp,
	const struct context *ctx
)
{
	paddr_t paddr;     /* Physical address.               */
	vaddr_t vaddr;     /* Faulting address.               */
	struct pte *pte;   /* Working page table table entry. */
	struct pde *pde;   /* Working page directory entry.   */
	struct pte *pgtab; /* Working page table.             */

	/* Get page address of faulting address. */
	vaddr = or1k_excp_get_addr(excp);
	vaddr &= OR1K_PAGE_MASK;

	/* Lookup PDE. */
	pde = pde_get(idle_pgdir, vaddr);
	if (!pde_is_present(pde))
		or1k_do_page_fault(excp, ctx);

	/* Lookup PTE. */
	pgtab = (struct pte *)(pde_frame_get(pde) << OR1K_PAGE_SHIFT);
	pte = pte_get(pgtab, vaddr);
	if (!pte_is_present(pte))
		or1k_do_page_fault(excp, ctx);

	/* Writing mapping to TLB. */
	paddr = pte_frame_get(pte) << OR1K_PAGE_SHIFT;
	if (or1k_tlb_write(excp->num, vaddr, paddr) < 0)
		kpanic("cannot write to tlb");
}

/**
 * Initializes the core components for or1k.
 */
PUBLIC void or1k_core_setup(void)
{
	/* TLB Handler. */
	exception_set_handler(EXCP_DTLB_FAULT, or1k_do_tlb_fault);
	exception_set_handler(EXCP_ITLB_FAULT, or1k_do_tlb_fault);

	/* Initial TLB. */
	or1k_tlb_init();
}

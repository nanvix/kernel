/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#define NANVIX_HAL_TARGET_H_

#include <arch/k1b/cpu.h>
#include <arch/k1b/mmu.h>
#include <arch/k1b/tlb.h>
#include <target/kalray/mppa256.h>
#include <nanvix/const.h>

/**
 * @brief Length of virtual addresses.
 *
 * Number of bits in a virtual address.
 *
 * @author Pedro Henrique Penna
 */
#define K1B_VADDR_LENGTH 32

/**
 * @brief Page Directory length.
 *
 * Number of Page Directory Entries (PDEs) per Page Directory.
 *
 * @author Pedro Henrique Penna
 */
#define K1B_PGDIR_LENGTH (1 << (K1B_VADDR_LENGTH - K1B_PGTAB_SHIFT))

/**
 * @brief Page Table length.
 *
 * Number of Page Table Entries (PTEs) per Page Table.
 *
 * @author Pedro Henrique Penna
 */
#define K1B_PGTAB_LENGTH (1 << (K1B_PGTAB_SHIFT - K1B_PAGE_SHIFT))

/**
 * @brief Root Page Table.
 */
PRIVATE struct pte root_pgtab[K1B_PGTAB_LENGTH] __attribute__((aligned(K1B_PAGE_SIZE)));

/**
 * @brief Root Page Directories.
 */
PRIVATE struct pde root_pgdir[K1B_PGDIR_LENGTH] __attribute__((aligned(K1B_PAGE_SIZE)));

/**
 * Alias to root page directory.
 */
PUBLIC struct pde *idle_pgdir = &root_pgdir[0];

/*
 * Physical Memory Layout
 */
const paddr_t MPPA256_KERNEL_BASE_PHYS  = MPPA256_HYPER_LOW_END_PHYS;                                 /* Kernel Base          */
const paddr_t MPPA256_KERNEL_END_PHYS   = (paddr_t)(&_kend);                                          /* Kernel End           */
const paddr_t MPPA256_KPOOL_BASE_PHYS   = (paddr_t)(&_kend);                                          /* Kernel Pool Base     */
const paddr_t MPPA256_KPOOL_END_PHYS    = (paddr_t)(&_kend) + MPPA256_KPOOL_SIZE;                     /* Kernel Pool End      */
const paddr_t MPPA256_USER_BASE_PHYS    = (paddr_t)(&_kend) + MPPA256_KPOOL_SIZE;                     /* User Base            */
const paddr_t MPPA256_USER_END_PHYS     = (paddr_t)(&_kend) + MPPA256_KPOOL_SIZE + MPPA256_UMEM_SIZE; /* User End             */
const paddr_t MPPA256_KSTACK_BASE_PHYS  = (paddr_t)(&_user_stack_start);                              /* Kernel Stack Base    */

/**
 * Virtual Memory Layout
 */
const vaddr_t MPPA256_KERNEL_BASE_VIRT  = MPPA256_HYPER_LOW_END_VIRT;                                 /* Kernel Base          */
const vaddr_t MPPA256_KERNEL_END_VIRT   = (vaddr_t)(&_kend);                                          /* Kernel End           */
const vaddr_t MPPA256_KPOOL_BASE_VIRT   = (vaddr_t)(&_kend);                                          /* Kernel Pool Base     */
const vaddr_t MPPA256_KPOOL_END_VIRT    = (vaddr_t)(&_kend) + MPPA256_KPOOL_SIZE;                     /* Kernel Pool End      */
const vaddr_t MPPA256_USER_BASE_VIRT    = (vaddr_t)(&_kend) + MPPA256_KPOOL_SIZE;                     /* User Base            */
const vaddr_t MPPA256_USER_END_VIRT     = (vaddr_t)(&_kend) + MPPA256_KPOOL_SIZE + MPPA256_UMEM_SIZE; /* User End             */
const vaddr_t MPPA256_KSTACK_BASE_VIRT  = (vaddr_t)(&_user_stack_start);                              /* Kernel Stack Base    */

/**
 * @brief Map Hypervisor page frames.
 *
 * The mmu_map_hypervisor() function maps page frames of the
 * hypervisor in the page table pointed to by @p pgtab.
 *
 * @param pgtab Target page table.
 */
PRIVATE void mmu_map_hypervisor(struct pte *pgtab)
{
	/* Fill up Low Hypervisor PTEs. */
	for (vaddr_t vaddr = MPPA256_HYPER_LOW_BASE_VIRT;
	             vaddr < MPPA256_HYPER_LOW_END_VIRT;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 0;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}

	/* Fill up High Hypervisor PTEs. */
	for (vaddr_t vaddr = MPPA256_HYPER_HIGH_BASE_VIRT;
	             vaddr < MPPA256_HYPER_HIGH_END_VIRT;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 0;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}
}

/**
 * @brief Map Kernel Code and Data pages.
 *
 * The mmu_map_kernel() function maps page frames of the kernel code
 * and data in the page table pointed to by @p pgtab.
 */
PRIVATE void mmu_map_kernel(struct pte *pgtab)
{
	/* Fill up Kernel Code and Data PTEs. */
	for (vaddr_t vaddr = MPPA256_KERNEL_BASE_PHYS;
	             vaddr < MPPA256_KERNEL_END_PHYS;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 1;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}
}

/**
 * @brief Map Kernel Page Pool pages.
 *
 * The mmu_map_kpool() function maps page frames of the kernel page
 * pool in the page table pointed to by @p pgtab.
 */
PRIVATE void mmu_map_kpool(struct pte *pgtab)
{
	/* Fill up Kernel Page Pool PTEs. */
	for (vaddr_t vaddr = MPPA256_KPOOL_BASE_PHYS;
	             vaddr < MPPA256_KPOOL_END_PHYS;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 1;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}
}

/**
 * @brief Warmups the MMU.
 *
 * The mmu_warmup() function loads the TLB of the underlying core with
 * the initial maping of the system. Overall, it loads Hypervisor the
 * Hypervisor, Kernel and Kernel Page Pool mappings into way one of
 * the architectural TLB, and then it invalidates all entries in way
 * zero.
 *
 * @bug We cannot invalidate entries in way 0 of the TLB.
 */
PRIVATE void mmu_warmup(void)
{
	vaddr_t start, end;

	/* Load Hypervisor entries into the TLB. */
	k1b_tlb_write(
		MPPA256_HYPER_LOW_BASE_VIRT,
		MPPA256_HYPER_LOW_BASE_VIRT,
		K1B_HUGE_PAGE_SHIFT,
		1,
		K1B_TLBE_PROT_RWX
	);
	k1b_tlb_write(
		MPPA256_HYPER_HIGH_BASE_VIRT,
		MPPA256_HYPER_HIGH_BASE_VIRT,
		K1B_HUGE_PAGE_SHIFT,
		1,
		K1B_TLBE_PROT_RWX
	);

	/* Load Kernel entries into the TLB. */
	start = MPPA256_KERNEL_BASE_VIRT; end = MPPA256_KERNEL_END_VIRT;
	for (vaddr_t vaddr = start; vaddr < end; vaddr += K1B_HUGE_PAGE_SIZE)
		k1b_tlb_write(vaddr, vaddr, K1B_HUGE_PAGE_SHIFT, 1, K1B_TLBE_PROT_RWX);

	/* Load Kernel Page Pool entries into the TLB. */
	start = MPPA256_KPOOL_BASE_VIRT; end = MPPA256_KPOOL_END_VIRT;
	for (vaddr_t vaddr = start; vaddr < end; vaddr += K1B_HUGE_PAGE_SIZE)
		k1b_tlb_write(vaddr, vaddr, K1B_HUGE_PAGE_SHIFT, 1, K1B_TLBE_PROT_RW);

	/* Invalidate all entries in way 0. */
	start = 0; end = _MEMORY_SIZE;
	for (vaddr_t vaddr = start; vaddr < end; vaddr += K1B_PAGE_SIZE)
		k1b_tlb_inval(vaddr, K1B_PAGE_SHIFT, 0);
}

/**
 * The k1b_mmu_setup() function initializes the Memory Management Unit
 * (MMU) of the underlying k1b core.
 */
PUBLIC void k1b_mmu_setup(void)
{
	int coreid;

	coreid = k1b_core_get_id();

	kprintf("[core %d][hal] initializing mmu", coreid);

	/*
	 * Master core builds
	 * root page directory.
	 */
	if (coreid == 0)
	{
		kprintf("[core %d][hal] memsize=%d MB kmem=%d KB kpool=%d KB umem=%d KB",
			coreid,
			_MEMORY_SIZE/(1024*1024),
			_KMEM_SIZE/1024,
			_KPOOL_SIZE/1024,
			_UMEM_SIZE/1024
		);

		/* Clean root page table. */
		for (int i = 0; i < K1B_PGTAB_LENGTH; i++)
			pte_clear(&root_pgtab[i]);

		/* Clean root page directory. */
		for (int i = 0; i < K1B_PGDIR_LENGTH; i++)
			pde_clear(&root_pgdir[i]);

		/* Build root page table. */
		mmu_map_hypervisor(root_pgtab);
		mmu_map_kernel(root_pgtab);
		mmu_map_kpool(root_pgtab);

		/* Build root page directory. */
		root_pgdir[0].present = 1;
		root_pgdir[0].writable = 1;
		root_pgdir[0].user = 0;
		root_pgdir[0].frame = (vaddr_t)(root_pgtab) >> K1B_PAGE_SHIFT;
	}

	mmu_warmup();

	k1b_tlb_init();
}

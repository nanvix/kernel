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

#include <arch/k1b/cache.h>
#include <arch/k1b/core.h>
#include <arch/k1b/cpu.h>
#include <arch/k1b/tlb.h>
#include <nanvix/const.h>

/**
 * @brief TLB
 *
 * Architectural Translation Lookaside Buffer (TLB).
 *
 * @note This structure is accessed by all cores in the processor.
 * Therefore, it must be aligned at a cache line boundary, otherwise
 * we could trash the data.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE struct
{
	/**
	 * @brief Join TLB.
	 *
	 * The Join TLB (JTLB) is a 2-way set-associative TLB intended for
	 * caching not-so-used physical to virtual address translations. A
	 * page of 2^n (with n >= 12) may be encoded either in the entry
	 * 2^(2p) or 2^(2p+1), where p is the value of the bit slice
	 * comprising bits n to n+5 of the virtual address corresponding
	 * to the start of the page.
	 */
	struct tlbe jtlb[K1B_JTLB_LENGTH];

	/**
	 * @brief Locked TLB.
	 *
	 * The Locked TLB (LTLB) is a fully associative TLB intended for
	 * caching always-used physical to virtual address translations.
	 * It may may encode in any entry, any virtual to physical address
	 * translation.
	 */
	struct tlbe ltlb[K1B_LTLB_LENGTH];
} __attribute__((aligned(K1B_CACHE_LINE_SIZE))) tlb[K1B_NUM_CORES];

/*============================================================================*
 * k1b_tlbe_idx_get()                                                         *
 *============================================================================*/

/**
 * @brief Gets a TLB entry.
 *
 * @param idx Index of the target entry in the TLB.
 *
 * The k1b_tlbe_get() function returns the architectural TLB entry at
 * position @p idx.
 *
 * @returns Upon successful completion, the architectural TLB entry at
 * position @p idx is returned. Upon failure, @p NULL is returned
 * instead.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE inline const struct tlbe *k1b_tlbe_get(int idx)
{
	int coreid;

	coreid = k1b_core_get_id();

	if ((idx >= 0) && (idx < K1B_JTLB_LENGTH))
		return (&tlb[coreid].jtlb[idx]);

	else if (idx < (K1B_JTLB_LENGTH + K1B_LTLB_LENGTH))
		return (&tlb[coreid].ltlb[(idx - K1B_JTLB_LENGTH)]);

	return (NULL);
}

/*============================================================================*
 * k1b_tlbe_dump()                                                            *
 *============================================================================*/

/**
 * The k1b_tlbe_dump() function dumps information about the TLB entry
 * @p idx on the kernel output device.
 */
PUBLIC void k1b_tlbe_dump(int idx)
{
	const struct tlbe *tlbe;
	static const char *status[4] = {"i", "p", "m", "a"};

	if ((tlbe = k1b_tlbe_get(idx)) == NULL)
	{
		kprintf("no tlb entry");
		return;
	}

	kprintf("[%d][%s] frame=%x page=%x size=%x",
		idx,
		status[tlbe->status],
		k1b_tlbe_paddr_get(tlbe),
		k1b_tlbe_vaddr_get(tlbe),
		k1b_tlbe_pgsize_get(tlbe)
	);
}

/*============================================================================*
 * k1b_tlb_flush()                                                            *
 *============================================================================*/

/**
 * The k1b_tlb_flush() function writes all JTLB entries cached in
 * memory into the architectural TLB of the underlying core.
 *
 * @todo Implement this function.
 */
PUBLIC int k1b_tlb_flush(void)
{
	return (0);
}

/*============================================================================*
 * k1b_tlb_lookup_vaddr()                                                     *
 *============================================================================*/

/**
 * The k1b_tlb_lookup_vaddr() function searches the architectural TLB
 * for an entry that matches the virtual address @p vaddr.
 *
 * @todo Search in the JTLB may be performed using a faster method
 * based on tag lookup.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC const struct tlbe *k1b_tlb_lookup_vaddr(vaddr_t vaddr)
{
	int coreid;
	const struct tlbe *tlbe;

	coreid = k1b_core_get_id();

	/* Search in JTLB. */
	for (int i = 0; i < K1B_JTLB_LENGTH; i++)
	{
		tlbe = &tlb[coreid].jtlb[i];

		/* Found */
		if (k1b_tlbe_vaddr_get(tlbe) == vaddr)
			return (tlbe);
	}

	/* Search in LTLB. */
	for (int i = 0; i < K1B_LTLB_LENGTH; i++)
	{
		tlbe = &tlb[coreid].ltlb[i];

		/* Found */
		if (k1b_tlbe_vaddr_get(tlbe) == vaddr)
			return (tlbe);
	}

	return (NULL);
}

/*============================================================================*
 * k1b_tlb_lookup_paddr()                                                     *
 *============================================================================*/

/**
 * The k1b_tlb_lookup_paddr() function searches the architectural TLB
 * for an entry that matches the physical address @p paddr.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC const struct tlbe *k1b_tlb_lookup_paddr(paddr_t paddr)
{
	int coreid;
	const struct tlbe *tlbe;

	coreid = k1b_core_get_id();

	/* Search in JTLB. */
	for (int i = 0; i < K1B_JTLB_LENGTH; i++)
	{
		tlbe = &tlb[coreid].jtlb[i];

		/* Found */
		if (k1b_tlbe_paddr_get(tlbe) == paddr)
			return (tlbe);
	}

	/* Search in LTLB. */
	for (int i = 0; i < K1B_LTLB_LENGTH; i++)
	{
		tlbe = &tlb[coreid].ltlb[i];

		/* Found */
		if (k1b_tlbe_paddr_get(tlbe) == paddr)
			return (tlbe);
	}

	return (NULL);
}

/*============================================================================*
 * k1b_tlb_write()                                                            *
 *============================================================================*/

/**
 * THe k1b_tlb_write() function writes an entry into the architectural
 * TLB. If the new entry conflicts to an old one, the old one is
 * overwritten.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int k1b_tlb_write(
		vaddr_t vaddr,
		paddr_t paddr,
		unsigned shift,
		unsigned way,
		unsigned protection
)
{
	int coreid;
	unsigned idx;
	struct tlbe tlbe;
	__k1_tlb_entry_t _tlbe;

	tlbe.addr_ext = 0;
	tlbe.addrspace = 0;
	tlbe.cache_policy = K1B_DTLBE_CACHE_POLICY_WRTHROUGH;
	tlbe.frame = paddr >> 12;
	tlbe.global = 1;
	tlbe.page = (vaddr >> 12) | (1 << (shift - 12 - 1));
	tlbe.protection = protection;
	tlbe.size = (shift == 12) ? 1 : 0;
	tlbe.status = K1B_TLBE_STATUS_AMODIFIED;

	coreid = k1b_core_get_id();
	idx = 2*((vaddr >> shift) & 0x3f) + way;

	kmemcpy(&_tlbe, &tlbe, K1B_TLBE_SIZE);
	kmemcpy(&tlb[coreid].jtlb[idx], &tlbe, K1B_TLBE_SIZE);

	/* Write to hardware TLB. */
	if (mOS_mem_write_jtlb(_tlbe, way) != 0)
	{
		kprintf("[hal] failed to invalidate tlb %x", vaddr);
		return (-EAGAIN);
	}

	return (0);
}

/**
 * The k1b_tlb_inval() function invalidates the TLB entry that
 * encodes the virtual address @p vaddr.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int k1b_tlb_inval(vaddr_t vaddr, unsigned shift, unsigned way)
{
	int coreid;
	unsigned idx;
	struct tlbe tlbe;
	__k1_tlb_entry_t _tlbe;

	tlbe.addr_ext = 0;
	tlbe.addrspace = 0;
	tlbe.cache_policy = 0;
	tlbe.frame = 0;
	tlbe.global = 0;
	tlbe.page = (vaddr >> 12) | (1 << (shift - 12 - 1));
	tlbe.protection = 0;
	tlbe.size = (shift == 12) ? 1 : 0;
	tlbe.status = K1B_TLBE_STATUS_INVALID;

	coreid = k1b_core_get_id();
	idx = 2*((vaddr >> shift) & 0x3f) + way;

	kmemcpy(&_tlbe, &tlbe, K1B_TLBE_SIZE);
	kmemcpy(&tlb[coreid].jtlb[idx], &tlbe, K1B_TLBE_SIZE);

	/* Write to hardware TLB. */
	if (mOS_mem_write_jtlb(_tlbe, way) != 0)
	{
		kprintf("[hal] failed to invalidate tlb %x", vaddr);
		return (-EAGAIN);
	}

	return (0);
}

/*============================================================================*
 * tlb_init()                                                                 *
 *============================================================================*/

/**
 * The k1b_tlb_init() initializes the architectural TLB of the
 * underlying k1b core.
 */
PUBLIC void k1b_tlb_init(void)
{
	int coreid;

	coreid = k1b_core_get_id();

	kprintf("[core %d][hal] initializing tlb", coreid);

	/* Read JTLB into memory. */
	for (int i = 0; i < K1B_JTLB_LENGTH; i++)
		k1b_tlbe_read(&tlb[coreid].jtlb[i], K1B_JTLB_OFFSET + i);

	/* Read LTLB into memory. */
	for (int i = 0; i < K1B_LTLB_LENGTH; i++)
		k1b_tlbe_read(&tlb[coreid].ltlb[i], K1B_LTLB_OFFSET + i);
}

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

#include <arch/or1k/core.h>
#include <arch/or1k/tlb.h>
#include <nanvix/const.h>

/*============================================================================*
 * k1b_tlb_flush()                                                            *
 *============================================================================*/

/**
 * @brief Flushes changes in the TLB.
 *
 * The or1k_tlb_flush() function flushes the changes made to the
 * TLB of the underlying or1k core.
 *
 * @returns This function always returns zero.
 */
PUBLIC int or1k_tlb_flush(void)
{
	unsigned nsets;
	unsigned dtlbmr_base;
	unsigned itlbmr_base;

	nsets = (1 << ((or1k_mfspr(OR1K_SPR_DMMUCFGR) & OR1K_SPR_DMMUCFGR_NTS)
			>> OR1K_SPR_DMMUCFGR_NTS_OFF));
		
	dtlbmr_base = OR1K_SPR_DTLBMR_BASE(0);
	itlbmr_base = OR1K_SPR_ITLBMR_BASE(0);

	for (unsigned i = 0; i < nsets; i++)
	{
		or1k_mtspr(dtlbmr_base, 0);
		or1k_mtspr(itlbmr_base, 0);
		dtlbmr_base++;
		itlbmr_base++;
	}

	return (0);
}

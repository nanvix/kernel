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

#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <arch/core/i386/gdt.h>
#include <arch/core/i386/tss.h>

/**
 * @brief Global Descriptor Table (GDT).
 */
PRIVATE struct gdte gdt[GDT_SIZE];

/**
 * @brief Pointer to Global Descriptor Table (GDTPTR)
 */
PRIVATE struct gdtptr gdtptr;

/*
 * @brief Sets an entry of the GDT.
 *
 * @param n           Target entry.
 * @param base        Base address of segment.
 * @param limit       Limit (size) of the segment.
 * @param granularity Granularity of segment.
 * @param access      Access permissions.
 */
PRIVATE void set_gdte
(int n, unsigned base, unsigned limit, unsigned granularity, unsigned access)
{
	/* Set segment base address. */
	gdt[n].base_low = (base & 0xffffff);
	gdt[n].base_high = (base >> 24) & 0xff;
	
	/* Set segment limit. */ 
	gdt[n].limit_low = (limit & 0xffff);
	gdt[n].limit_high = (limit >> 16) & 0xf;
	
	/* Set granularity and access. */
	gdt[n].granularity = granularity;
	gdt[n].access = access;
}

/*
 * @brief Sets up GDT.
 */
PUBLIC void gdt_setup(void)
{
	/* Size-error checking. */
	KASSERT_SIZE(sizeof(struct gdte), GDTE_SIZE);
	KASSERT_SIZE(sizeof(struct gdtptr), GDTPTR_SIZE);

	/* Blank GDT and GDT pointer. */
	kmemset(gdt, 0, sizeof(gdt));
	kmemset(&gdtptr, 0, GDTPTR_SIZE);
	
	/* Set GDT entries. */
	set_gdte(GDT_NULL, 0, 0x00000, 0x0, 0x00);
	set_gdte(GDT_CODE_DPL0, 0, 0xfffff, 0xc, 0x9a);
	set_gdte(GDT_DATA_DPL0, 0, 0xfffff, 0xc, 0x92);
	set_gdte(GDT_CODE_DPL3, 0, 0xfffff, 0xc, 0xfa);
	set_gdte(GDT_DATA_DPL3, 0, 0xfffff, 0xc, 0xf2);
	set_gdte(GDT_TSS, (unsigned) &tss, (unsigned)&tss + TSS_SIZE, 0x0, 0xe9);
	
	/* Set GDT pointer. */
	gdtptr.size = sizeof(gdt) - 1;
	gdtptr.ptr = (unsigned) gdt;
	
	/* Flush GDT. */
	gdt_flush(&gdtptr);
}

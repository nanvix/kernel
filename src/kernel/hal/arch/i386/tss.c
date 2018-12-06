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
#include <arch/i386/gdt.h>
#include <arch/i386/tss.h>

/**
 * @brief Task state segment.
 */
PUBLIC struct tss tss;

/**
 * @brief Setups the TSS.
 */
PUBLIC void tss_setup(void)
{
	/* Size-error checking. */
	KASSERT_SIZE(sizeof(struct tss), TSS_SIZE);
	
	/* Blank TSS. */
	kmemset(&tss, 0, TSS_SIZE);
	
	/* Fill up TSS. */
	tss.ss0 = KERNEL_DS;
	tss.iomap = (TSS_SIZE - 1) << 16;
	
	/* Flush TSS. */
	tss_flush();
}

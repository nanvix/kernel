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

#ifndef ARCH_I386_GDT_H_
#define ARCH_I386_GDT_H_

/**
 * @addtogroup i386-gdt GDT
 * @ingroup i386
 *
 * @brief Global Descriptor Table
 */
/**@{*/

	/**
	 * @brief Size of a GDT entry (in bytes).
	 */
	#define GDTE_SIZE 8
	
	/**
	 * @brief Size of GDTPTR (in bytes).
	 */
	#define GDTPTR_SIZE 6
	
	/**
	 * @brief Number of entries in the GDT.
	 */
	#define GDT_SIZE 6

	/**
	 * @name GDT Entries
	 */
	/**@{*/
	#define GDT_NULL       0 /**< Null.       */
	#define GDT_CODE_DPL0  1 /**< Code DPL 0. */
	#define GDT_DATA_DPL0  2 /**< Data DPL 0. */
	#define GDT_CODE_DPL3  3 /**< Code DPL 3. */
	#define GDT_DATA_DPL3  4 /**< Data DPL 3. */
	#define GDT_TSS        5 /**< TSS.        */
	/**@}*/
	
	/**
	 * @name GDT Segment Selectors
	 */
	/**@{*/
	#define KERNEL_CS (GDTE_SIZE*GDT_CODE_DPL0)     /**< Kernel code. */
	#define KERNEL_DS (GDTE_SIZE*GDT_DATA_DPL0)     /**< Kernel data. */
	#define USER_CS   (GDTE_SIZE*GDT_CODE_DPL3 + 3) /**< User code.   */
	#define USER_DS   (GDTE_SIZE*GDT_DATA_DPL3 + 3) /**< User data.   */
	#define TSS       (GDTE_SIZE*GDT_TSS + 3)       /**< TSS.         */
	/**@}*/

#ifndef _ASM_FILE_

	#include <nanvix/const.h>

	/**
	 * @brief Global descriptor table entry.
	 */
	struct gdte
	{
		unsigned limit_low   : 16; /**< Limit low.   */
		unsigned base_low    : 24; /**< Base low.    */
		unsigned access      :  8; /**< Access.      */
		unsigned limit_high  :  4; /**< Limit high.  */
		unsigned granularity :  4; /**< Granularity. */
		unsigned base_high   :  8; /**< Base high.   */
	} __attribute__((packed));
	
	/**
	 * @brief Global descriptor table pointer.
	 */
	struct gdtptr
	{
		unsigned size : 16; /**< GDT size.            */
		unsigned ptr  : 32; /**< GDT virtual address. */
	} __attribute__((packed));

	/**
	 * Initializes the Global Descriptor Table (GDT).
	 */
	EXTERN void gdt_setup(void);

	/**
	 * Flushes the Global Descriptor Table (GDT).
	 *
	 * @param gdtptr Global descriptor table pointer.
	 */
	static inline void gdt_flush(const struct gdtptr *gdtptr)
	{
		__asm__ __volatile__ (
			"mov %0, %%eax;\
			lgdt (%%eax);\
			ljmp %1, $reload_cs;\
			reload_cs:\
			movw %1, %%ax;\
			movw %%ax, %%ds;\
			movw %%ax, %%es;\
			movw %%ax, %%fs;\
			movw %%ax, %%gs;"
			:
			: "r" (gdtptr), "N" (KERNEL_CS)
			: "memory", "eax"
		);
	}

#endif /* _ASM_FILE_ */

/**@}*/

#endif /* ARCH_I386_GDT_H_ */

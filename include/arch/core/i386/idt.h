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

#ifndef ARCH_I386_IDT_H_
#define ARCH_I386_IDT_H_

/**
 * @addtogroup i386-core-idt IDT
 * @ingroup i386-core
 *
 * @brief Interrupt Descritor Table
 */
/**@{*/

	/**
	 * @brief Size of and IDT entry (in bytes).
	 */
	#define IDTE_SIZE 8
	
	/**
	 * @brief Size of IDTPTR (in bytes).
	 */
	#define IDTPTR_SIZE 6
	
	/**
	 * @brief Number of entries in the IDT.
	 */
	#define IDT_SIZE 256

	/**
	 * @name Types of Interrupt Entries
	 */
	/**@{*/
	#define IDT_TASK32 0x5 /**< 32-bit task gate.      */
	#define IDT_INT16  0x6 /**< 16-bit interrupt gate. */
	#define IDT_TRAP16 0x7 /**< 16-bit trap gate.      */
	#define IDT_INT32  0xe /**< 32-bit interrupt gate. */
	#define IDT_TRAP32 0xf /**< 32-bit trap gate.      */
	/**@}*/

	#include <nanvix/const.h>

	/**
	 * @brief Interrupt descriptor table entry.
	 */
	struct idte
	{
		unsigned handler_low  : 16; /**< Handler low.           */
		unsigned selector     : 16; /**< GDT selector.          */
		unsigned              :  8; /**< Always zero.           */
		unsigned type         :  4; /**< Gate type (sse above). */
		unsigned flags        :  4; /**< Flags.                 */
		unsigned handler_high : 16; /**< handler high.          */
	} __attribute__((packed));
	
	/**
	 * @brief Interrupt descriptor table pointer.
	 */
	struct idtptr
	{
		unsigned size : 16; /**< IDT size.            */
		unsigned ptr  : 32; /**< IDT virtual address. */
	} __attribute__((packed));

	/**
	 * Initializes the Interrupt Descriptor Table (IDT).
	 */
	EXTERN void idt_setup(void);

	/**
	 * @brief Flushes the IDT.
	 *
	 * Flushes the Interrupt Descriptor Table (IDT).
	 *
	 * @param idtptr Interrupt descriptor table pointer.
	 */
	static inline void idt_flush(const struct idtptr *idtptr)
	{
		__asm__ __volatile__(
			"mov %0, %%eax;\
			lidt (%%eax);"
			:
			: "r" (idtptr)
			: "memory", "eax"
		);
	}

/**@}*/

#endif /* ARCH_I386_IDT_H_ */

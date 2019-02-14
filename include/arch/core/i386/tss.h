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

#ifndef ARCH_I386_TSS_H_
#define ARCH_I386_TSS_H_

/**
 * @addtogroup i386-core-tss TSS
 * @ingroup i386-core
 *
 * @brief Task State Segment
 */
/**@{*/

	/**
	 * @brief Size of a TSS entry (in bytes).
	 */
	#define TSS_SIZE 104

	/**
	 * @name Offsets to the TSS Structure
	 */
	/**@{*/
	#define TSS_ESP0     4 /* Ring 0 stack pointer. */
	#define TSS_SS0      8 /* Ring 0 stack segment. */
	#define TSS_ESP1    12 /* Ring 1 stack pointer. */
	#define TSS_SS1     16 /* Ring 1 stack segment. */
	#define TSS_ESP2    20 /* Ring 2 stack pointer. */
	#define TSS_SS2     24 /* Ring 2 stack segment. */
	#define TSS_CR3     28 /* cr3.                  */
	#define TSS_EIP     32 /* eip.                  */
	#define TSS_EFLAGS  36 /* eflags.               */
	#define TSS_EAX     40 /* eax.                  */
	#define TSS_ECX     44 /* ecx.                  */
	#define TSS_EDX     48 /* edx.                  */
	#define TSS_EBX     52 /* ebx.                  */
	#define TSS_ESP     56 /* esp.                  */
	#define TSS_EBP     60 /* ebp.                  */
	#define TSS_ESI     64 /* esi.                  */
	#define TSS_EDI     68 /* edi.                  */
	#define TSS_ES      72 /* es.                   */
	#define TSS_CS      76 /* cs.                   */
	#define TSS_SS      80 /* ss.                   */
	#define TSS_DS      84 /* ds.                   */
	#define TSS_FS      88 /* fs.                   */
	#define TSS_GS      92 /* gs.                   */
	#define TSS_LDTR    96 /* LDT selector.         */
	#define TSS_IOMAP  100 /* IO map.               */
	/**@}*/

	#include <nanvix/const.h>
	#include <arch/core/i386/gdt.h>

	/**
	 * @brief Task state segment.
	 */
	struct tss
	{
		unsigned link;   /**< Previous TSS in the list. */
		unsigned esp0;   /**< Ring 0 stack pointer.     */
		unsigned ss0;    /**< Ring 0 stack segment.     */
		unsigned esp1;   /**< Ring 1 stack pointer.     */
		unsigned ss1;    /**< Ring 1 stack segment.     */
		unsigned esp2;   /**< Ring 2 stack pointer.     */
		unsigned ss2;    /**< Ring 2 stack segment.     */
		unsigned cr3;    /**< cr3.                      */
		unsigned eip;    /**< eip.                      */
		unsigned eflags; /**< eflags.                   */
		unsigned eax;    /**< eax.                      */
		unsigned ecx;    /**< ecx.                      */
		unsigned edx;    /**< edx.                      */
		unsigned ebx;    /**< ebx.                      */
		unsigned esp;    /**< esp.                      */
		unsigned ebp;    /**< ebp.                      */
		unsigned esi;    /**< esi.                      */
		unsigned edi;    /**< edi.                      */
		unsigned es;     /**< es.                       */
		unsigned cs;     /**< cs.                       */
		unsigned ss;     /**< ss.                       */
		unsigned ds;     /**< ds.                       */
		unsigned fs;     /**< fs.                       */
		unsigned gs;     /**< gs.                       */
		unsigned ldtr;   /**< LDT selector.             */
		unsigned iomap;  /**< IO map.                   */
	} __attribute__((packed));

	/* Forward declarations. */
	EXTERN struct tss tss;

	/**
	 * Initializes the Task State Segment (TSS).
	 */
	EXTERN void tss_setup(void);

	/**
	 * @brief Flushes the TSS.
	 *
	 * Flushes the Task State Segment (TSS).
	 */
	static inline void tss_flush(void)
	{
		__asm__ __volatile__(
			"movl %0, %%eax;\
			ltr %%ax;"
			:
			: "Z" (TSS)
			: "eax"
		);
	}

/**@}*/

#endif /* ARCH_I386_TSS_H_ */

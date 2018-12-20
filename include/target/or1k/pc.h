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

#ifndef TARGET_OR1K_PC_H_
#define TARGET_OR1K_PC_H_

/**
 * @defgroup or1kpc OR1K PC
 * @ingroup targets
 */
/**@{*/

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_stdout_init
	#define __hal_stdout_write
	/**@}*/

	#include <arch/or1k/or1k.h>

/*============================================================================*
 * Interrupt and Exception Interfaces                                         *
 *============================================================================*/

	/**
	 * @name Hardware Interrupts for the OR1K PC Target
	 */
	/**@{*/
	#define OR1K_PC_INT_CLOCK    0   /*< Tmer.               */
	#define OR1K_PC_INT_OMPIC    1   /*< OMPIC.              */
	#define OR1K_PC_INT_COM1     2   /*< COM1.               */
	#define OR1K_PC_INT_EXTERNAL 256 /*< External interrupt. */
	/**@}*/

	/**
	 * @brief Number of hardware interrupts in the OR1K PC target.
	 */
	#define _HAL_INT_NR OR1K_NUM_HWINT

	/**
	 * @brief Number of exceptions in the OR1K PC target.
	 */
	#define _HAL_NUM_EXCEPTION OR1K_NUM_EXCEPTION

/*============================================================================*
 * Memory Interface                                                           *
 *============================================================================*/

	/**
	 * @brief Kernel stack size (in bytes).
	 */
	#define _KSTACK_SIZE OR1K_PAGE_SIZE

	/**
	 * @brief Memory size (in bytes).
	 */
	#define _HAL_MEM_SIZE 0x01000000

	/**
	 * @name Virtual Memory Layout
	 */
	/**@{*/
	#define _UBASE_VIRT  0x02000000 /**< User base.        */
	#define _USTACK_ADDR 0xc0000000 /**< User stack.       */
	#define _KBASE_VIRT  0xc0000000 /**< Kernel base.      */
	#define _KPOOL_VIRT  0xc1000000 /**< Kernel page pool. */
	/**@}*/

	/**
	 * @name Physical Memory Layout
	 */
	/**@{*/
	#define _KBASE_PHYS 0x00000000 /**< Kernel base.      */
	#define _KPOOL_PHYS 0x01000000 /**< Kernel page pool. */
	#define _UBASE_PHYS 0x02000000 /**< User base.        */
	/**@}*/

	/**
	 * @brief Memory size (in bytes).
	 *
	 * @cond or1kpc
	 */
	#define _MEMORY_SIZE (32*1024*1024)
	/**@endcond*/

	/**
	 * @brief Kernel memory size (in bytes).
	 */
	#define _KMEM_SIZE (16*1024*1024)

	/**
	 * @brief Kernel page pool size (in bytes).
	 *
	 * @cond or1kpc
	 */
	#define _KPOOL_SIZE (4*1024*1024)
	/*@endcond*/

	/**
	 * @brief User memory size (in bytes).
	 *
	 * @cond or1kpc
	 */
	#define _UMEM_SIZE (_MEMORY_SIZE - _KMEM_SIZE - _KPOOL_SIZE)
	/**@endcond*/

/*============================================================================*
 * Clock Interface                                                            *
 *============================================================================*/

	/**
	 * @name Hardware Interrupts
	 */
	/**@{*/
	#define HAL_INT_CLOCK OR1K_PC_INT_CLOCK /*< Programmable interrupt timer. */
	/**@}*/

/*============================================================================*
 * Processor Interface                                                        *
 *============================================================================*/

	/**
	 * @brief Number of cores in a a CPU in the the OR1K PC target.
	 */
	#define _HAL_NUM_CORES OR1K_NUM_CORES

/*============================================================================*
 * Debug Interface                                                            *
 *============================================================================*/

	/**
	 * @see console_init()
	 */
	static inline void hal_stdout_init(void)
	{
	}

	/**
	 * @see console_write()
	 */
	static inline void hal_stdout_write(const char *buf, size_t n)
	{
		((void)buf);
		((void)n);
	}

/**@}*/

#endif /* TARGET_OR1K_PC_H_ */

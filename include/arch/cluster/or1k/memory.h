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

#ifndef CLUSTER_OR1K_MEMORY_H_
#define CLUSTER_OR1K_MEMORY_H_

	/* Cluster Interface Implementation */
	#include <arch/cluster/or1k/_or1k.h>

/**
 * @addtogroup cluster-or1k-io or1k Cluster Memory
 * @ingroup cluster-or1k
 */
/**@{*/

	/**
	 * @brief Memory size (in bytes).
	 */
	#define OR1K_MEM_SIZE (32*1024*1024)

	/**
	 * @brief Kernel memory size (in bytes).
	 */
	#define OR1K_KMEM_SIZE (16*1024*1024)

	/**
	 * @brief Kernel page pool size (in bytes).
	 */
	#define OR1K_KPOOL_SIZE (4*1024*1024)

	/**
	 * @name Virtual Memory Layout
	 */
	/**@{*/
	#define OR1K_UBASE_VIRT  0x02000000 /**< User base.        */
	#define OR1K_USTACK_ADDR 0xc0000000 /**< User stack.       */
	#define OR1K_KBASE_VIRT  0xc0000000 /**< Kernel base.      */
	#define OR1K_KPOOL_VIRT  0xc1000000 /**< Kernel page pool. */
	#define OR1K_UART_VIRT   0xc4000000 /**< Kernel UART page. */
	#define OR1K_OMPIC_VIRT  0xc5000000 /**< Kernel OMPIC page.*/
	/**@}*/

	/**
	 * @name Physical Memory Layout
	 */
	/**@{*/
	#define OR1K_KBASE_PHYS 0x00000000 /**< Kernel base.            */
	#define OR1K_KPOOL_PHYS 0x01000000 /**< Kernel page pool.       */
	#define OR1K_UBASE_PHYS 0x02000000 /**< User base.              */
	#define OR1K_UART_PHYS  0x90000000 /**< UART Physical address.  */
	#define OR1K_OMPIC_PHYS 0x98000000 /**< OMPIC Physical address. */
	/**@}*/

	/**
	 * OMPIC Registers and flags.
	 */
	/**@{*/
	#define OR1K_OMPIC_CPUBYTES	        8
	#define OR1K_OMPIC_CTRL(cpu)        (OR1K_OMPIC_VIRT + (0x0 + ((cpu) * OR1K_OMPIC_CPUBYTES)))
	#define OR1K_OMPIC_STAT(cpu)        (OR1K_OMPIC_VIRT + (0x4 + ((cpu) * OR1K_OMPIC_CPUBYTES)))
	#define OR1K_OMPIC_CTRL_IRQ_ACK	    (1 << 31)
	#define OR1K_OMPIC_CTRL_IRQ_GEN	    (1 << 30)
	#define OR1K_OMPIC_CTRL_DST(cpu)    (((cpu) & 0x3fff) << 16)
	#define OR1K_OMPIC_STAT_IRQ_PENDING (1 << 30)
	#define OR1K_OMPIC_DATA(x)          ((x) & 0xffff)
	#define OR1K_OMPIC_STAT_SRC(x)      (((x) >> 16) & 0x3fff)
	/**@}*/

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond or1k
 */

	/**
	 * @brief Memory size (in bytes).
	 */
	#define _MEMORY_SIZE OR1K_MEM_SIZE

	/**
	 * @brief Kernel stack size (in bytes).
	 */
	#define _KSTACK_SIZE OR1K_PAGE_SIZE

	/**
	 * @brief Kernel memory size (in bytes).
	 */
	#define _KMEM_SIZE OR1K_KMEM_SIZE

	/**
	 * @brief Kernel page pool size (in bytes).
	 */
	#define _KPOOL_SIZE OR1K_KPOOL_SIZE

	/**
	 * @brief User memory size (in bytes).
	 */
	#define _UMEM_SIZE (OR1K_MEM_SIZE - OR1K_KMEM_SIZE - OR1K_KPOOL_SIZE)

	/**
	 * @name Virtual Memory Layout
	 */
	/**@{*/
	#define _UBASE_VIRT  OR1K_UBASE_VIRT  /**< User Base        */
	#define _USTACK_ADDR OR1K_USTACK_ADDR /**< User Stack       */
	#define _KBASE_VIRT  OR1K_KBASE_VIRT  /**< Kernel Base      */
	#define _KPOOL_VIRT  OR1K_KPOOL_VIRT  /**< Kernel Page Pool */
	#define _UART_ADDR   OR1K_UART_VIRT   /**< UART Device      */
	/**@}*/

	/**
	 * @name Physical Memory Layout
	 */
	/**@{*/
	#define _KBASE_PHYS OR1K_KERNEL_BASE_PHYS /**< Kernel Base      */
	#define _KPOOL_PHYS OR1K_KPOOL_PHYS       /**< Kernel Page Pool */
	#define _UBASE_PHYS OR1K_UBASE_PHYS       /**< User Base        */
	/**@}*/

/**@endcond*/

#endif /* CLUSTER_OR1K_MEMORY_H_ */

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

#ifndef CLUSTER_K1B_MEMORY_H_
#define CLUSTER_K1B_MEMORY_H_

	/* Cluster Interface Implementation */
	#include <arch/cluster/k1b/_k1b.h>

/**
 * @addtogroup k1b-cluster-mem k1b Cluster Memory
 * @ingroup k1b-cluster
 */
/**@{*/

	/**
	 * @brief Memory size (in bytes) for Compute Cluster.
	 */
	#define K1B_NODE_MEM_SIZE (2*1024*1024)

	/**
	 * @brief Memory size (in bytes) for IO DDR Cluster.
	 */
	#define K1B_IODDR_MEM_SIZE (4*1024*1024)

	/**
	 * @brief Memory size (in bytes) for IO Ethernet Cluster.
	 */
	#define K1B_IOETH_MEM_SIZE (4*1024*1024)

	/**
	 * @brief Kernel stack size (in bytes).
	 */
	#if defined(__ioddr__) || defined(__ioeth__)
		#define K1B_KSTACK_SIZE (0x1800)
	#elif defined(__node__)
		#define K1B_KSTACK_SIZE (0x800)
	#endif

	/**
	 * @brief Kernel pool size (in bytes).
	 */
	#if defined(__ioddr__) || defined(__ioeth__)
		#define K1B_KPOOL_SIZE (0x10000)
	#elif defined(__node__)
		#define K1B_KPOOL_SIZE (0x10000)
	#endif

	/**
	 * @brief User memory size (in bytes).
	 */
	#if defined(__ioddr__) || defined(__ioeth__)
		#define K1B_UMEM_SIZE (512*1024)
	#elif defined(__node__)
		#define K1B_UMEM_SIZE (512*1024)
	#endif

	/**
	 * @name Physical Memory Layout
	 */
	/**@{*/
	#if defined(__ioddr__) || defined(__ioeth__)
		#define K1B_HYPER_LOW_BASE_PHYS  0x00000000 /**< Low Hypervisor Base  */
		#define K1B_HYPER_LOW_END_PHYS   0x00010000 /**< Low Hypervisor End   */
		#define K1B_HYPER_HIGH_BASE_PHYS 0x001f0000 /**< High Hypervisor Base */
		#define K1B_HYPER_HIGH_END_PHYS  0x00200000 /**< High Hypervisor End  */
	#elif defined(__node__)
		#define K1B_HYPER_LOW_BASE_PHYS  0x00000000 /**< Low Hypervisor Base  */
		#define K1B_HYPER_LOW_END_PHYS   0x00008000 /**< Low Hypervisor End   */
		#define K1B_HYPER_HIGH_BASE_PHYS 0x001f8000 /**< High Hypervisor Base */
		#define K1B_HYPER_HIGH_END_PHYS  0x00200000 /**< High Hypervisor End  */
	#endif
#ifndef _ASM_FILE_
	extern const paddr_t K1B_KERNEL_BASE_PHYS;      /**< Kernel Base          */
	extern const paddr_t K1B_KERNEL_END_PHYS;       /**< Kernel End           */
	extern const paddr_t K1B_KSTACK_BASE_PHYS;      /**< Kernel Stack Base    */
	extern const paddr_t K1B_KPOOL_BASE_PHYS;       /**< Kernel Pool Base     */
	extern const paddr_t K1B_KPOOL_END_PHYS;        /**< Kernel Pool End      */
	extern const paddr_t K1B_USER_BASE_PHYS;        /**< User Base            */
	extern const paddr_t K1B_USER_END_PHYS;         /**< User End             */
#endif
	/**@}*/

	/**
	 * @name Virtual Memory Layout
	 */
	/**@{*/
	#if defined(__ioddr__) || defined(__ioeth__)
		#define K1B_HYPER_LOW_BASE_VIRT  0x00000000 /**< Low Hypervisor Base  */
		#define K1B_HYPER_LOW_END_VIRT   0x00010000 /**< Low Hypervisor End   */
		#define K1B_HYPER_HIGH_BASE_VIRT 0x001f0000 /**< High Hypervisor Base */
		#define K1B_HYPER_HIGH_END_VIRT  0x00200000 /**< High Hypervisor End  */
	#elif defined(__node__)
		#define K1B_HYPER_LOW_BASE_VIRT  0x00000000 /**< Low Hypervisor Base  */
		#define K1B_HYPER_LOW_END_VIRT   0x00008000 /**< Low Hypervisor End   */
		#define K1B_HYPER_HIGH_BASE_VIRT 0x001f8000 /**< High Hypervisor Base */
		#define K1B_HYPER_HIGH_END_VIRT  0x00200000 /**< High Hypervisor End  */
	#endif
#ifndef _ASM_FILE_
	extern const vaddr_t K1B_KERNEL_BASE_VIRT;      /**< Kernel Base          */
	extern const vaddr_t K1B_KERNEL_END_VIRT;       /**< Kernel End           */
	extern const vaddr_t K1B_KSTACK_BASE_VIRT;      /**< Kernel Stack Base    */
	extern const vaddr_t K1B_KPOOL_BASE_VIRT;       /**< Kernel Pool Base     */
	extern const vaddr_t K1B_KPOOL_END_VIRT;        /**< Kernel Pool End      */
	extern const vaddr_t K1B_USER_BASE_VIRT;        /**< User Base            */
	extern const vaddr_t K1B_USER_END_VIRT;         /**< User End             */
#endif
	/**@}*/
	
/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond k1b
 */

	/**
	 * @brief Memory size (in bytes).
	 */
	#if defined(__node__)
		#define _MEMORY_SIZE K1B_NODE_MEM_SIZE
	#elif defined (__ioddr__)
		#define _MEMORY_SIZE K1B_IODDR_MEM_SIZE
	#elif defined (__ioeth__)
		#define _MEMORY_SIZE K1B_IOETH_MEM_SIZE
	#endif

	/**
	 * @brief Kernel stack size (in bytes).
	 */
	#define _KSTACK_SIZE K1B_KSTACK_SIZE

	/**
	 * @brief Kernel memory size (in bytes).
	 */
	#define _KMEM_SIZE (K1B_KERNEL_END_PHYS - K1B_KERNEL_BASE_PHYS)

	/**
	 * @brief Kernel page pool size (in bytes).
	 */
	#define _KPOOL_SIZE K1B_KPOOL_SIZE

	/**
	 * @brief User memory size (in bytes).
	 */
	#define _UMEM_SIZE K1B_UMEM_SIZE

	/**
	 * @name Virtual Memory Layout
	 */
	/**@{*/
	#define _UBASE_VIRT  K1B_USER_BASE_VIRT       /**< User Base        */
	#define _USTACK_ADDR K1B_HYPER_HIGH_BASE_VIRT /**< User Stack       */
	#define _KBASE_VIRT  K1B_KERNEL_BASE_VIRT     /**< Kernel Base      */
	#define _KPOOL_VIRT  K1B_KPOOL_BASE_VIRT      /**< Kernel Page Pool */
	/**@}*/

	/**
	 * @name Physical Memory Layout
	 */
	/**@{*/
	#define _KBASE_PHYS K1B_KERNEL_BASE_PHYS /**< Kernel Base      */
	#define _KPOOL_PHYS K1B_KPOOL_BASE_PHYS  /**< Kernel Page Pool */
	#define _UBASE_PHYS K1B_USER_BASE_PHYS   /**< User Base        */
	/**@}*/

/**@endcond*/

#endif /* CLUSTER_K1B_MEMORY_H_ */

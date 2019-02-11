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

#ifndef TARGET_KALRAY_MPPA256_H_
#define TARGET_KALRAY_MPPA256_H_

/**
 * @defgroup mppa256 Kalray MPPA-256
 * @ingroup targets
 */
/**@{*/

#ifndef _ASM_FILE_

	#ifndef NANVIX_HAL_TARGET_H_
	#error "include <nanvix/hal/target.h> instead"
	#endif

	#include <arch/k1b/k1b.h>

	#include <driver/jtag.h>

#endif
	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_stdout_init
	#define __hal_stdout_write
	/**@}*/

/*============================================================================*
 * Interrupt and Exception Interfaces                                         *
 *============================================================================*/

	/**
	 * @name Hardware Interrupts for Kalray MPPA-256 Target
	 */
	/**@{*/
	#define MPPA256_INT_CLOCK0     0 /**< Clock 0              */
	#define MPPA256_INT_CLOCK1     1 /**< Clock 1              */
	#define MPPA256_INT_TIMER      2 /**< Watchdog Timer       */
	#define MPPA256_INT_CNOC       3 /**< Control NoC          */
	#define MPPA256_INT_DNOC       4 /**< Data NoC             */
	#define MPPA256_INT_DMA        5 /**< DMA                  */
	#define MPPA256_INT_NOC_ERR    6 /**< NoC Error            */
	#define MPPA256_INT_TIMER_ERR  7 /**< Watchdog Timer Error */
	#define MPPA256_INT_PE0        8 /**< Remote Core 0        */
	#define MPPA256_INT_PE1        9 /**< Remote Core 1        */
	#define MPPA256_INT_PE2       10 /**< Remote Core 2        */
	#define MPPA256_INT_PE3       11 /**< Remote Core 3        */
	#define MPPA256_INT_PE4       12 /**< Remote Core 4        */
	#define MPPA256_INT_PE5       13 /**< Remote Core 5        */
	#define MPPA256_INT_PE6       14 /**< Remote Core 6        */
	#define MPPA256_INT_PE7       15 /**< Remote Core 7        */
	#define MPPA256_INT_PE8       16 /**< Remote Core 8        */
	#define MPPA256_INT_PE9       17 /**< Remote Core 9        */
	#define MPPA256_INT_PE10      18 /**< Remote Core 10       */
	#define MPPA256_INT_PE11      19 /**< Remote Core 11       */
	#define MPPA256_INT_PE12      20 /**< Remote Core 12       */
	#define MPPA256_INT_PE13      21 /**< Remote Core 14       */
	#define MPPA256_INT_PE14      22 /**< Remote Core 14       */
	#define MPPA256_INT_PE15      23 /**< Remote Core 15       */
	/**@}*/

	/**
	 * @brief Number of hardware interrupts in the Kalray MPPA-256 target.
	 */
	#define _HAL_INT_NR K1B_NUM_HWINT

	/**
	 * @brief Number of exceptions in the Kalray MPPA-256 target.
	 */
	#define _HAL_NUM_EXCEPTIONS K1B_NUM_EXCEPTIONS


/*============================================================================*
 * Memory Interface                                                           *
 *============================================================================*/

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
	 * @brief Log2 of kernel stack size.
	 */
	#define K1B_KSTACK_SIZE_LOG2 (11)

	/**
	 * @brief Kernel stack size (in bytes).
	 */
	#define K1B_KSTACK_SIZE (1 << K1B_KSTACK_SIZE_LOG2)

	/**
	 * @name Physical Memory Layout
	 *
	 * @todo Check if this layout is valid for IO Clusters.
	 */
	/**@{*/
	#if defined(__ioddr__) || defined(__ioeth__)
		#define MPPA256_HYPER_LOW_BASE_PHYS  0x00000000 /**< Low Hypervisor Base  */
		#define MPPA256_KERNEL_BASE_PHYS     0x00010000 /**< Kernel Base          */
		#define MPPA256_KSTACK_BASE_PHYS     0x0001f000 /**< Kernel Stack Base    */
		#define MPPA256_KERNEL_END_PHYS      0x00070000 /**< Kernel End           */
		#define MPPA256_KPOOL_BASE_PHYS      0x00070000 /**< Kernel Pool Base     */
		#define MPPA256_KPOOL_END_PHYS       0x00080000 /**< Kernel Pool End      */
		#define MPPA256_USER_BASE_PHYS       0x00080000 /**< User Base            */
		#define MPPA256_USER_END_PHYS        0x001f0000 /**< User End             */
		#define MPPA256_HYPER_HIGH_BASE_PHYS 0x001f0000 /**< High Hypervisor Base */
	#elif defined(__node__)
		#define MPPA256_HYPER_LOW_BASE_PHYS  0x00000000 /**< Low Hypervisor Base  */
		#define MPPA256_KERNEL_BASE_PHYS     0x00008000 /**< Kernel Base          */
		#define MPPA256_KSTACK_BASE_PHYS     0x0001f000 /**< Kernel Stack Base    */
		#define MPPA256_KERNEL_END_PHYS      0x00058000 /**< Kernel End           */
		#define MPPA256_KPOOL_BASE_PHYS      0x00058000 /**< Kernel Pool Base     */
		#define MPPA256_KPOOL_END_PHYS       0x00078000 /**< Kernel Pool End      */
		#define MPPA256_USER_BASE_PHYS       0x00078000 /**< User Base            */
		#define MPPA256_USER_END_PHYS        0x001f8000 /**< User End.            */
		#define MPPA256_HYPER_HIGH_BASE_PHYS 0x001f8000 /**< High Hypervisor Base */
	#endif
	/**@}*/

	/**
	 * @name Virtual Memory Layout
	 *
	 * @todo Check if this layout is valid for IO Clusters.
	 */
	/**@{*/
	#if defined(__ioddr__) || defined(__ioeth__)
		#define MPPA256_HYPER_LOW_BASE_VIRT  0x00000000 /**< Low Hypervisor Base  */
		#define MPPA256_KERNEL_BASE_VIRT     0x00010000 /**< Kernel Base          */
		#define MPPA256_KSTACK_BASE_VIRT     0x0001f000 /**< Kernel Stack Base    */
		#define MPPA256_KERNEL_END_VIRT      0x00070000 /**< Kernel End           */
		#define MPPA256_KPOOL_BASE_VIRT      0x00070000 /**< Kernel Pool Base     */
		#define MPPA256_KPOOL_END_VIRT       0x00080000 /**< Kernel Pool End      */
		#define MPPA256_HYPER_HIGH_BASE_VIRT 0x001f0000 /**< High Hypervisor Base */
		#define MPPA256_USER_BASE_VIRT       0x80200000 /**< User Base            */
		#define MPPA256_USER_END_VIRT        0xc0000000 /**< User End             */
	#elif defined(__node__)
		#define MPPA256_HYPER_LOW_BASE_VIRT  0x00000000 /**< Low Hypervisor Base  */
		#define MPPA256_KERNEL_BASE_VIRT     0x00008000 /**< Kernel Base          */
		#define MPPA256_KSTACK_BASE_VIRT     0x0001f000 /**< Kernel Stack Base    */
		#define MPPA256_KERNEL_END_VIRT      0x00058000 /**< Kernel End           */
		#define MPPA256_KPOOL_BASE_VIRT      0x00058000 /**< Kernel Pool Base     */
		#define MPPA256_KPOOL_END_VIRT       0x00078000 /**< Kernel Pool End      */
		#define MPPA256_HYPER_HIGH_BASE_VIRT 0x001f8000 /**< High Hypervisor Base */
		#define MPPA256_USER_BASE_VIRT       0x80200000 /**< User Base            */
		#define MPPA256_USER_END_VIRT        0xc0000000 /**< User End             */
	#endif
	/**@}*/

	/**
	 * @cond mppa256
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
		 * @name Virtual Memory Layout
		 */
		/**@{*/
		#define _UBASE_VIRT  MPPA256_USER_BASE_VIRT       /**< User Base        */
		#define _USTACK_ADDR MPPA256_HYPER_HIGH_BASE_VIRT /**< User Stack       */
		#define _KBASE_VIRT  MPPA256_KERNEL_BASE_VIRT     /**< Kernel Base      */
		#define _KPOOL_VIRT  MPPA256_KPOOL_BASE_VIRT      /**< Kernel Page Pool */
		/**@}*/

		/**
		 * @name Physical Memory Layout
		 */
		/**@{*/
		#define _KBASE_PHYS MPPA256_KERNEL_BASE_PHYS /**< Kernel Base      */
		#define _KPOOL_PHYS MPPA256_KPOOL_BASE_PHYS  /**< Kernel Page Pool */
		#define _UBASE_PHYS MPPA256_USER_BASE_PHYS   /**< User Base        */
		/**@}*/

		/**
		 * @brief Kernel stack size (in bytes).
		 */
		#define _KSTACK_SIZE K1B_PAGE_SIZE

		/**
		 * @brief Kernel memory size (in bytes).
		 */
		#define _KMEM_SIZE (MPPA256_KERNEL_END_PHYS - MPPA256_KERNEL_BASE_PHYS)

		/**
		 * @brief Kernel page pool size (in bytes).
		 */
		#define _KPOOL_SIZE (MPPA256_KPOOL_END_PHYS - MPPA256_KPOOL_BASE_PHYS)

		/**
		 * @brief User memory size (in bytes).
		 */
		#define _UMEM_SIZE (MPPA256_USER_END_PHYS - MPPA256_USER_BASE_PHYS)

	/**@endcond*/

/*============================================================================*
 * Clock Interface                                                            *
 *============================================================================*/

	/**
	 * @name Hardware Interrupts
	 */
	/**@{*/
	#define HAL_INT_CLOCK MPPA256_INT_CLOCK0 /*< Programmable interrupt timer. */
	/**@}*/

/*============================================================================*
 * Processor Interface                                                        *
 *============================================================================*/

	/**
	 * @brief Number of cores in a cluster in the Kalray MPPA-256 target.
	 */
	#define _HAL_NUM_CORES K1B_NUM_CORES

/*============================================================================*
 * Debug Interface                                                            *
 *============================================================================*/

#ifndef _ASM_FILE_

	/**
	 * @see jtag_init()
	 *
	 * @cond mppa256
	 */
	static inline void hal_stdout_init(void)
	{
		jtag_init();
	}
	/**@endcond*/

	/**
	 * @see jtag_write()
	 *
	 * @cond mppa256
	 */
	static inline void hal_stdout_write(const char *buf, size_t n)
	{
		jtag_write(buf, n);
	}
	/**@endcond*/

#endif

/**@}*/

#endif /* TARGET_KALRAY_MPPA256_H_ */

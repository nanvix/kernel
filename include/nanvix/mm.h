/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

/**
 * @defgroup kernel-mm MM
 * @ingroup kernel
 *
 * @brief Memory Management
 */

#ifndef NANVIX_MM_H_
#define NANVIX_MM_H_

	#include <nanvix/hal/hal.h>

/**
 * @addtogroup kernel-mm
 */
/**@{*/

	/**
	 * @name Virtual Memory Layout
	 */
	/**@{*/
	#define UBASE_VIRT  _UBASE_VIRT  /**< User Base         */
	#define USTACK_ADDR _USTACK_ADDR /**< User Stack        */
	#define KBASE_VIRT  _KBASE_VIRT  /**< Kernel Base       */
	#define KPOOL_VIRT  _KPOOL_VIRT  /**< Kernel Page Pool  */
	#define INITRD_VIRT _INITRD_VIRT /**< Initial RAM disk. */
	/**@}*/

	/**
	 * @name Physical Memory Layout
	 */
	/**@{*/
	#define KBASE_PHYS _KBASE_PHYS /**< Kernel base.      */
	#define KPOOL_PHYS _KPOOL_PHYS /**< Kernel page pool. */
	#define UBASE_PHYS _UBASE_PHYS /**< User base.        */
	/**@}*/

	/**
	 * @brief Physical memory size (in bytes).
	 */
	#define MEMORY_SIZE _MEMORY_SIZE

	/**
	 * @brief Kernel memory size (in bytes).
	 */
	#define KMEM_SIZE _KMEM_SIZE

	/**
	 * @brief Kernel page pool size (in bytes).
	 */
	#define KPOOL_SIZE _KPOOL_SIZE

	/**
	 * @brief User memory size (in bytes).
	 */
	#define UMEM_SIZE _UMEM_SIZE

	/**
	 * Casts something to a virtual address.
	 *
	 * @param x Something.
	 */
	#define VADDR(x) ((vaddr_t)(x))

	/**
	 * @brief Asserts a user virtual address.
	 *
	 * The mm_is_uaddr() function asserts whether or not the virtual
	 * address @p vaddr lies in user space.
	 *
	 * @returns If @p vaddr lies in user space, non zero is returned.
	 * Otherwise, zero is returned instead.
	 */
	static inline int mm_is_uaddr(vaddr_t vaddr)
	{
		return ((vaddr >= UBASE_VIRT) && (vaddr < KBASE_VIRT));
	}

	/**
	 * @brief Asserts a kernel virtual address.
	 *
	 * The mm_is_kaddr() function asserts whether or not the virtual
	 * address @p vaddr lies in kernel space.
	 *
	 * @returns If @p vaddr lies in kernel space, non zerp is
	 * returned. Otherwise, zero is returned instead.
	 */
	static inline int mm_is_kaddr(vaddr_t vaddr)
	{
		return (vaddr >= KBASE_VIRT);
	}

/**@}*/

#endif /** NANVIX_MM_H_ */

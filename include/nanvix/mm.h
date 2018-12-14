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

/*============================================================================*
 * Page Frame Allocator                                                       *
 *============================================================================*/

/**
 * @addtogroup kernel-mm-frame Frame Allocator
 * @ingroup kernel-mm
 *
 * @brief Page Frame Allocator
 */
/**@{*/

	/**
	 * @brief Number of page frames for user use.
	 */
	#define NUM_UFRAMES (UMEM_SIZE/PAGE_SIZE)

	/**
	 * @param Null frame.
	 */
	#define FRAME_NULL ((frame_t) -1)

	/**
	 * @brief Asserts if a frame ID is valid.
	 *
	 * The frame_is_valid_id() function asserts whether or not the
	 * frame @p ID is valid.
	 *
	 * @returns If @p id is valid, non zero is returned. Otherwise,
	 * zero is returned instead.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline int frame_is_valid_id(int id)
	{
		return ((id >= 0) && (id < NUM_UFRAMES));
	}

	/**
	 * @brief Converts an ID of a user page frame to a page frame number.
	 *
	 * @param id ID of target user page frame.
	 *
	 * @returns Frame number of target user page frame.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline frame_t frame_id_to_num(int id)
	{
		/* Invalid ID. */
		if (!frame_is_valid_id(id))
			return (FRAME_NULL);

		return ((UBASE_PHYS >> PAGE_SHIFT) + id);
	}

	/**
	 * @brief Asserts if a frame number is valid.
	 *
	 * The frame_is_valid_num() function asserts whether or not the
	 * frame number @p frame is valid.
	 *
	 * @returns If @p frame is valid, non zero is returned. Otherwise,
	 * zero is returned instead.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline int frame_is_valid_num(frame_t frame)
	{
		return (
			(frame >= (UBASE_PHYS >> PAGE_SHIFT)) &&
			(frame < ((UBASE_PHYS >> PAGE_SHIFT) + NUM_UFRAMES))
		);
	}

	/**
	 * @brief Converts a page frame number to an ID of a user page frame.
	 *
	 * @param frame Number of the target page frame.
	 *
	 * @returns ID of target user page frame.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline int frame_num_to_id(frame_t frame)
	{
		/* Invalid frame. */
		if (!frame_is_valid_num(frame))
			return (-1);

		return (frame - (UBASE_PHYS >> PAGE_SHIFT));
	}

	/**
	 * @brief Allocates a page frame.
	 *
	 * @returns Upon successful completion, the number of the
	 * allocated page frame is returned. Upon failure, @p FRAME_NULL
	 * is returned instead.
	 */
	EXTERN frame_t frame_alloc(void);

	/**
	 * @brief Frees a page frame.
	 *
	 * @param frame Number of the target page frame.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int frame_free(frame_t frame);

	/**
	 * @brief Runs unit tests on the frame allocator.
	 */
	EXTERN void frame_test_driver(void);

	/**
	 * @brief Initializes the frame allocator.
	 */
	EXTERN void frame_init(void);

/**@}*/

#endif /** NANVIX_MM_H_ */

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

#ifndef NANVIX_HAL_MEMORY_H_
#define NANVIX_HAL_MEMORY_H_

/**
 * @defgroup kernel-hal-memory Memory
 * @ingroup kernel-hal
 *
 * @brief Memory Interface
 */

	#include <nanvix/const.h>
	#include <nanvix/hal/target.h>

/*============================================================================*
 * Memory Information                                                         *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-memory
 */
/**@{*/

	/**
	 * @brief Memory size (in bytes).
	 */
	#define HAL_MEM_SIZE _HAL_MEM_SIZE

/**@}*/

/*============================================================================*
 * TLB Management                                                             *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-tlb TLB
 * @ingroup kernel-hal-memory
 *
 * @brief Translation Lookaside Buffer
 */
/**@{*/

	/**
	 * @brief TLB entry.
	 */
	struct tlbe;

	/**
	 * @brief Gets the virtual address encoded in a TLB entry.
	 *
	 * @param tlbe Target TLB entry.
	 *
	 * @returns The virtual address encoded in the TLB entry pointed
	 * to by @p tlbe.
	 */
	EXTERN vaddr_t tlbe_vaddr_get(const struct tlbe *tlbe);

	/**
	 * @brief Gets the physical address encoded in a TLB entry.
	 *
	 * @param tlbe Target TLB entry.
	 *
	 * @returns The physical address encoded in the TLB entry pointed
	 * to by @p tlbe.
	 */
	EXTERN paddr_t tlbe_paddr_get(const struct tlbe *tlbe);

	/**
	 * @brief Lookups a TLB entry by virtual address.
	 *
	 * @param handler_num Handler number, identifies which TLB
	 * type should be used.
	 *
	 * @param vaddr Target virtual address.
	 *
	 * @returns Upon successful completion, a pointer to the TLB entry
	 * that matches the virtual address @p vaddr is returned. If no
	 * TLB entry matches @p vaddr, @p NULL is returned instead.
	 */
	EXTERN const struct tlbe *tlb_lookup_vaddr(int handler_num, vaddr_t vaddr);

	/**
	 * @brief Lookups a TLB entry by physical address.
	 *
	 * @param handler_num Handler number, identifies which TLB
	 * type should be used.
	 *
	 * @param paddr Target physical address.
	 *
	 * @returns Upon successful completion, a pointer to the TLB entry
	 * that matches the physical address @p paddr is returned. If no
	 * TLB entry matches @p paddr, @p NULL is returned instead.
	 */
	EXTERN const struct tlbe *tlb_lookup_paddr(int handler_num, paddr_t paddr);

	/**
	 * @brief Encodes a virtual address into the TLB.
	 *
	 * @param handler_num Handler number, identifies which TLB
	 * type should be used.
	 *
	 * @param vaddr Target virtual address.
	 * @param paddr Target physical address.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
#if !((defined(HAL_TLB_HARDWARE) && !defined(__tlb_write_fn)))
	EXTERN int tlb_write(int handler_num, vaddr_t vaddr, paddr_t paddr);
#else
	static inline int tlb_write(int handler_num, vaddr_t vaddr, paddr_t paddr)
	{
		((void) handler_num);
		((void) vaddr);
		((void) paddr);

		return (0);
	}
#endif

	/**
	 * @brief Invalidates a virtual address in the TLB.
	 *
	 * @param handler_num Handler number, identifies which TLB
	 * type should be used.
	 *
	 * @param vaddr Target virtual address.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
#if !((defined(HAL_TLB_HARDWARE) && !defined(__tlb_inval_fn)))
	EXTERN int tlb_inval(int handler_num, vaddr_t vaddr);
#else
	static inline int tlb_inval(int handler_num, vaddr_t vaddr)
	{
		((void) handler_num);
		((void) vaddr);

		return (0);
	}
#endif

	/**
	 * @brief Flushes changes in the TLB.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int tlb_flush(void);

/**@}*/

/*============================================================================*
 * Memory Cache Management                                                    *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-cache Cache
 * @ingroup kernel-hal-memory
 *
 * @brief Memory Cache
 */
/**@{*/

	/**
	 * @brief Invalidates the data cache.
	 */
	EXTERN void hal_dcache_invalidate(void);

/**@}*/

#endif /* NANVIX_HAL_MEMORY_H_ */

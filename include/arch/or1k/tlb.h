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

#ifndef ARCH_OR1K_TLB_H_
#define ARCH_OR1K_TLB_H_

	#include <arch/or1k/core.h>

/**
 * @addtogroup or1k-tlb TLB
 * @ingroup or1k-memory
 *
 * @brief Translation Lookaside Buffer
 */
/**@{*/

/**
 * @if or1k
 */

	/**
	 * @brief Hardware-managed TLB.
	 */
	#define HAL_TLB_HARDWARE

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __tlb_flush_fn /**< tlb_flush() */
	/**@}*/

/**@endif**/

	/**
	 * @brief Flushes the TLB.
	 */
	EXTERN int or1k_tlb_flush(void);

/**
 * @if or1k
 */

	/**
	 * @see or1k_tlb_flush().
	 */
	static inline int tlb_flush(void)
	{
		return (or1k_tlb_flush());
	}

/**@endif*/

/**@}*/
	
#endif /* ARCH_OR1K_TLB_H_ */

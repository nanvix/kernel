/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018-2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_CORE_OR1K_CACHE_H_
#define ARCH_CORE_OR1K_CACHE_H_

/**
 * @addtogroup or1k-core-cache Cache
 * @ingroup or1k-core
 *
 * @brief Memory Cache
 */
/**@{*/

#ifndef _ASM_FILE_

	#include <arch/core/or1k/core.h>

#endif /* _ASM_FILE_ */

	/**
	 * @brief Cache line size (in bytes).
	 *
	 * @todo Check this.
	 */
	#define OR1K_CACHE_LINE_SIZE 64

	/**
	 * @brief Invalidates the data cache.
	 *
	 * @note The or1k target features cache coherency.
	 */
	static inline void or1k_dcache_inval(void)
	{
		or1k_mtspr(OR1K_SPR_DCBIR, 0);
	}

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond mor1kx
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_dcache_invalidate /**< hal_dcache_invalidate() */
	/**@}*/

	/**
	 * @see OR1K_CACHE_LINE_SIZE.
	 */
	#define CACHE_LINE_SIZE OR1K_CACHE_LINE_SIZE

	/**
	 * @see or1k_dcache_inval().
	 */
	static inline void hal_dcache_invalidate(void)
	{
		or1k_dcache_inval();
	}

/**@endcond*/

#endif /* ARCH_CORE_OR1K_CACHE_H_ */

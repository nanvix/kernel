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

#ifndef ARCH_K1B_CACHE_H_
#define ARCH_K1B_CACHE_H_

/**
 * @addtogroup k1b-cache Memory Cache
 * @ingroup k1b
 *
 * @brief Memory Cache
 */
/**@{*/

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_dcache_invalidate
	/**@}*/

	/**
	 * @brief Cache line size (in bytes).
	 *
	 * @todo Check if the following this is valid for cores in IO DDR
	 * and Ethernet Clusters.
	 */
	#if defined(__k1bdp__)
		#define K1B_CACHE_LINE_SIZE 64
	#else
		#define K1B_CACHE_LINE_SIZE 64
	#endif

	/**
	 * @see K1B_CACHE_LINE_SIZE
	 */
	#define CACHE_LINE_SIZE K1B_CACHE_LINE_SIZE

	/**
	 * @brief Invalidates the data cache.
	 *
	 * The k1b_dcache_inval() function invalidates the data cache of
	 * the underlying core. First, it purges the write buffer, then it
	 * waits all pending write operations of other cores to complete,
	 * and finally it performs a full invalidation in the data cache.
	 */
	static inline void k1b_dcache_inval(void)
	{
		__builtin_k1_wpurge();
		__builtin_k1_fence();
		__builtin_k1_dinval();
	}

	/**
	 * @see k1b_dcache_inval()
	 *
	 * @cond k1b
	 */
	static inline void hal_dcache_invalidate(void)
	{
		k1b_dcache_inval();
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_K1B_CACHE_H_ */

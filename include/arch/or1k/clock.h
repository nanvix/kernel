/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_OR1K_CLOCK_H_
#define ARCH_OR1K_CLOCK_H_

/**
 * @addtogroup or1k-Tick Timer
 * @ingroup or1k
 *
 * @brief Tick Timer
 */
/**@{*/

	#include <nanvix/const.h>

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_clock_init
	/**@}*/

	/**
	 * @brief Estimated CPU frequency (in Hz)
	 */
	#define CPU_FREQUENCY 20000000
	
	/**
	 * @brief Initializes the clock driver in the or1k architecture.
	 *
	 * @param freq Target frequency for the clock device.
	 */
	EXTERN void or1k_clock_init(unsigned freq);

	/**
	 * @see or1k_clock_init()
	 *
	 * @cond or1k
	 */
	static inline void hal_clock_init(unsigned freq)
	{
		or1k_clock_init(freq);
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_OR1K_CLOCK */

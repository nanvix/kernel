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

	#ifndef NANVIX_HAL_TARGET_H_
	#error "include <nanvix/hal/target.h> instead"
	#endif

	#include <arch/k1b/k1b.h>
	#include <driver/jtag.h>

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_stdout_init
	#define __hal_stdout_write
	/**@}*/

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
	 * @name Hardware Interrupts
	 */
	/**@{*/
	#define HAL_INT_CLOCK MPPA256_INT_CLOCK0 /*< Programmable interrupt timer. */
	/**@}*/

	/**
	 * @brief Number of cores in a cluster in the Kalray MPPA-256 target.
	 */
	#define _HAL_NUM_CORES K1B_NUM_CORES

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

/**@}*/

#endif /* TARGET_KALRAY_MPPA256_H_ */

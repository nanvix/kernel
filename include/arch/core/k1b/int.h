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

#ifndef ARCH_CORE_K1B_INT_H_
#define ARCH_CORE_K1B_INT_H_

/**
 * @addtogroup k1b-core-int Interrupts
 * @ingroup k1b-core
 *
 * @brief Hardware and Software Interrupts
 */
/**@{*/

	#define __NEED_K1B_IVT
	#include <arch/core/k1b/ivt.h>

	#include <arch/core/k1b/context.h>
	#include <mOS_vcore_u.h>
	#include <vbsp.h>

	/**
	 * @name Hardware Interrupts for Kalray MPPA-256 Target
	 */
	/**@{*/
	#define K1B_INT_CLOCK0     0 /**< Clock 0              */
	#define K1B_INT_CLOCK1     1 /**< Clock 1              */
	#define K1B_INT_TIMER      2 /**< Watchdog Timer       */
	#define K1B_INT_CNOC       3 /**< Control NoC          */
	#define K1B_INT_DNOC       4 /**< Data NoC             */
	#define K1B_INT_DMA        5 /**< DMA                  */
	#define K1B_INT_NOC_ERR    6 /**< NoC Error            */
	#define K1B_INT_TIMER_ERR  7 /**< Watchdog Timer Error */
	#define K1B_INT_PE0        8 /**< Remote Core 0        */
	#define K1B_INT_PE1        9 /**< Remote Core 1        */
	#define K1B_INT_PE2       10 /**< Remote Core 2        */
	#define K1B_INT_PE3       11 /**< Remote Core 3        */
	#define K1B_INT_PE4       12 /**< Remote Core 4        */
	#define K1B_INT_PE5       13 /**< Remote Core 5        */
	#define K1B_INT_PE6       14 /**< Remote Core 6        */
	#define K1B_INT_PE7       15 /**< Remote Core 7        */
	#define K1B_INT_PE8       16 /**< Remote Core 8        */
	#define K1B_INT_PE9       17 /**< Remote Core 9        */
	#define K1B_INT_PE10      18 /**< Remote Core 10       */
	#define K1B_INT_PE11      19 /**< Remote Core 11       */
	#define K1B_INT_PE12      20 /**< Remote Core 12       */
	#define K1B_INT_PE13      21 /**< Remote Core 14       */
	#define K1B_INT_PE14      22 /**< Remote Core 14       */
	#define K1B_INT_PE15      23 /**< Remote Core 15       */
	/**@}*/

	/**
	 * @brief Hardware interrupt dispatcher.
	 *
	 * @param hwintid ID of the hardware interrupt that was triggered.
	 * @param ctx     Interrupted context.
	 */
	extern void k1b_do_hwint(k1b_hwint_id_t hwintid, struct context *ctx);

	/**
	 * @brief Enables interrupts.
	 *
	 * Enables all hardware interrupts in the underlying core.
	 */
	static inline void k1b_hwint_enable(void)
	{
		mOS_it_enable();
	}

	/**
	 * @brief Disables interrupts.
	 *
	 * Disables all hardware interrupts in the underlying core.
	 */
	static inline void k1b_hwint_disable(void)
	{
		mOS_it_disable();
	}

	/**
	 * @brief Sets a handler for a hardware interrupt.
	 *
	 * @param num     Number of the target hardware interrupt.
	 * @param handler Hardware interrupt handler.
	 */
	extern void k1b_hwint_handler_set(int num, void (*handler)(int));

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond k1b
 */

	/**
	 * @brief Number of hardware interrupts in the Kalray MPPA-256 target.
	 */
	#define _HAL_INT_NR K1B_NUM_HWINT

	/**
	 * @name Hardware Interrupts
	 */
	/**@{*/
	#define HAL_INT_CLOCK K1B_INT_CLOCK0 /*< Programmable interrupt timer. */
	/**@}*/

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_disable_interrupts
	#define __hal_enable_interrupts
	#define __hal_interrupt_set_handler
	/**@}*/

	/**
	 * @see k1b_hwint_enable().
	 */
	static inline void hal_enable_interrupts(void)
	{
		k1b_hwint_enable();
	}

	/**
	 * @see k1b_hwint_disable().
	 */
	static inline void hal_disable_interrupts(void)
	{
		k1b_hwint_disable();
	}

	/**
	 * @see k1b_hwint_handler_set().
	 */
	static inline void hal_interrupt_set_handler(int num, void (*handler)(int))
	{
		k1b_hwint_handler_set(num, handler);
	}

/**@endcond*/

#endif /* ARCH_CORE_K1B_INT_H_ */

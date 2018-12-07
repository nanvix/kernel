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

#ifndef ARCH_K1B_INT_H_
#define ARCH_K1B_INT_H_

/**
 * @addtogroup k1b-int Interrupt
 * @ingroup k1b
 *
 * @brief Interface for dealing with hardware and software interrupts.
 */
/**@{*/

	#include <arch/k1b/context.h>
	#include <arch/k1b/ivt.h>
	#include <nanvix/const.h>
	#include <mOS_vcore_u.h>
	#include <vbsp.h>

	/**
	 * @brief Hardware interrupt dispatcher.
	 *
	 * @param hwintid ID of the hardware interrupt that was triggered.
	 * @param ctx     Interrupted context.
	 *
	 * @note This function is called from mOS.
	 */
	EXTERN void k1b_do_hwint(k1b_hwint_id_t hwintid, struct k1b_context *ctx);

	/**
	 * @brief Enables interrupts.
	 *
	 * Enables all hardware interrupts in the underlying core.
	 */
	static inline void k1b_hwint_enable(void)
	{
		mOS_set_it_level(0);
		mOS_it_enable();
	}

	/**
	 * @see k1b_hwint_enable()
	 *
	 * @cond k1b
	 */
	static inline void hal_enable_interrupts(void)
	{
		k1b_hwint_enable();
	}
	/**@endcond*/

	/**
	 * @brief Disables interrupts.
	 *
	 * Disables all hardware interrupts in the underlying core.
	 */
	static inline void k1b_hwint_disable(void)
	{
		mOS_it_disable();
		mOS_set_it_level(0xf);
	}

	/**
	 * @see k1b_hwint_disable()
	 *
	 * @cond k1b
	 */
	static inline void hal_disable_interrupts(void)
	{
		k1b_hwint_disable();
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_K1B_INT_H_ */

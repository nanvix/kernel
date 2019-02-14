/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_CORE_OR1K_INT_H_
#define ARCH_CORE_OR1K_INT_H_

/**
 * @addtogroup or1k-core-int Hardware Interrupts
 * @ingroup or1k-core
 *
 * @brief Hardware and Software Interrupts
 */
/**@{*/

	#define __NEED_OR1K_CONTEXT
	#include <arch/core/or1k/context.h>

	#define __NEED_OR1K_PIC
	#include <arch/core/or1k/pic.h>

#ifndef _ASM_FILE_

	#include <arch/core/or1k/core.h>
	#include <nanvix/const.h>

#endif /* _ASM_FILE_ */

	/**
	 * @brief Number of hardware interrupts in the or1k architecture.
	 */
	#define OR1K_NUM_HWINT 3

#ifndef _ASM_FILE_

	/**
	 * @brief System Call Hook
	 */
	EXTERN void syscall(void);

	/**
	 * @name Hardware Interrupt Hooks
	 */
	/**@{*/
	EXTERN void hwint0(void);
	EXTERN void hwint1(void);
	EXTERN void hwint2(void);
	/**@}*/

	/**
	 * @brief High-level hardware interrupt dispatcher.
	 *
	 * @param num Number of triggered hardware interrupt.
	 * @param ctx Interrupted execution context.
	 *
	 * @note This function is called from assembly code.
	 */
	EXTERN void or1k_do_hwint(int num, const struct context *ctx);

	/**
	 * @brief Enables hardware interrupts.
	 *
	 * The or1k_sti() function enables all hardware interrupts in the
	 * underlying or1k core.
	 */
	static inline void or1k_hwint_enable(void)
	{
		or1k_mtspr(OR1K_SPR_SR, or1k_mfspr(OR1K_SPR_SR) | OR1K_SPR_SR_IEE
			| OR1K_SPR_SR_TEE);
	}

	/**
	 * @brief Disables hardware interrupts.
	 *
	 * The or1k_cli() function disables all hardware interrupts in the
	 * underlying or1k core.
	 */
	static inline void or1k_hwint_disable(void)
	{
		 or1k_mtspr(
			OR1K_SPR_SR,
			or1k_mfspr(OR1K_SPR_SR) & ~(OR1K_SPR_SR_IEE | OR1K_SPR_SR_TEE)
		);
	}

	/**
	 * @brief Sets a handler for a hardware interrupt.
	 *
	 * @param num     Number of the target hardware interrupt.
	 * @param handler Hardware interrupt handler.
	 */
	EXTERN void or1k_hwint_handler_set(int num, void (*handler)(int));

#endif

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
	#define __hal_disable_interrupts    /**< hal_disable_interrupts()    */
	#define __hal_enable_interrupts     /**< hal_enable_interrupts()     */
	#define __hal_interrupt_set_handler /**< hal_interrupt_set_handler() */
	/**@}*/

	/**
	 * @brief Number of hardware interrupts.
	 */
	#define _HAL_INT_NR OR1K_NUM_HWINT

	/**
	 * @name Hardware Interrupts
	 */
	/**@{*/
	#define HAL_INT_CLOCK OR1K_INT_CLOCK /*< Programmable interrupt timer. */
	/**@}*/

#ifndef _ASM_FILE_

	/**
	 * @see or1k_sti()
	 */
	static inline void hal_enable_interrupts(void)
	{
		or1k_hwint_enable();
	}

	/**
	 * @see or1k_cli()
	 */
	static inline void hal_disable_interrupts(void)
	{
		or1k_hwint_disable();
	}

	/**
	 * @see or1k_hwint_handler_set()
	 */
	static inline void hal_interrupt_set_handler(int num, void (*handler)(int))
	{
		or1k_hwint_handler_set(num, handler);
	}

#endif /* _ASM_FILE_ */

/**@endcond*/

#endif /* ARCH_CORE_OR1K_INT_H_ */

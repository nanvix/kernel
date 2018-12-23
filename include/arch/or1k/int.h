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

#ifndef ARCH_OR1K_INT_H_
#define ARCH_OR1K_INT_H_

/**
 * @addtogroup or1k-int Hardware Interrupts
 * @ingroup or1k
 *
 * @brief Hardware and Software Interrupts
 */
/**@{*/

	#include <nanvix/const.h>

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_disable_interrupts
	#define __hal_enable_interrupts
	#define __hal_interrupt_set_handler
	/**@}*/

	/**
	 * @brief Number of hardware interrupts in the or1k architecture.
	 */
	#define OR1K_NUM_HWINT 3
	
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
	 * @brief Disables hardware interrupts.
	 *
	 * The or1k_cli() function disables all hardware interrupts in the
	 * underlying or1k core.
	 */
	static inline void or1k_cli(void)
	{
		 or1k_mtspr(OR1K_SPR_SR, or1k_mfspr(OR1K_SPR_SR) & ~(OR1K_SPR_SR_IEE
		 	| OR1K_SPR_SR_TEE));
	}

	/**
	 * @see or1k_cli()
	 *
	 * @cond or1k
	 */
	static inline void hal_disable_interrupts(void)
	{
		or1k_cli();
	}
	/**@endcond*/

	/**
	 * @brief Enables hardware interrupts.
	 *
	 * The or1k_sti() function enables all hardware interrupts in the
	 * underlying or1k core.
	 */
	static inline void or1k_sti(void)
	{
		or1k_mtspr(OR1K_SPR_SR, or1k_mfspr(OR1K_SPR_SR) | OR1K_SPR_SR_IEE
			| OR1K_SPR_SR_TEE);
	}

	/**
	 * @see or1k_sti()
	 *
	 * @cond or1k
	 */
	static inline void hal_enable_interrupts(void)
	{
		or1k_sti();
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_OR1K_INT_H_ */

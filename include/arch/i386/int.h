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

#ifndef ARCH_I386_INT_H_
#define ARCH_I386_INT_H_

/**
 * @addtogroup i386-int Hardware Interrupts
 * @ingroup i386
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
	 * @brief Number of hardware interrupts in the i386 architecture.
	 */
	#define I386_INT_NR 16

	/* Software interrupt hooks. */
	/**
	 * @name Software Interrupt Hooks
	 */
	/**@{*/
	EXTERN void swint0(void);
	EXTERN void swint1(void);
	EXTERN void swint2(void);
	EXTERN void swint3(void);
	EXTERN void swint4(void);
	EXTERN void swint5(void);
	EXTERN void swint6(void);
	EXTERN void swint7(void);
	EXTERN void swint8(void);
	EXTERN void swint9(void);
	EXTERN void swint10(void);
	EXTERN void swint11(void);
	EXTERN void swint12(void);
	EXTERN void swint13(void);
	EXTERN void swint14(void);
	EXTERN void swint15(void);
	EXTERN void swint16(void);
	EXTERN void swint17(void);
	EXTERN void swint19(void);
	/**@}*/
	
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
	EXTERN void hwint3(void);
	EXTERN void hwint4(void);
	EXTERN void hwint5(void);
	EXTERN void hwint6(void);
	EXTERN void hwint7(void);
	EXTERN void hwint8(void);
	EXTERN void hwint9(void);
	EXTERN void hwint10(void);
	EXTERN void hwint11(void);
	EXTERN void hwint12(void);
	EXTERN void hwint13(void);
	EXTERN void hwint14(void);
	EXTERN void hwint15(void);
	EXTERN void hwint16(void);
	/**@}*/

	/**
	 * @brief Disables hardware interrupts.
	 *
	 * The i386_cli() function disables all hardware interrupts in the
	 * underlying i386 core.
	 */
	static inline void i386_cli(void)
	{
		 __asm__("cli");
	}

	/**
	 * @see i386_cli()
	 *
	 * @cond i386
	 */
	static inline void hal_disable_interrupts(void)
	{
		i386_cli();
	}
	/**@endcond*/

	/**
	 * @brief Enables hardware interrupts.
	 *
	 * The i386_sti() function enables all hardware interrupts in the
	 * underlying i386 core.
	 */
	static inline void i386_sti(void)
	{
		 __asm__("sti");
	}

	/**
	 * @see i386_sti()
	 *
	 * @cond i386
	 */
	static inline void hal_enable_interrupts(void)
	{
		i386_sti();
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_I386_INT_H_ */

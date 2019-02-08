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

/*============================================================================*
 *                             Interrupt Interface                            *
 *============================================================================*/

/**
 * @addtogroup i386-int Hardware Interrupts
 * @ingroup i386
 *
 * @brief Hardware and Software Interrupts
 */
/**@{*/

	#include <nanvix/const.h>
	#include <arch/i386/context.h>

	/**
	 * @brief Number of hardware interrupts in the i386 architecture.
	 */
	#define I386_NUM_HWINT 16
	
	/**
	 * @brief System Call Hook
	 */
	EXTERN void syscall(void);
	
	/**
	 * @name Hardware Interrupt Hooks
	 */
	/**@{*/
	EXTERN void _do_hwint0(void);
	EXTERN void _do_hwint1(void);
	EXTERN void _do_hwint2(void);
	EXTERN void _do_hwint3(void);
	EXTERN void _do_hwint4(void);
	EXTERN void _do_hwint5(void);
	EXTERN void _do_hwint6(void);
	EXTERN void _do_hwint7(void);
	EXTERN void _do_hwint8(void);
	EXTERN void _do_hwint9(void);
	EXTERN void _do_hwint10(void);
	EXTERN void _do_hwint11(void);
	EXTERN void _do_hwint12(void);
	EXTERN void _do_hwint13(void);
	EXTERN void _do_hwint14(void);
	EXTERN void _do_hwint15(void);
	/**@}*/

	/**
	 * @brief High-level hardware interrupt dispatcher.
	 *
	 * @param num Number of triggered hardware interrupt.
	 * @param ctx Interrupted execution context.
	 *
	 * @note This function is called from assembly code.
	 */
	EXTERN void i386_do_hwint(int num, const struct context *ctx);

	/**
	 * @brief Enables hardware interrupts.
	 *
	 * The i386_hwint_enable() function enables all hardware interrupts in the
	 * underlying i386 core.
	 */
	static inline void i386_hwint_enable(void)
	{
		 __asm__("sti");
	}

	/**
	 * @brief Disables hardware interrupts.
	 *
	 * The i386_hwint_disable() function disables all hardware interrupts in the
	 * underlying i386 core.
	 */
	static inline void i386_hwint_disable(void)
	{
		 __asm__("cli");
	}

	/**
	 * @brief Sets a handler for a hardware interrupt.
	 *
	 * @param num     Number of the target hardware interrupt.
	 * @param handler Hardware interrupt handler.
	 */
	EXTERN void i386_hwint_handler_set(int num, void (*handler)(int));

/**@}*/

/*============================================================================*
 *                              Exported Interface                            *
 *============================================================================*/

/**
 * @cond i386
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_disable_interrupts
	#define __hal_enable_interrupts
	#define __hal_interrupt_set_handler
	/**@}*/

/**
 * @addtogroup kernel-hal-interrupts Interrupt
 * @ingroup kernel-hal-cpu
 */
/**@{*/

	/**
	 * @see i386_hwint_enable()
	 */
	static inline void hal_enable_interrupts(void)
	{
		i386_hwint_enable();
	}

	/**
	 * @see i386_hwint_disable()
	 */
	static inline void hal_disable_interrupts(void)
	{
		i386_hwint_disable();
	}

	/**
	 * @see i386_hwint_handler_set().
	 */
	static inline void hal_interrupt_set_handler(int num, void (*handler)(int))
	{
		i386_hwint_handler_set(num, handler);
	}

/**@}*/

/**@endcond*/

#endif /* ARCH_I386_INT_H_ */

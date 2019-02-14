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
 * @addtogroup i386-core-int Hardware Interrupts
 * @ingroup i386-core
 *
 * @brief Hardware and Software Interrupts
 */
/**@{*/

	#include <nanvix/const.h>
	#include <arch/core/i386/context.h>

	/**
	 * @brief Number of hardware interrupts in the i386 architecture.
	 */
	#define I386_NUM_HWINT 16

	/**
	 * @name Hardware Interrupts for the IBM PC Target
	 */
	/**@{*/
	#define I386_PC_INT_CLOCK    0 /*< Programmable interrupt timer.              */
	#define I386_PC_INT_KEYBOARD 1 /*< Keyboard.                                  */
	#define I386_PC_INT_COM2     3 /*< COM2.                                      */
	#define I386_PC_INT_COM1     4 /*< COM1.                                      */
	#define I386_PC_INT_LPT2     5 /*< LPT2.                                      */
	#define I386_PC_INT_FLOPPY   6 /*< Floppy disk.                               */
	#define I386_PC_INT_LPT1     7 /*< LPT1.                                      */
	#define I386_PC_INT_CMOS     8 /*< CMOS real-time clock.                      */
	#define I386_PC_INT_SCSI1    9 /*< Free for peripherals (legacy SCSI or NIC). */
	#define I386_PC_INT_SCSI2   10 /*< Free for peripherals (legacy SCSI or NIC). */
	#define I386_PC_INT_SCSI3   11 /*< Free for peripherals (legacy SCSI or NIC). */
	#define I386_PC_INT_MOUSE   12 /*< PS2 mouse.                                 */
	#define I386_PC_INT_COPROC  13 /*< FPU, coprocessor or inter-processor.       */
	#define I386_PC_INT_ATA1    14 /*< Primary ATA hard disk.                     */
	#define I386_PC_INT_ATA2    15 /*< Secondary ATA hard disk.                   */
	/**@}*/
	
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
	 * @brief Number of hardware interrupts in the IBM PC target.
	 */
	#define _HAL_INT_NR I386_NUM_HWINT

	/**
	 * @name Hardware Interrupts
	 */
	/**@{*/
	#define HAL_INT_CLOCK I386_PC_INT_CLOCK /*< Programmable interrupt timer. */
	/**@}*/

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

/**@endcond*/

#endif /* ARCH_I386_INT_H_ */

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

#ifndef NANVIX_HAL_EXCEPTION_H_
#define NANVIX_HAL_EXCEPTION_H_

/**
 * @addtogroup kernel-hal-exception Exception
 * @ingroup kernel-hal-cpu
 *
 * @brief Exceptions
 *
 * Exceptions occur when the processor detects an error condition
 * while executing an instruction, such as division by zero. The
 * processor detects a variety of error conditions including
 * protection violations, page faults, and internal machine faults.
 *
 * Exceptions are classified as faults, traps, or aborts depending on
 * the way they are reported and whether the instruction that caused
 * the exception can be restarted without loss of program or task
 * continuity.
 *
 * A trap is an exception that is reported immediately following the
 * execution of the trapping instruction. Traps allow execution of a
 * program or task to be continued without loss of program continuity.
 * The return address for the trap handler points to the instruction
 * to be executed after the trapping instruction.
 *
 * A fault is an exception that can generally be corrected and that,
 * once corrected, allows the program to be restarted with no loss of
 * continuity. When a fault is reported, the processor restores the
 * machine state to the state prior to the beginning of execution of
 * the faulting instruction. The return address  for the fault handler
 * points to the faulting instruction, rather than to the instruction
 * following the faulting instruction.
 *
 * An abort is an exception that does not always report the precise
 * location of the instruction causing the exception and does not
 * allow a restart of the program or task that caused the exception.
 * Aborts are used to report severe errors, such as hardware errors
 * and inconsistent or illegal values in system tables.
 */
/**@{*/

	#include <nanvix/hal/target.h>
	#include <nanvix/const.h>

	/**
	 * @brief Number of exceptions.
	 */
	#define HAL_NUM_EXCEPTION _HAL_NUM_EXCEPTION

	/**
	 * @brief Exception handler
	 */
	typedef void (*hal_exception_handler_t)(int);

	/**
	 * @brief Sets a handler for an exception.
	 *
	 * @param excpnum Number of the target exception.
	 * @param handler Handler.
	 *
	 * @note This function does not check if a handler is already
	 * set for the target hardware exception.
	 */
	EXTERN void hal_exception_set_handler(int excpnum, hal_exception_handler_t handler);

	/**
	 * @brief Registers an exception handler.
	 *
	 * @param excpnum Number of the exception.
	 * @param handler Interrupt handler.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int hal_exception_register(int excpnum, hal_exception_handler_t handler);

/**@}*/

#endif /* NANVIX_EXCEPTION_H_ */

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

#ifndef ARCH_K1B_EXCP_H_
#define ARCH_K1B_EXCP_H_

	#include <nanvix/const.h>

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_exception_set_handler
	/**@}*/

	/**
	 * @brief Number of exceptions.
	 */
	#define K1B_NUM_EXCEPTION 16

	/**
	 * @name Exceptions
	 */
	/**@{*/
	#define K1B_EXCP_RESET            0 /**< Reset Exception                               */
	#define K1B_EXCP_OPCODE           1 /**< Bad Instruction Bundle                        */
	#define K1B_EXCP_PROTECTION       2 /**< Protection Fault                              */
	#define K1B_EXCP_ALIGNMENT        3 /**< Alignment Check                               */
	#define K1B_EXCP_RANGE_CODE       4 /**< Instruction Out of Range                      */
	#define K1B_EXCP_RANGE_DATA       5 /**< Data Out of Range                             */
	#define K1B_EXCP_DOUBLE_ECC_CODE  6 /**< Double ECC Fault on Out of Range Instruction  */
	#define K1B_EXCP_DOUBLE_ECC_DATA  7 /**< Double ECC Fault on Out of Range Data         */
	#define K1B_EXCP_PARITY_CODE      8 /**< Parity Error on Out of Range Instruction      */
	#define K1B_EXCP_PARITY_DATA      9 /**< Parity Error on Out of Range Data             */
	#define K1B_EXCP_SINGLE_ECC_CODE 10 /**< Single ECC Fault on Out of Range Instruction  */
	#define K1B_EXCP_SINGLE_ECC_DATA 11 /**< Single ECC Fault on Out of Range Data         */
	#define K1B_EXCP_PAGE_FAULT      12 /**< Page Fault                                    */
	#define K1B_EXCP_PAGE_PROTECTION 13 /**< Page Protection                               */
	#define K1B_EXCP_WRITE_CLEAN     14 /**< Write to Clean Exception                      */
	#define K1B_EXCP_ATOMIC_CLEAN    15 /**< Atomic to Clean Exception                     */
	/**@}*/

	/**
	 * @brief Sets a handler for an exception.
	 *
	 * @param excpnum Number of the target exception.
	 * @param handler Handler.
	 *
	 * @note This function does not check if a handler is already
	 * set for the target hardware exception.
	 */
	EXTERN void k1b_excp_set_handler(int excpnum, void (*handler)(int));

	/**
	 * @see k1b_excp_set_handler()
	 */
	static inline void hal_exception_set_handler(int excpnum, void (*handler)(int))
	{
		k1b_excp_set_handler(excpnum, handler);
	}

	/**
	 * @brief Low-level exception dispatcher.
	 */
	EXTERN void _do_excp(void);

	/**
	 * @brief High-level exception dispatcher.
	 *
	 * @param excpnum Number of tirggered exception.
	 *
	 * @note This function is called from assembly code.
	 */
	EXTERN void do_excp(int excpnum);

#endif /* ARCH_K1B_EXCP_H_ */

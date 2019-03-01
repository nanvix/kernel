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

	/* Core Interface Implementation */
	#include <nanvix/hal/core/_core.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Constants */
	#ifndef HAL_NUM_EXCEPTIONS
	#error "HAL_NUM_EXCEPTIONS not defined"
	#endif
	#ifndef EXCP_INVALID_OPCODE
	#error "EXCP_INVALID_OPCODE not defined"
	#endif
	#ifndef EXCP_PAGE_FAULT
	#error "EXCP_PAGE_FAULT not defined"
	#endif
	#ifndef EXCP_PAGE_PROTECTION
	#error "EXCP_PAGE_PROTECTION not defined"
	#endif
	#ifdef HAL_TLB_SOFTWARE
		#ifndef EXCP_DTLB_FAULT
		#error "EXCP_DTLB_FAULT not defined"
		#endif
		#ifndef EXCP_ITLB_FAULT
		#error "EXCP_ITLB_FAULT not defined"
		#endif
	#endif
	#ifndef EXCP_GENERAL_PROTECTION
	#error "EXCP_GENERAL_PROTECTION not defined"
	#endif

	/* Functions */
	#ifndef __exception_struct
	#error "struct exception not defined?"
	#endif
	#ifndef __exception_get_addr
	#error "exception_get_addr() not defined?"
	#endif
	#ifndef __exception_get_instr
	#error "exception_get_instr() not defined?"
	#endif
	#ifndef __exception_get_num
	#error "exception_get_num() not defined?"
	#endif
	#ifndef __exception_set_handler
	#error "exception_set_handler() not defined?"
	#endif

/*============================================================================*
 * Exception Interface                                                        *
 *============================================================================*/

	#include <nanvix/hal/core/context.h>
	#include <nanvix/const.h>

/**
 * @addtogroup kernel-hal-core-exception Exception
 * @ingroup kernel-hal-core
 *
 * @brief Exceptions HAL Interface
  */
/**@{*/

	/**
	 * @brief Exception information
	 */
	struct exception;

	/**
	 * @brief Exception handler
	 */
	typedef void (*hal_exception_handler_t)(const struct exception *, const struct context *);

	/**
	 * @brief Gets the number of an exception.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The exception number stored in the exception
	 * information structure pointed to by @p excp.
	 */
	EXTERN int hal_excp_get_num(const struct exception *excp);

	/**
	 * @brief Gets the address of an exception.
	 *
	 * The k1b_excp_get_num() function gets the exception address.
	 * stored in the exception information structure pointed to by @p
	 * excp.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The exception address stored in the exception
	 * information structure pointed to by @p excp.
	 */
	EXTERN vaddr_t hal_excp_get_addr(const struct exception *excp);

	/**
	 * @brief Sets a handler for an exception.
	 *
	 * @param excpnum Number of the target exception.
	 * @param handler Exception handler.
	 *
	 * @note This function does not check if a handler is already
	 * set for the target hardware exception.
	 */
	EXTERN void hal_exception_set_handler(int excpnum, hal_exception_handler_t handler);

/**@}*/

#endif /* NANVIX_EXCEPTION_H_ */

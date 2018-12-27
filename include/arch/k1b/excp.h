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

/**
 * @cond k1b
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __exception_struct          /**< Exception Information       */
	#define __hal_exception_set_handler /**< hal_exception_set_handler() */
	/**@}*/

/**@endcond*/

	/**
	 * @brief Exception information size (in bytes).
	 */
	#define K1B_EXCEPTION_SIZE 12

	/**
	 * @name Offsets to the Exception Information structure.
	 */
	/**@{*/
	#define K1B_EXCEPTION_NUM 0 /**< Exception Number      */
	#define K1B_EXCEPTION_EA  4 /**< Exception Address     */
	#define K1B_EXCEPTION_SPC 8 /**< Saved Program Counter */
	/**@}*/

#ifndef _ASM_FILE_

	#include <nanvix/const.h>
	#include <arch/k1b/context.h>
	#include <arch/k1b/mmu.h>

	/**
	 * @brief Number of exceptions.
	 */
	#define K1B_NUM_EXCEPTIONS 16

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
	 * Exception information.
	 */
	struct exception
	{
		uint32_t num; /**< Exception number.      */
		uint32_t ea;  /**< Exception address.     */
		uint32_t spc; /**< Saved program counter. */
	} __attribute__((packed));

	/**
	 * @brief Gets the number of an exception.
	 *
	 * The k1b_excp_get_num() function gets the exception number
	 * stored in the exception information structure pointed to by @p
	 * excp.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The exception number stored in the exception
	 * information structure pointed to by @p excp.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline int k1b_excp_get_num(const struct exception *excp)
	{
		return (excp->num);
	}

	/**
	 * @brief Gets the address of an exception.
	 *
	 * The k1b_excp_get_num() function gets the exception address
	 * stored in the exception information structure pointed to by @p
	 * excp.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The exception address stored in the exception
	 * information structure pointed to by @p excp.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline vaddr_t k1b_excp_get_addr(const struct exception *excp)
	{
		return (excp->ea);
	}

	/**
	 * @brief Gets the program counter at an exception.
	 *
	 * The k1b_excp_get_num() function gets the program counter
	 * stored in the exception information structure pointed to by @p
	 * excp.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The program counter stored in the exception
	 * information structure pointed to by @p excp.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline vaddr_t k1b_excp_get_spc(const struct exception *excp)
	{
		return (excp->spc);
	}

	/**
	 * @brief Sets a handler for an exception.
	 *
	 * @param excpnum Number of the target exception.
	 * @param handler Handler.
	 *
	 * @note This function does not check if a handler is already
	 * set for the target hardware exception.
	 *
	 */
	EXTERN void k1b_excp_set_handler(
		int excpnum,
		void (*handler)(const struct exception *, const struct context *)
	);

	/**
	 * @brief Low-level exception dispatcher.
	 */
	EXTERN void _do_excp(void);

	/**
	 * @brief High-level exception dispatcher.
	 *
	 * @brief excp Exception information.
	 * @brief ctx  Saved execution context.
	 *
	 * @note This function is called from assembly code.
	 */
	EXTERN void do_excp(const struct exception *excp, const struct context *ctx);

/**
 * @cond k1b
 */

	/**
	 * @brief Exception Codes
	 */
	/**@*/
	#define EXCP_INVALID_OPCODE      K1B_EXCP_OPCODE          /**< Invalid Opcode     */
	#define EXCP_PAGE_FAULT          K1B_EXCP_PAGE_FAULT      /**< Page Fault         */
	#define EXCP_PAGE_PROTECTION     K1B_EXCP_PAGE_PROTECTION /**< Page Protection    */
	#define EXCP_TLB_FAULT           K1B_EXCP_PAGE_FAULT      /**< TLB Fault          */
	#define EXCP_GENERAL_PROTECTION  K1B_EXCP_PROTECTION      /**< General Protection */
	/**@}*/

	/**
	 * @see k1b_excp_get_num().
	 */
	static inline int hal_exception_get_num(const struct exception *excp)
	{
		return (k1b_excp_get_num(excp));
	}

	/**
	 * @see k1b_excp_get_addr().
	 */
	static inline int hal_exception_get_addr(const struct exception *excp)
	{
		return (k1b_excp_get_addr(excp));
	}

	/**
	 * @see k1b_excp_get_spc().
	 */
	static inline int hal_exception_get_pc(const struct exception *excp)
	{
		return (k1b_excp_get_spc(excp));
	}

	/**
	 * @see k1b_excp_set_handler()
	 */
	static inline void hal_exception_set_handler(
		int excpnum,
		void (*handler)(const struct exception *, const struct context *)
	)
	{
		k1b_excp_set_handler(excpnum, handler);
	}

/**@endcond*/

#endif /* _ASM_FILE_ */

#endif /* ARCH_K1B_EXCP_H_ */

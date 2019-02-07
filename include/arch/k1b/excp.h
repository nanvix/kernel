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
 * @addtogroup k1b-exception Exception
 * @ingroup k1b
 *
 * @brief Exceptions
 */
/**@{*/

	/**
	 * @name Provided Interface
	 *
	 * @cond k1b
	 */
	/**@{*/
	#define __exception_struct      /**< @ref exception               */
	#define __exception_get_addr    /**< @ref exception_get_addr()    */
	#define __exception_get_instr   /**< @ref exception_get_instr()   */
	#define __exception_get_num     /**< @ref exception_get_num()     */
	#define __exception_set_handler /**< @ref exception_set_handler() */
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
	 *
	 * Number of exceptions natively supported by the hardware.
	 */
	#define K1B_NUM_EXCEPTIONS 16

	/**
	 * @brief Number of virtual exceptions.
	 *
	 * Number of exceptions virtualized in software.
	 */
	#define K1B_NUM_EXCEPTIONS_VIRT 1

	/**
	 * @name Hardware Exceptions
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
	#define K1B_EXCP_TLB_FAULT       12 /**< TLB Fault                                     */
	#define K1B_EXCP_PAGE_PROTECTION 13 /**< Page Protection                               */
	#define K1B_EXCP_WRITE_CLEAN     14 /**< Write to Clean Exception                      */
	#define K1B_EXCP_ATOMIC_CLEAN    15 /**< Atomic to Clean Exception                     */
	/**@}*/

	/**
	 * @name Virtual Exceptions
	 */
	/**@{*/
	#define K1B_EXCP_VIRT_PAGE_FAULT 16 /**< Page Fault (Virtual Exception) */
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
	 * @brief Exception handler.
	 */
	typedef void (*k1b_exception_handler_fn)(const struct exception *, const struct context *);

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
	 * @see k1b_excp_get_num().
	 *
	 * @cond k1b
	 */
	static inline int exception_get_num(const struct exception *excp)
	{
		return (k1b_excp_get_num(excp));
	}
	/**@endcond*/

	/**
	 * @brief Gets the address of an exception.
	 *
	 * The k1b_excp_get_addr() function gets the exception address
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
	 * @see k1b_excp_get_addr().
	 *
	 * @cond k1b
	 */
	static inline int exception_get_addr(const struct exception *excp)
	{
		return (k1b_excp_get_addr(excp));
	}
	/**@endcond*/

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
	 * @see k1b_excp_get_spc().
	 *
	 * @cond k1b
	 */
	static inline int exception_get_instr(const struct exception *excp)
	{
		return (k1b_excp_get_spc(excp));
	}
	/**@endcond*/

	/**
	 * @brief Sets a handler for an exception.
	 *
	 * @param num     Number of the target exception.
	 * @param handler Exception handler.
	 *
	 * @note This function does not check if a handler is already
	 * set for the target hardware exception.
	 *
	 */
	EXTERN void k1b_excp_set_handler(int num, k1b_exception_handler_fn handler);

	/**
	 * @see k1b_excp_set_handler()
	 *
	 * @cond k1b
	 */
	static inline void exception_set_handler(int num, k1b_exception_handler_fn handler)
	{
		k1b_excp_set_handler(num, handler);
	}
	/**@endcond*/

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
	#define EXCP_PAGE_FAULT          K1B_EXCP_VIRT_PAGE_FAULT /**< Page Fault         */
	#define EXCP_PAGE_PROTECTION     K1B_EXCP_PAGE_PROTECTION /**< Page Protection    */
	#define EXCP_ITLB_FAULT          K1B_EXCP_TLB_FAULT       /**< I-TLB Fault        */
	#define EXCP_DTLB_FAULT          K1B_EXCP_TLB_FAULT       /**< D-TLB Fault        */
	#define EXCP_GENERAL_PROTECTION  K1B_EXCP_PROTECTION      /**< General Protection */
	/**@}*/

/**@endcond*/

#endif /* _ASM_FILE_ */

/**@}*/

#endif /* ARCH_K1B_EXCP_H_ */

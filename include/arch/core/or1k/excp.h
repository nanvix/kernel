/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_CORE_OR1K_EXCP_H_
#define ARCH_CORE_OR1K_EXCP_H_

/**
 * @addtogroup or1k-core-exception Exception
 * @ingroup or1k-core
 *
 * @brief Exceptions
 */
/**@{*/

	#define __NEED_OR1K_CONTEXT
	#include <arch/core/or1k/context.h>

#ifndef _ASM_FILE_

	#define __NEED_VADDR_T
	#define __NEED_PADDR_T
	#include <arch/core/or1k/types.h>

	#include <nanvix/const.h>
	#include <stdint.h>

#endif /* _ASM_FILE_ */

	/**
	 * @brief Exception information size (in bytes).
	 */
	#define OR1K_EXCEPTION_SIZE 12

	/**
	 * @name Offsets to the Exception Information structure.
	 *
	 * @see exception
	 */
	/**@{*/
	#define OR1K_EXCEPTION_NUM   0 /**< Exception Number      */
	#define OR1K_EXCEPTION_EEAR  4 /**< Exception Address     */
	#define OR1K_EXCEPTION_EPCR  8 /**< Saved Program Counter */
	/**@}*/

	/**
	 * @brief Number of exceptions in the or1k core.
	 */
	#define OR1K_NUM_EXCEPTIONS 10

	/**
	 * @name or1k Exception Codes
	 */
	/**@{*/
	#define OR1K_EXCP_RESET                 0 /**@< Reset exception          */
	#define OR1K_EXCP_BUSERROR              1 /**@< Bus Error Exception      */
	#define OR1K_EXCP_PAGE_FAULT            2 /**@< Page fault Exception     */
	#define OR1K_EXCP_ALIGNMENT             3 /**@< Alignment Exception      */
	#define OR1K_EXCP_ILLEGAL_INSTRUCTION   4 /**@< Illegal Instruction      */
	#define OR1K_EXCP_DTLB_FAULT            5 /**@< Data TLB Fault           */
	#define OR1K_EXCP_ITLB_FAULT            6 /**@< Instruction TLB Fault    */
	#define OR1K_EXCP_RANGE                 7 /**@< Range Exception          */
	#define OR1K_EXCP_FLOAT_POINT           8 /**@< Floating Point Exception */
	#define OR1K_EXCP_TRAP                  9 /**@< Trap Exception           */
	/**@}*/

#ifndef _ASM_FILE_

/**
 * @cond mor1kx
 */

	/**
	 * @brief Exception information.
	 */
	struct exception
	{
		uint32_t num;    /**< Exception number.      */
		uint32_t eear;   /**< Exception address.     */
		uint32_t epcr;   /**< Saved program counter. */
	} __attribute__((packed));

/**@endif*/

	/**
	 * @brief Exception handler.
	 */
	typedef void (*or1k_exception_handler_fn)(const struct exception *, const struct context *);

	/**
	 * @brief Gets the number of an exception.
	 *
	 * The or1k_excp_get_num() function gets the exception number
	 * stored in the exception information structure pointed to by @p
	 * excp.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The exception number stored in the exception
	 * information structure pointed to by @p excp.
	 *
	 * @author Davidson Francis
	 */
	static inline int or1k_excp_get_num(const struct exception *excp)
	{
		return (excp->num);
	}

	/**
	 * @brief Gets the address of an exception.
	 *
	 * The or1k_excp_get_addr() function gets the exception address
	 * stored in the exception information structure pointed to by @p
	 * excp.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The exception address stored in the exception
	 * information structure pointed to by @p excp.
	 *
	 * @author Davidson Francis
	 */
	static inline vaddr_t or1k_excp_get_addr(const struct exception *excp)
	{
		return (excp->eear);
	}

	/**
	 * @brief Gets the program counter at an exception.
	 *
	 * The or1k_excp_get_num() function gets the program counter
	 * stored in the exception information structure pointed to by @p
	 * excp.
	 *
	 * @param excp Target exception information structure.
	 *
	 * @returns The program counter stored in the exception
	 * information structure pointed to by @p excp.
	 *
	 * @author Davidson Francis
	 */
	static inline vaddr_t or1k_excp_get_epcr(const struct exception *excp)
	{
		return (excp->epcr);
	}

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
	EXTERN void or1k_excp_set_handler(int num, or1k_exception_handler_fn handler);

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

#endif /* _ASM_FILE_ */

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
	#define __exception_struct      /**< exception               */
	#define __exception_get_addr    /**< exception_get_addr()    */
	#define __exception_get_instr   /**< exception_get_instr()   */
	#define __exception_get_num     /**< exception_get_num()     */
	#define __exception_set_handler /**< exception_set_handler() */
	/**@}*/

	/**
	 * @name Exception Codes
	 */
	/**@*/
	#define EXCP_INVALID_OPCODE      OR1K_EXCP_ILLEGAL_INSTRUCTION  /**< Invalid Opcode     */
	#define EXCP_PAGE_FAULT          OR1K_EXCP_PAGE_FAULT           /**< Page Fault         */
	#define EXCP_PAGE_PROTECTION     OR1K_EXCP_PAGE_FAULT           /**< Page Protection    */
	#define EXCP_DTLB_FAULT          OR1K_EXCP_DTLB_FAULT           /**< DTLB Fault         */
	#define EXCP_ITLB_FAULT          OR1K_EXCP_ITLB_FAULT           /**< ITLB Fault         */
	#define EXCP_GENERAL_PROTECTION  OR1K_EXCP_RESET                /**< General Protection */
	/**@}*/

	/**
	 * @brief Number of exceptions in the IBM PC target.
	 */
	#define HAL_NUM_EXCEPTIONS OR1K_NUM_EXCEPTIONS

#ifndef _ASM_FILE_

	/**
	 * @see or1k_excp_get_num().
	 */
	static inline int exception_get_num(const struct exception *excp)
	{
		return (or1k_excp_get_num(excp));
	}

	/**
	 * @see or1k_excp_get_addr().
	 */
	static inline int exception_get_addr(const struct exception *excp)
	{
		return (or1k_excp_get_addr(excp));
	}

	/**
	 * @see or1k_excp_get_epcr().
	 */
	static inline int exception_get_instr(const struct exception *excp)
	{
		return (or1k_excp_get_epcr(excp));
	}

	/**
	 * @see or1k_excp_set_handler().
	 */
	static inline void exception_set_handler(int num, or1k_exception_handler_fn handler)
	{
		or1k_excp_set_handler(num, handler);
	}

	/**
	 * @brief Forwards an exception.
	 *
	 * @param num  Target exception.
	 * @param excp Exception to be forwarded.
	 * @param ctx  Context information of the forwarded exception.
	 */
	EXTERN void forward_excp(int num, const struct exception *excp, const struct context *ctx);

#endif /* _ASM_FILE_ */

/**@endcond*/

#endif /* ARCH_CORE_OR1K_EXCP_H_ */

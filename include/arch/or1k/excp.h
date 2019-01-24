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

#ifndef ARCH_OR1K_EXCEPTION_H_
#define ARCH_OR1K_EXCEPTION_H_
/**
 * @addtogroup or1k-exception Exception
 * @ingroup or1k
 *
 * @brief Exceptions
 */
/**@{*/

	/**
	 * @brief Shadow registers support, not all architectures have,
	 * so must ensure that the underlying architecture have, to
	 * properly save the registers.
	 */
#define OR1K_HAVE_SHADOW_GPRS 

	/**
	 * @brief Temporary registers used while inside exception/interrupt
	 * handler.
	 */
#ifdef OR1K_HAVE_SHADOW_GPRS
	#define OR1K_EXCEPTION_STORE_SP        l.mtspr r0,  r1, OR1K_SPR_SHADOW_GPR(1)
	#define OR1K_EXCEPTION_LOAD_SP(reg)    l.mfspr reg, r0, OR1K_SPR_SHADOW_GPR(1)
	#define OR1K_EXCEPTION_STORE_GPR2      l.mtspr r0,  r2, OR1K_SPR_SHADOW_GPR(2)
	#define OR1K_EXCEPTION_LOAD_GPR2(reg)  l.mfspr reg, r0, OR1K_SPR_SHADOW_GPR(2)
	#define OR1K_EXCEPTION_STORE_GPR3      l.mtspr r0,  r3, OR1K_SPR_SHADOW_GPR(3)
	#define OR1K_EXCEPTION_LOAD_GPR3(reg)  l.mfspr reg, r0, OR1K_SPR_SHADOW_GPR(3)
	#define OR1K_EXCEPTION_STORE_GPR4      l.mtspr r0,  r4, OR1K_SPR_SHADOW_GPR(4)
	#define OR1K_EXCEPTION_LOAD_GPR4(reg)  l.mfspr reg, r0, OR1K_SPR_SHADOW_GPR(4)
	#define OR1K_EXCEPTION_STORE_GPR5      l.mtspr r0,  r5, OR1K_SPR_SHADOW_GPR(5)
	#define OR1K_EXCEPTION_LOAD_GPR5(reg)  l.mfspr reg, r0, OR1K_SPR_SHADOW_GPR(5)
	#define OR1K_EXCEPTION_STORE_GPR6      l.mtspr r0,  r6, OR1K_SPR_SHADOW_GPR(6)
	#define OR1K_EXCEPTION_LOAD_GPR6(reg)  l.mfspr reg, r0, OR1K_SPR_SHADOW_GPR(6)
#else
	#define OR1K_EXCEPTION_STORE_SP        l.sw 0x80(r0), r1
	#define OR1K_EXCEPTION_LOAD_SP(reg)    l.lwz reg, 0x80(r0)
	#define OR1K_EXCEPTION_STORE_GPR2      l.sw 0x64(r0), r2
	#define OR1K_EXCEPTION_LOAD_GPR2(reg)  l.lwz reg, 0x64(r0)
	#define OR1K_EXCEPTION_STORE_GPR3      l.sw  0x68(r0), r3
	#define OR1K_EXCEPTION_LOAD_GPR3(reg)  l.lwz reg, 0x68(r0)
	#define OR1K_EXCEPTION_STORE_GPR4      l.sw  0x6c(r0), r4
	#define OR1K_EXCEPTION_LOAD_GPR4(reg)  l.lwz reg, 0x6c(r0)
	#define OR1K_EXCEPTION_STORE_GPR5      l.sw  0x70(r0), r5
	#define OR1K_EXCEPTION_LOAD_GPR5(reg)  l.lwz reg, 0x70(r0)
	#define OR1K_EXCEPTION_STORE_GPR6      l.sw  0x74(r0), r6
	#define OR1K_EXCEPTION_LOAD_GPR6(reg)  l.lwz reg, 0x74(r0)
#endif

	/**
	 * @name Provided Interface
	 *
	 * @cond or1k
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
	#define OR1K_NUM_EXCEPTIONS 9

	/**
	 * @name or1k Exception Codes
	 */
	/**@{*/
	#define OR1K_EXCP_RESET                 0 /**@< Reset exception          */
	#define OR1K_EXCP_BUSERROR              1 /**@< Bus Error Exception      */
	#define OR1K_EXCP_PAGE_FAULT            2 /**@< Page fault Exception     */
	#define OR1K_EXCP_ALIGNMENT             3 /**@< Alignment Exception      */
	#define OR1K_EXCP_ILLEGAL_INSTRUCTION   4 /**@< Illegal Instruction      */
	#define OR1K_EXCP_TLB_FAULT             5 /**@< Invalid Opcode Exception */
	#define OR1K_EXCP_RANGE                 6 /**@< TLB Fault                */
	#define OR1K_EXCP_FLOAT_POINT           7 /**@< Floating Point Exception */
	#define OR1K_EXCP_TRAP                  8 /**@< Trap Exception           */
	/**@}*/

	/**
	 * @brief Exception Codes
	 *
	 * @cond or1k
	 */
	/**@*/
	#define EXCP_INVALID_OPCODE      OR1K_EXCP_ILLEGAL_INSTRUCTION  /**< Invalid Opcode     */
	#define EXCP_PAGE_FAULT          OR1K_EXCP_PAGE_FAULT           /**< Page Fault         */
	#define EXCP_PAGE_PROTECTION     OR1K_EXCP_PAGE_FAULT           /**< Page Protection    */
	#define EXCP_TLB_FAULT           OR1K_EXCP_TLB_FAULT            /**< TLB Fault          */
	#define EXCP_GENERAL_PROTECTION  OR1K_EXCP_RESET                /**< General Protection */
	/**@}*/
	/**@endcond*/

/**@endcond*/

#ifndef _ASM_FILE_

	#include <nanvix/const.h>
	#include <arch/or1k/context.h>
	#include <arch/or1k/mmu.h>
	#include <stdint.h>

	/**
	 * @brief Exception information.
	 *
	 * @cond or1k
	 */
	struct exception
	{
		uint32_t num;    /**< Exception number.      */
		uint32_t eear;   /**< Exception address.     */
		uint32_t epcr;   /**< Saved program counter. */
	} __attribute__((packed));
	/**@endcond*/

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
	 * @see or1k_excp_get_num().
	 *
	 * @cond or1k
	 */
	static inline int exception_get_num(const struct exception *excp)
	{
		return (or1k_excp_get_num(excp));
	}
	/**@endcond*/

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
	 * @see or1k_excp_get_addr().
	 *
	 * @cond or1k
	 */
	static inline int exception_get_addr(const struct exception *excp)
	{
		return (or1k_excp_get_addr(excp));
	}
	/**@endcond*/

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
	 * @see or1k_excp_get_epcr().
	 *
	 * @cond or1k
	 */
	static inline int exception_get_instr(const struct exception *excp)
	{
		return (or1k_excp_get_epcr(excp));
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
	EXTERN void or1k_excp_set_handler(int num, or1k_exception_handler_fn handler);

	/**
	 * @see or1k_excp_set_handler()
	 *
	 * @cond or1k
	 */
	static inline void exception_set_handler(int num, or1k_exception_handler_fn handler)
	{
		or1k_excp_set_handler(num, handler);
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

#endif /* _ASM_FILE_ */

/**@}*/

#endif /* ARCH_OR1K_EXCEPTION_H_ */

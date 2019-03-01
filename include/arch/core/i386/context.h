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

#ifndef ARCH_I386_CONTEXT_H_
#define ARCH_I386_CONTEXT_H_

/**
 * @addtogroup i386-core-context Context
 * @ingroup i386-core
 *
 * @brief Execution Context
 */

	/**
	 * @name Procided Interface
	 */
	/**@{*/
	#define __context_struct /**< Exection Context Structure */
	/**@}*/

	/**
	 * @brief Hardware-saved execution context size (in bytes).
	 */
	#define I386_CONTEXT_HW_SIZE 20

	/**
	 * @brief Software-saved execution context size (in bytes).
	 */
	#define I386_CONTEXT_SW_SIZE 44

	/**
	 * @brief Execution context size (in bytes).
	 */
	#define I386_CONTEXT_SIZE (I386_CONTEXT_HW_SIZE + I386_CONTEXT_SW_SIZE)

	/**
	 * @name Offsets to the Context Structure
	 */
	/**@{*/
	#define I386_CONTEXT_GS       0 /**< Extra Data Segment #3 Register        */
	#define I386_CONTEXT_FS       4 /**< Extra Data Segment #2 Register        */
	#define I386_CONTEXT_ES       8 /**< Extra Data Segment #1 Register        */
	#define I386_CONTEXT_DS      12 /**< Data Segment Register                 */
	#define I386_CONTEXT_EDI     16 /**< Extended Destination Index Register   */
	#define I386_CONTEXT_ESI     20 /**< Extended Source Index Register        */
	#define I386_CONTEXT_EBP     24 /**< Extended Stack base Pointer Register  */
	#define I386_CONTEXT_EDX     28 /**< Extended Accumulator #2 Register      */
	#define I386_CONTEXT_ECX     32 /**< Extended Counter Register             */
	#define I386_CONTEXT_EBX     36 /**< Extended Base Index Register          */
	#define I386_CONTEXT_EAX     40 /**< Exntended Accumulator #1 Register     */
	#define I386_CONTEXT_EIP     44 /**< Extended Instruction Pointer Register */
	#define I386_CONTEXT_CS      48 /**< Code Segment Register                 */
	#define I386_CONTEXT_EFLAGS  52 /**< Exended Flags Register                */
	#define I386_CONTEXT_ESP     56 /**< Extended Stack Pointer Register       */
	#define I386_CONTEXT_SS      60 /**< Stack Segment Register                */
	/**@}*/

#ifndef _ASM_FILE_

	#include <arch/core/i386/core.h>

	/**
	 * Saved execution context upon interrupts and exceptions.
	 */
	struct context
	{
        dword_t gs, fs, es, ds;                    /**< Segment Registers         */
        dword_t edi, esi, ebp, edx, ecx, ebx, eax; /**< General Purpose Registers */
        dword_t eip, cs, eflags, useresp, ss;      /**< Special Registers         */
	} __attribute__((packed));

#endif /* _ASM_FILE_ */

#endif /* ARCH_I386_CONTEXT_H_ */

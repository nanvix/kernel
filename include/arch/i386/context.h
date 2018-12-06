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
 * @addtogroup i386-context Context
 * @ingroup i386
 *
 * @brief Interface for manipulating the execution context.
 */

	/**
	 * @name Offsets to the Context Structure
	 */
	/**@{*/
	#define GS       4
	#define FS       8
	#define ES      12
	#define DS      16
	#define EDI     20
	#define ESI     24
	#define EBP     28
	#define EBX     32
	#define EDX     36
	#define ECX     40
	#define EAX     44
	#define EIP     48
	#define CS      52
	#define EFLAGS  56
	#define USERESP 60
	#define SS      64
	/**@}*/

#ifndef _ASM_FILE_

	#include <arch/i386/core.h>

	/**
	 * @brief Saved context during interrupt/exception switch.
	 */
	struct context
	{   
		dword_t old_kesp;
        dword_t gs, fs, es, ds;
        dword_t edi, esi, ebp, ebx, edx, ecx, eax;
        dword_t eip, cs, eflags, useresp, ss;	
	};

#endif /* _ASM_FILE */

#endif /* ARCH_I386_H_ */

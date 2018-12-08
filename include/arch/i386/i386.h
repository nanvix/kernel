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

#ifndef ARCH_I386_H_
#define ARCH_I386_H_

	#ifndef TARGET_IBM_PC_H_
	#error "include <target/ibm/pc.h> instead"
	#endif

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_disable_interrupts
	#define __hal_enable_interrupts
	#define __hal_processor_get_num_cores
	/**@}*/

	/**
	 * @defgroup i386 x86 Architecture
	 * @ingroup i386-pc
	 */
	#include <arch/i386/8253.h>
	#include <arch/i386/8259.h>
	#include <arch/i386/cache.h>
	#include <arch/i386/core.h>
	#include <arch/i386/int.h>
	#include <arch/i386/paging.h>

#ifndef _ASM_FILE_

	/**
	 * Returns the number of cores of the underlying processor.
	 */
	static inline int hal_processor_get_num_cores(void)
	{
		return (1);
	}

	/**
	 * @see cli()
	 */
	static inline void hal_disable_interrupts(void)
	{
		cli();
	}

	/**
	 * @see sti()
	 */
	static inline void hal_enable_interrupts(void)
	{
		sti();
	}

#endif /* _ASM_FILE_ */

#endif /* ARCH_I386_H_ */

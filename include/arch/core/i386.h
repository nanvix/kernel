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

#ifndef CORE_I386_H_
#define CORE_I386_H_

	/**
	 * @addtogroup i386-core i386 Core
	 * @ingroup cores
	 */

	#ifndef __NEED_CORE_I386
		#error "i386 core not required"
	#endif

	#include <arch/core/i386/8253.h>
	#include <arch/core/i386/8259.h>
	#include <arch/core/i386/cache.h>
	#include <arch/core/i386/core.h>
	#include <arch/core/i386/excp.h>
	#include <arch/core/i386/int.h>
	#include <arch/core/i386/mmu.h>
	#include <arch/core/i386/pmio.h>
	#include <arch/core/i386/tlb.h>

/**
 * @cond i386
 */

	/* Feature Declaration */
	#define CORE_SUPPORTS_PMIO 1

/**@}*/

#endif /* CORE_I386_H_ */


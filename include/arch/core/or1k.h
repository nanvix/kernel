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

#ifndef CORE_OR1K_H_
#define CORE_OR1K_H_

	/**
	 * @addtogroup or1k-core OpenRISC Core
	 * @ingroup cores
	 */

	#ifndef __NEED_CORE_OR1K
		#error "or1k core not required"
	#endif

	#include <arch/core/or1k/cache.h>
	#include <arch/core/or1k/clock.h>
	#include <arch/core/or1k/core.h>
	#include <arch/core/or1k/excp.h>
	#include <arch/core/or1k/int.h>
	#include <arch/core/or1k/io.h>
	#include <arch/core/or1k/mmu.h>
	#include <arch/core/or1k/ompic.h>
	#include <arch/core/or1k/pic.h>
	#include <arch/core/or1k/spinlock.h>
	#include <arch/core/or1k/tlb.h>
	#include <arch/core/or1k/types.h>

#endif /* CORE_OR1K_H_ */


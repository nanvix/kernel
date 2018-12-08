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

#ifndef NANVIX_HAL_TARGET_H_
#define NANVIX_HAL_TARGET_H_

	#if defined(__mppa256__)
	#include <target/kalray/mppa256.h>
	#endif

	#if (defined(__i386__) && (__pc__))
	#include <target/ibm/pc.h>
	#endif

	#ifndef _HAL_NUM_CORES
	#error "_HAL_NUM_CORES not defined"
	#endif

	#ifndef __hal_cpu_get_num_cores
	#error "hal_cpu_get_num_cores() not defined?"
	#endif

	#ifndef HAL_INT_CLOCK
	#error "HAL_INT_CLOCK not defined"
	#endif

	#ifndef __hal_clock_init
	#error "hal_clock_init() not defined?"
	#endif

	#ifndef __hal_core_get_id
	#error "hal_core_get_id() not defined?"
	#endif

	#ifndef __hal_core_halt
	#error "hal_core_halt() not defined?"
	#endif

	#ifndef __hal_core_setup
	#error "hal_core_setup() not defined?"
	#endif

	#ifndef __hal_stdout_init
	#error "hal_stdout_init() not defined?"
	#endif

	#ifndef __hal_stdout_write
	#error "hal_stdout_write() not defined?"
	#endif

	#ifndef _HAL_INT_NR
	#error "_HAL_INT_NR not defined"
	#endif

	#ifndef __hal_disable_interrupts
	#error "hal_disable_interrupts() not defined?"
	#endif

	#ifndef __hal_enable_interrupts
	#error "hal_enable_interrupts() not defined?"
	#endif

#ifdef XXX
	#ifndef __hal_interrupt_ack
	#error "hal_interrupt_ack() not defined?"
	#endif
#endif

	#ifndef __hal_interrupt_set_handler
	#error "hal_interrupt_set_handler() not defined?"
	#endif

	#ifndef __hal_intlvl_raise
	#error "hal_intlvl_raise() not defined?"
	#endif

	#ifndef __hal_intlvl_drop
	#error "hal_intlvl_hal_drop() not defined?"
	#endif

	#ifndef __hal_outputb
	#error "hal_outputb() not defined?"
	#endif

	#ifndef __hal_iowait
	#error "hal_iowait() not defined?"
	#endif

	#ifndef __hal_dcache_invalidate
	#error "hal_dcache_invalidate() not defined?"
	#endif

#endif /* NANVIX_HAL_TARGET_H_ */

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

#ifndef ARCH_K1B_IVT_H_
#define ARCH_K1B_IVT_H_

	#include <nanvix/const.h>
	#include <vbsp.h>
	#include <mOS_vcore_u.h>

	/**
	 * @brief Number of hardware interrupts.
	 *
	 * Number of hardware interrupt entries in the Interrupt Vector
	 * Table (IVT).
	 */
	#define K1B_NUM_HWINT 24

	/**
	 * @brief Number of software interrupts.
	 *
	 * Number of software interrupt entries in the Interrupt vector
	 * table (IVT).
	 */
	#define K1B_NUM_SWINT 1

	/**
	 * @brief Hardware interrupt ID.
	 */
	typedef bsp_ev_src_e k1b_hwint_id_t;

	/**
	 * @brief Hardware Interrupt handler.
	 */
	typedef it_handler_t k1b_hwint_handler_fn;

	/**
	 * @brief Software interrupt handler.
	 */
	typedef mOS_exception_handler_t k1b_swint_handler_fn;

	/**
	 * @brief Hardware interrupt numbers.
	 */
	EXTERN const k1b_hwint_id_t hwints[K1B_NUM_HWINT];

	/**
	 * @brief Initializes the interrupt vector table.
	 *
	 * @param do_hwint Default hardware interrupt handler.
	 * @param do_swint Default software interrupt handler.
	 */
	EXTERN void k1b_ivt_setup(k1b_hwint_handler_fn do_hwint, k1b_swint_handler_fn do_swint);

#endif /* ARCH_K1B_IVT_H_ */

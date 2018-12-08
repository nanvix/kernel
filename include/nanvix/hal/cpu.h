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

#ifndef NANVIX_HAL_CPU_H_
#define NANVIX_HAL_CPU_H_

/**
 * @addtogroup kernel-hal-processor Processor
 * @ingroup kernel-hal
 *
 * @brief Processor
 */
/**@{*/

	#include <nanvix/const.h>
	#include <nanvix/hal/target.h>

	#ifndef _HAL_NUM_CORES
	#error "_HAL_NUM_CORES not defined"
	#endif

	#ifndef __hal_core_get_id
	#error "hal_core_get_id() not defined?"
	#endif

	#ifndef __hal_processor_get_num_cores
	#error "hal_processor_get_num_cores() not defined?"
	#endif

	#ifndef __hal_processor_halt
	#error "hal_processor_halt() not defined?"
	#endif

	#ifndef __hal_processor_setup
	#error "hal_processor_setup() not defined?"
	#endif

	/**
	 * @brief Number of cores.
	 */
	#define HAL_NUM_CORES _HAL_NUM_CORES

	/**
	 * @brief Initializes the processor.
	 */
	EXTERN void hal_processor_setup(void);

	/**
	 * @brief Halts the processor.
	 */
	EXTERN void hal_processor_halt(void);

	/**
	 * @brief Gets the number of cores.
	 *
	 * @returns The number of cores of the underlying processor.
	 */
	EXTERN int hal_processor_get_num_cores(void);

	/**
	 * @brief Gets the ID of the underlying core.
	 *
	 * @returns The ID of the underlying core.
	 */
	EXTERN int hal_core_get_id(void);

/**@}*/

#endif /* NANVIX_HAL_CPU_H_ */

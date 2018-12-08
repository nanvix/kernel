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

#ifndef NANVIX_HAL_CORE_H_
#define NANVIX_HAL_CORE_H_

/**
 * @addtogroup kernel-hal-core Core
 * @ingroup kernel-hal-cpu
 *
 * @brief Core Interface
 */
/**@{*/

	#include <nanvix/const.h>
	#include <nanvix/hal/target.h>

	/**
	 * @brief Initializes the processor.
	 */
	EXTERN void hal_core_setup(void);

	/**
	 * @brief Halts the processor.
	 */
	EXTERN void hal_core_halt(void);

	/**
	 * @brief Gets the ID of the underlying core.
	 *
	 * @returns The ID of the underlying core.
	 */
	EXTERN int hal_core_get_id(void);

/**@}*/

#endif /* NANVIX_HAL_CORE_H_ */


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
	 * @brief Gets the ID of the underlying core.
	 *
	 * @returns The ID of the underlying core.
	 */
	EXTERN int core_get_id(void);

	/**
	 * @brief Halts instruction execution in the underlying core.
	 */
	EXTERN void core_halt(void);

	/**
	 * @brief Shutdowns the underlying core.
	 *
	 * @param status Shutdown status.
	 */
	EXTERN void core_shutdown(int status);

#ifdef HAL_SMP

	/**
	 * @brief Suspends instruction execution in the underling core.
	 */
	EXTERN void core_sleep(void);

	/**
	 * @brief Wakes up a core.
	 *
	 * @param coreid ID of the target core.
	 */
	EXTERN void core_wakeup(int coreid);

	/**
	 * @brief Starts a core.
	 *
	 * @param coreid ID of the target core.
	 * @param start  Starting routine to execute.
	 */
	EXTERN void core_start(int coreid, void (*start)(void));

	/**
	 * @brief Reset the underlying core.
	 */
	EXTERN void core_reset(void);

#endif /* HAL_SMP */

/**@}*/

#endif /* NANVIX_HAL_CORE_H_ */


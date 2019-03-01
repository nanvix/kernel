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

	#ifndef __NEED_HAL_CORE
		#error "include <nanvix/hal/cluster.h> instead"
	#endif

	/* Core Interface Implementation */
	#include <nanvix/hal/core/_core.h>

	#include <nanvix/hal/core/cache.h>
	#include <nanvix/hal/core/clock.h>
	#include <nanvix/hal/core/context.h>
	#include <nanvix/hal/core/exception.h>
	#include <nanvix/hal/core/interrupt.h>
	#include <nanvix/hal/core/mmu.h>
	#include <nanvix/hal/core/spinlock.h>
	#include <nanvix/const.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Functions */
	#ifndef __core_get_id
	#error "core_get_id() not defined?"
	#endif
	#ifndef __core_shutdown
	#error "core_shutdown() not defined?"
	#endif
	#ifndef __core_sleep
	#error "core_sleep() not defined?"
	#endif
	#ifndef __core_start
	#error "core_start() not defined?"
	#endif
	#ifndef __core_wakeup
	#error "core_wakeup() not defined?"
	#endif
	#ifndef __core_reset
	#error "core_reset() not defined?"
	#endif

/*============================================================================*
 * Core Interface                                                             *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-core Core
 * @ingroup kernel-hal
 *
 * @brief Core HAL Interface
 */
/**@{*/

	/**
	 * @brief Gets the ID of the underlying core.
	 *
	 * @returns The ID of the underlying core.
	 */
	EXTERN int core_get_id(void);

	/**
	 * @brief Shutdowns the underlying core.
	 *
	 * @param status Shutdown status.
	 */
	EXTERN void core_shutdown(int status);

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

/**@}*/

#endif /* NANVIX_HAL_CORE_H_ */


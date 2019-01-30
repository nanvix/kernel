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

#ifndef ARCH_K1B_CORE_H_
#define ARCH_K1B_CORE_H_

/**
 * @addtogroup k1b-core Core
 * @ingroup k1b
 *
 * @brief k1b Core
 */
/**@{*/

	/* External dependencies. */
	#include <nanvix/const.h>
	#include <mOS_vcore_u.h>

/*============================================================================*
 *                               Core Interface                               *
 *============================================================================*/

	/**
	 * @name States of a Core
	 */
	/**@{*/
	#define K1B_CORE_SLEEPING 0 /**< Sleeping    */
	#define K1B_CORE_RUNNING  1 /**< Running     */
	#define K1B_CORE_OFFLINE  2 /**< Powered Off */
	/**@}*/

	/**
	 * @brief Gets the ID of the core.
	 *
	 * The k1b_core_get_id() returns the ID of the underlying core.
	 *
	 * @returns The ID of the underlying core.
	 */
	static inline int k1b_core_get_id(void)
	{
		return (__k1_get_cpu_id());
	}

	/**
	 * @brief Puts the underlyig core in idle mode 1.
	 *
	 * The k1b_await() function puts the processor in idle mode 1. In
	 * this mode, instruction execution is suspended until an
	 * interrupt is triggered, be it eligible or not. Events that are
	 * not mapped on interrupts and are triggered during the idle
	 * period do not wakeup cores in compute clusters.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline void k1b_await(void)
	{
		mOS_idle1();
	}

	/**
	 * @brief Initializes the underlying core.
	 */
	EXTERN void k1b_core_setup(void);

	/**
	 * @brief Resumes instruction execution in the underlying core.
	 */
	EXTERN void k1b_core_run(void);

	/**
	 * @brief Starts a core.
	 *
	 * @param coreid ID of the target core.
	 * @param start  Starting routine to execute.
	 */
	EXTERN void k1b_core_start(int coreid, void (*start)(void));

	/**
	 * @brief Wakes up a core.
	 *
	 * @param coreid ID of the target core.
	 */
	EXTERN void k1b_core_wakeup(int coreid);

	/**
	 * @brief Suspends instruction execution in the underling core.
	 */
	EXTERN void k1b_core_sleep(void);

	/**
	 * @brief Shutdowns the underlying core.
	 *
	 * @param status Shutdown status.
	 */
	EXTERN void k1b_core_shutdown(int status);

	/**
	 * @brief Resets the underlying core.
	 *
	 * The k1b_core_reset() function resets execution instruction in
	 * the underlying core by reseting the kernel stack to its initial
	 * location and relaunching the k1b_slave_setup() function.
	 *
	 * @note This function does not return.
	 * @note For the implementation of this function check out
	 * assembly source files.
	 *
	 * @see k1b_slave_setup()
	 *
	 * @author Pedro Henrique Penna
	 */
	EXTERN void k1b_core_reset(void);

/*============================================================================*
 *                              Exported Interface                            *
 *============================================================================*/

	/**
	 * @name Provided Interface
	 *
	 * @cond k1b
	 */
	/**@{*/
	#define __core_get_id   /**< core_get_id()   */
	#define __core_halt     /**< core_halt()     */
	#define __core_shutdown /**< core_shutdown() */
	#define __core_sleep    /**< core_sleep()    */
	#define __core_wakeup   /**< core_wakeup()   */
	#define __core_start    /**< core_start()    */
	/**@}*/
	/**@endcond*/

	/**
	 * @see k1b_core_get_id()
	 *
	 * @cond k1b
	 */
	static inline int core_get_id(void)
	{
		return (k1b_core_get_id());
	}
	/*@endcond*/

	/**
	 * @see k1b_await()
	 *
	 * @cond k1b
	 */
	static inline void core_halt(void)
	{
		k1b_await();
	}
	/*@endcond*/

	/**
	 * @see k1b_core_sleep().
	 *
	 * @cond k1b
	 */
	static inline void core_sleep(void)
	{
		k1b_core_sleep();
	}
	/**@endcond*/

	/**
	 * @see k1b_core_wakeup().
	 *
	 * @cond k1b
	 */
	static inline void core_wakeup(int coreid)
	{
		k1b_core_wakeup(coreid);
	}
	/**@endcond*/

	/**
	 * @see k1b_core_start().
	 *
	 * @cond k1b
	 */
	static inline void core_start(int coreid, void (*start)(void))
	{
		k1b_core_start(coreid, start);
	}
	/**@endcond*/

	/**
	 * @see k1b_core_shutdown().
	 *
	 * @cond k1b
	 */
	static inline void shutdown(int status)
	{
		k1b_core_shutdown(status);
	}
	/**@endcond*/

	/**
	 * @see k1b_core_reset().
	 *
	 * @cond k1b
	 */
	static inline void core_reset(void)
	{
		k1b_core_reset();
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_K1B_CORE_H_ */

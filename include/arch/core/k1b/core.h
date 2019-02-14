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

#ifndef ARCH_CORE_K1B_CORE_H_
#define ARCH_CORE_K1B_CORE_H_

/**
 * @addtogroup k1b-core
 */
/**@{*/

	#include <mOS_vcore_u.h>

	/**
	 * @name States of a Core
	 */
	/**@{*/
	#define K1B_CORE_IDLE      0 /**< Idle        */
	#define K1B_CORE_SLEEPING  1 /**< Sleeping    */
	#define K1B_CORE_RUNNING   2 /**< Running     */
	#define K1B_CORE_RESETTING 3 /**< Resetting   */
	#define K1B_CORE_OFFLINE   4 /**< Powered Off */
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
	 * @brief Initializes the underlying core.
	 */
	extern void k1b_core_setup(void);

	/**
	 * @brief Resumes instruction execution in the underlying core.
	 */
	extern void k1b_core_run(void);

	/**
	 * @brief Starts a core.
	 *
	 * @param coreid ID of the target core.
	 * @param start  Starting routine to execute.
	 */
	extern void k1b_core_start(int coreid, void (*start)(void));

	/**
	 * @brief Wakes up a core.
	 *
	 * @param coreid ID of the target core.
	 */
	extern void k1b_core_wakeup(int coreid);

	/**
	 * @brief Suspends instruction execution in the underlying core.
	 */
	extern void k1b_core_sleep(void);

	/**
	 * @brief Suspends instruction execution in the underlying core.
	 */
	extern void k1b_core_idle(void);

	/**
	 * @brief Shutdowns the underlying core.
	 *
	 * @param status Shutdown status.
	 */
	extern void k1b_core_shutdown(int status);

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
	extern void k1b_core_reset(void);

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond k1b
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __core_get_id   /**< core_get_id()   */
	#define __core_shutdown /**< core_shutdown() */
	#define __core_sleep    /**< core_sleep()    */
	#define __core_wakeup   /**< core_wakeup()   */
	#define __core_start    /**< core_start()    */
	#define __core_reset    /**< core_start()    */
	/**@}*/

	/**
	 * @see k1b_core_get_id().
	 */
	static inline int core_get_id(void)
	{
		return (k1b_core_get_id());
	}

	/**
	 * @see k1b_core_sleep().
	 */
	static inline void core_sleep(void)
	{
		k1b_core_sleep();
	}

	/**
	 * @see k1b_core_wakeup().
	 */
	static inline void core_wakeup(int coreid)
	{
		k1b_core_wakeup(coreid);
	}

	/**
	 * @see k1b_core_start().
	 */
	static inline void core_start(int coreid, void (*start)(void))
	{
		k1b_core_start(coreid, start);
	}

	/**
	 * @see k1b_core_shutdown().
	 */
	static inline void core_shutdown(int status)
	{
		k1b_core_shutdown(status);
	}

	/**
	 * @see k1b_core_reset().
	 */
	static inline void core_reset(void)
	{
		k1b_core_reset();
	}

/**@endcond*/

#endif /* ARCH_CORE_K1B_CORE_H_ */

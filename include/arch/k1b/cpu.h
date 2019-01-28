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

#ifndef ARCH_K1B_CPU_H_
#define ARCH_K1B_CPU_H_

/**
 * @addtogroup k1b-cpu CPU
 * @ingroup k1b
 *
 * @brief k1b Processor
 */
/**@{*/

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_cpu_get_num_cores
	#define __spinlock_t          /**< @p spinlock_t      */
	#define __spinlock_init_fn    /**< spinlock_init()    */
	#define __spinlock_lock_fn    /**< spinlock_lock()    */
	#define __spinlock_trylock_fn /**< spinlock_trylock() */
	#define __spinlock_unlock_fn  /**< spinlock_unlock()  */
	#define __cpu_wait_fn         /**< cpu_wait()         */
	#define __cpu_notify_fn       /**< cpu_notify()       */
	/**@}*/

	#include <HAL/hal/hal_ext.h>
	#include <arch/k1b/core.h>
	#include <arch/k1b/cache.h>
	#include <nanvix/const.h>
	#include <stdint.h>

	/**
	 * @brief Number of cores in the k1b processor.
	 */
	#ifdef __k1io__
		#define K1B_NUM_CORES 4
	#else
		#define K1B_NUM_CORES 16
	#endif

	/**
	 * @brief Gets the number of cores.
	 *
	 * The k1b_cpu_get_num_cores() gets the number of cores in the
	 * underlying k1b processor.
	 *
	 * @returns The the number of cores in the underlying processor.
	 */
	static inline int k1b_cpu_get_num_cores(void)
	{
		return (K1B_NUM_CORES);
	}

	/**
	 * @see k1b_cpu_get_num_cores()
	 *
	 * @cond k1b
	 */
	static inline int hal_cpu_get_num_cores(void)
	{
		return (k1b_cpu_get_num_cores());
	}
	/**@endcond*/

	/**
	 * @brief Gets the number of cores.
	 *
	 * @returns The number of cores in the underlying processor.
	 */
	EXTERN int hal_cpu_get_num_cores(void);

/*============================================================================*
 *                        Inter-Processor Interrupts                          *
 *============================================================================*/

	/**
	 * @brief Waits for an inter-processor interrupt.
	 *
	 * The k1b_cpu_wait() function puts the underlying core in
	 * low-power consumption mode and waits for a inter-processor
	 * interrupt (IPI) to be triggered.
	 *
	 * @bug For some unknown reason, we have to flush the cache here.
	 */
	static inline void k1b_cpu_wait(void)
	{
		k1b_await();
		k1b_dcache_inval();
		mOS_it_disable_num(MOS_VC_IT_USER_0);
		mOS_it_clear_num(MOS_VC_IT_USER_0);
		mOS_it_enable_num(MOS_VC_IT_USER_0);
	}

	/**
	 * @see k1b_cpu_wait()
	 *
	 * @cond k1b
	 */
	static inline void cpu_wait(void)
	{
		k1b_cpu_wait();
	}
	/**@endcond*/

	/**
	 * @brief Sends an inter-processor interrupt.
	 *
	 * The k1b_cpu_notify() function sends an inter-processor
	 * interrupt (IPI) to the core whose ID equals to @p coreid.
	 *
	 * @param coreid ID of the target core.
	 *
	 * @todo Check whether or not @p coreid is valid.
	 * @bug No sanity check is performed in @p coreid.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline void k1b_cpu_notify(int coreid)
	{
		mOS_pe_notify(1 << coreid, 0, 1, 0);
	}

	/**
	 * @see k1b_cpu_notify()
	 *
	 * @cond k1b
	 */
	static inline void cpu_notify(int coreid)
	{
		k1b_cpu_notify(coreid);
	}
	/**@endcond*/

/*============================================================================*
 *                                 Spinlocks                                  *
 *============================================================================*/

	/**
	 * @name Spinlock State
	 */
	/**@{*/
	#define K1B_SPINLOCK_UNLOCKED 0 /**< Unlocked */
	#define K1B_SPINLOCK_LOCKED   1 /**< Locked   */
	/**@}*/

	/**
	 * @brief Spinlock.
	 */
	typedef uint64_t spinlock_t;

	/**
	 * @brief Initializes a spinlock.
	 *
	 * @param lock Target spinlock.
	 */
	static inline void k1b_spinlock_init(spinlock_t *lock)
	{
		__builtin_k1_sdu(lock,  K1B_SPINLOCK_UNLOCKED);
	}

	/**
	 * @see k1b_spinlock_init()
	 *
	 * @cond k1b
	 */
	static inline void spinlock_init(spinlock_t *lock)
	{
		k1b_spinlock_init(lock);
	}
	/**@endcond*/

	/**
	 * @brief Attempts to lock a spinlock.
	 *
	 * @param lock Target spinlock.
	 *
	 * @returns Upon successful completion, the spinlock pointed to by
	 * @p lock is locked and zero is returned. Upon failure, non-zero
	 * is returned instead, and the lock is not acquired by the
	 * caller.
	 */
	static inline int k1b_spinlock_trylock(spinlock_t *lock)
	{
		return (__builtin_k1_ldc(lock) == K1B_SPINLOCK_UNLOCKED);
	}

	/**
	 * @see k1b_spinlock_trylock()
	 *
	 * @cond k1b
	 */
	static inline int spinlock_trylock(spinlock_t *lock)
	{
		return (k1b_spinlock_trylock(lock));
	}
	/**@endcond*/

	/**
	 * @brief Locks a spinlock.
	 *
	 * @param lock Target spinlock.
	 */
	static inline void k1b_spinlock_lock(spinlock_t *lock)
	{
		while (!k1b_spinlock_trylock(lock))
			/* noop */;
	}

	/**
	 * @see k1b_spinlock_lock()
	 *
	 * @cond k1b
	 */
	static inline void spinlock_lock(spinlock_t *lock)
	{
		k1b_spinlock_lock(lock);
	}
	/**@endcond*/

	/**
	 * @brief Unlocks a spinlock.
	 *
	 * @param lock Target spinlock.
	 */
	static inline void k1b_spinlock_unlock(spinlock_t *lock)
	{
		__builtin_k1_sdu(lock, K1B_SPINLOCK_UNLOCKED);
	}

	/**
	 * @see k1b_spinlock_unlock()
	 *
	 * @cond k1b
	 */
	static inline void spinlock_unlock(spinlock_t *lock)
	{
		k1b_spinlock_unlock(lock);
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_K1B_CPU_H_ */

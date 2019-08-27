/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

/**
 * @addtogroup nanvix Nanvix System
 */
/**@{*/

#ifndef NANVIX_H_
#define NANVIX_H_

	#include <nanvix/syscall.h>
	#include <nanvix/config.h>
	#include <sys/types.h>
	#include <stdbool.h>
	#include <stdint.h>

/*============================================================================*
 * Kernel Threads                                                             *
 *============================================================================*/

	/**
	 * @brief Thread ID.
	 */
	typedef int kthread_t;

	/* System calls. */
	extern ssize_t nanvix_write(int, const char *, size_t);

	/**
	 * @name Thread Management Kernel Calls
	 */
	/**@{*/
	extern kthread_t kthread_self(void);
	extern int kthread_create(kthread_t *, void *(*)(void*), void *);
	extern int kthread_exit(void *);
	extern int kthread_join(kthread_t, void **);
	/**@}*/

	/**
	 * @name Thread Synchronization Kernel Calls
	 */
	/**@{*/
	extern int ksleep(void);
	extern int kwakeup(kthread_t);
	/**@}*/

	/**
	 * @brief Shutdowns the kernel.
	 *
	 * @returns Upon successful completion, this function does not
	 * return.Upon failure, a negative error code is returned instead.
	 */
	extern int kshutdown(void);

/*============================================================================*
 * Performance Monitoring                                                     *
 *============================================================================*/

	/**
	 * @brief Queries a performance event.
	 *
	 * @param event Target event.
	 *
	 * @returns Upon successful completion, either zero or non zero is
	 * returned, indicating whether or not the queried performance event
	 * may be monitored in the underlying core. Upon failure, a negative
	 * error code is returned instead.
	 */
	extern int nanvix_perf_query(int event);

	/**
	 * @brief Starts a performance monitor.
	 *
	 * @param perf  Target performance monitor.
	 * @param event Target event to watch.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	extern int nanvix_perf_start(int perf, int event);

	/**
	 * @brief Stops a performance monitor.
	 *
	 * @param perf Target performance monitor.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	extern int nanvix_perf_stop(int perf);

	/**
	 * @brief Restarts a performance monitor.
	 *
	 * @param perf Target performance monitor.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	extern int nanvix_perf_restart(int perf);

	/**
	 * @brief Reads a PM register.
	 *
	 * @param perf Target performance monitor.
	 *
	 * @returns Upon successful completion, the value of the target
	 * performance monitor. Upon failure, a negative error code,
	 * converted to uint64_t is returned instead.
	 */
	extern uint64_t nanvix_perf_read(int perf);

/*============================================================================*
 * Sync Service                                                               *
 *============================================================================*/

	/**
	 * @name Sync Kernel Calls
	 */
	/**@{*/
	extern int ksync_create(const int *, int, int);
	extern int ksync_open(const int *, int, int);
	extern int ksync_wait(int);
	extern int ksync_signal(int);
	extern int ksync_close(int);
	extern int ksync_unlink(int);
	/**@}*/

/*============================================================================*
 * Mailbox Service                                                            *
 *============================================================================*/

	/**
	 * @name Mailbox Kernel Calls
	 */
	/**@{*/
	extern int kmailbox_create(int);
	extern int kmailbox_open(int);
	extern int kmailbox_unlink(int);
	extern int kmailbox_close(int);
	extern int kmailbox_awrite(int, const void *, size_t);
	extern int kmailbox_aread(int, void *, size_t);
	extern int kmailbox_wait(int);
	/**@}*/

/*============================================================================*
 * Portal Service                                                             *
 *============================================================================*/

	/**
	 * @name Portal Kernel Calls
	 */
	/**@{*/
	extern int kportal_create(int);
	extern int kportal_allow(int, int);
	extern int kportal_open(int, int);
	extern int kportal_unlink(int);
	extern int kportal_close(int);
	extern int kportal_awrite(int, const void *, size_t);
	extern int kportal_aread(int, void *, size_t);
	extern int kportal_wait(int);
	/**@}*/

/*============================================================================*
 * Signal                                                                     *
 *============================================================================*/

	/**
	 * @brief Controls the behavior of a signal.
	 *
	 * @param signum Signal ID.
	 * @param sigact Behavior descriptor.
	 *
	 * @returns Zero if successfully changes the behavior, non zero otherwise.
	 */
	extern int ksigctl(int signum, struct ksigaction *sigact);

	/**
	 * @brief Schedules an alarm signal.
	 *
	 * @param seconds Time in seconds.
	 *
	 * @returns Zero if successfully register the alarm, non zero otherwise.
	 */
	extern int kalarm(int seconds);

	/**
	 * @brief Sends a signal.
	 *
	 * @param signum Signal ID.
	 * @param tid    Thread ID.
	 *
	 * @returns Zero if successfully sends the signal, non zero otherwise.
	 */
	extern int ksigsend(int signum, int tid);

	/**
	 * @brief Waits for the receipt of a signal.
	 *
	 * @param signum Signal ID.
	 *
	 * @returns Zero if successfully receives the signal, non zero otherwise.
	 */
	extern int ksigwait(int signum);

	/**
	 * @brief Returns from a signal handler.
	 */
	extern int ksigreturn(void);

/*============================================================================*
 * Fast Mutex                                                                 *
 *============================================================================*/

#if (CORES_NUM > 1)

	/**
	 * @brief Fast mutex.
	 */
	struct nanvix_fmutex
	{
		spinlock_t lock;
	};

	/**
	 * @brief Initializes a fast mutex.
	 *
	 * @param m Target fmutex.
	 *
	 * @param Upon sucessful completion, zero is returned. Upon failure, a
	 * negative error code is returned instead.
	 */
	static inline int nanvix_fmutex_init(struct nanvix_fmutex *m)
	{
		/* Invalid mutex. */
		if (m == NULL)
			return (-EINVAL);

		spinlock_init(&m->lock);

		return (0);
	}

	/**
	 * @brief Locks a fast mutex.
	 *
	 * @param m Target fmutex.
	 *
	 * @param Upon sucessful completion, zero is returned. Upon failure, a
	 * negative error code is returned instead.
	 */
	static inline int nanvix_fmutex_lock(struct nanvix_fmutex *m)
	{
		/* Invalid mutex. */
		if (UNLIKELY(m == NULL))
			return (-EINVAL);

		spinlock_lock(&m->lock);

		return (0);
	}

	/**
	 * @brief Unlocks a fast mutex.
	 *
	 * @param m Target fmutex.
	 *
	 * @param Upon sucessful completion, zero is returned. Upon failure, a
	 * negative error code is returned instead.
	 */
	static inline int nanvix_fmutex_unlock(struct nanvix_fmutex *m)
	{
		/* Invalid mutex. */
		if (UNLIKELY(m == NULL))
			return (-EINVAL);

		spinlock_unlock(&m->lock);

		return (0);
	}

#endif /* CORES_NUM > 1 */


/*============================================================================*
 * Mutex                                                                      *
 *============================================================================*/

#if (CORES_NUM > 1)

	/**
	 * @brief Mutex.
	 */
	struct nanvix_mutex
	{
		bool locked;                /**< Locked?           */
		spinlock_t lock;            /**< Lock.             */

		#if (__NANVIX_MUTEX_SLEEP)

			kthread_t tids[THREAD_MAX]; /**< Sleeping threads. */

		#endif /* __NANVIX_MUTEX_SLEEP */
	};

	/**
	 * @brief Initializes a mutex.
	 *
	 * @param m Target mutex.
	 *
	 * @param Upon sucessful completion, zero is returned. Upon failure, a
	 * negative error code is returned instead.
	 */
	extern int nanvix_mutex_init(struct nanvix_mutex *m);

	/**
	 * @brief Locks a mutex.
	 *
	 * @param m Target mutex.
	 *
	 * @param Upon sucessful completion, zero is returned. Upon failure, a
	 * negative error code is returned instead.
	 */
	extern int nanvix_mutex_lock(struct nanvix_mutex *m);

	/**
	 * @brief unlocks a mutex.
	 *
	 * @param m Target mutex.
	 *
	 * @param Upon sucessful completion, zero is returned. Upon failure, a
	 * negative error code is returned instead.
	 */
	extern int nanvix_mutex_unlock(struct nanvix_mutex *m);

#endif /* CORES_NUM > 1 */

/*============================================================================*
 * Semaphore                                                                  *
 *============================================================================*/

	#if (CORES_NUM > 1)

		/**
		 * @brief Semaphore.
		 */
		struct nanvix_semaphore
		{
			int val;                    /**< Semaphore value.  */
			spinlock_t lock;            /**< Lock.             */

			#if (__NANVIX_SEMAPHORE_SLEEP)

				kthread_t tids[THREAD_MAX]; /**< Sleeping threads. */

			#endif /* __NANVIX_SEMAPHORE_SLEEP */
		};

		/**
		 * @brief Initializes a semaphore.
		 *
		 * @param m Target semaphore.
		 *
		 * @param Upon sucessful completion, zero is returned. Upon failure, a
		 * negative error code is returned instead.
		 */
		extern int nanvix_semaphore_init(struct nanvix_semaphore *sem, int val);

		/**
		 * @brief Performs a down operation on a semaphore.
		 *
		 * @param m Target semaphore.
		 *
		 * @param Upon sucessful completion, zero is returned. Upon failure, a
		 * negative error code is returned instead.
		 */
		extern int nanvix_semaphore_down(struct nanvix_semaphore *sem);

		/**
		 * @brief Performs an up operation on a semaphore.
		 *
		 * @param m Target semaphore.
		 *
		 * @param Upon sucessful completion, zero is returned. Upon failure, a
		 * negative error code is returned instead.
		 */
		extern int nanvix_semaphore_up(struct nanvix_semaphore *sem);

	#endif

/**@}*/

#endif /* NANVIX_H_ */

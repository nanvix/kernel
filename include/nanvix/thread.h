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

#ifndef NANVIX_THREAD_H_
#define NANVIX_THREAD_H_

	#include <nanvix/hal/hal.h>
	#include <nanvix/const.h>

	/**
	 * @brief Maximum number of threads.
	 */
	#define THREAD_MAX HAL_NUM_CORES

	/**
	 * @brief Thread states.
	 */
	enum thread_states
	{
		THREAD_NOT_STARTED, /**< Not started. */
		THREAD_RUNNING,     /**< Running.     */
		THREAD_TERMINATED   /**< Terminated.  */
	};

	/**
	 * @brief Thread.
	 */
	struct thread
	{
		int coreid;               /**< Core ID.                */
		enum thread_states state; /**< State.                  */
		void *arg;                /**< Argument.               */
		void *(*start)(void*);    /**< Starting routine.       */
		struct thread *next;      /**< Next thread in a queue. */
	};

	/**
	 * @brief Thread ID.
	 */
	typedef int tid_t;

	/* Forward definitions. */
	EXTERN int thread_create(tid_t *, void*(*)(void*), void *);

	/**
	 * @brief Atomically puts the calling thread to sleep.
	 *
	 * @param queue Target sleeping queue.
	 * @param lock  Spinlock o release.
	 *
	 * @note This function is not thread safe.
	 */
	EXTERN void thread_asleep(struct thread **queue, spinlock_t *lock);

	/**
	 * @brief Wakes all threads in a sleeping queue.
	 */
	EXTERN void thread_wakeup(struct thread **queue);

/*============================================================================*
 *                                 Semaphores                                 *
 *============================================================================*/

	/**
	 * @brief Semahore
	 */
	struct semaphore
	{
		int count;            /**< Semaphore value. */
		spinlock_t lock;      /**< Semaphore lock.  */
		struct thread *queue; /**< Sleeping queue.  */
	};

	/**
	 * @brief Initializes a semaphore.
	 *
	 * The SEMAPHORE_INIT() macro statically initializes the fields of
	 * a semaphore. The initial value of the semaphore is set to @p x
	 * in the initialization.
	 *
	 * @param x Initial semaphore value.
	 */
	#define SEMAPHORE_INIT(x)      \
	{                              \
		.count = (x),              \
		.lock = SPINLOCK_UNLOCKED, \
		.queue = NULL,             \
	}

	/**
	 * @brief Initializes a semaphore.
	 *
	 * The semaphore_init() function dynamically initializes the
	 * fields of the semaphore pointed to by @p sem. The initial value
	 * of the semaphore is set to @p x in the initialization.
	 *
	 * @param sem Target semaphore.
	 * @param x   Initial semaphore value.
	 */
	static inline void semaphore_init(struct semaphore *sem, int x)
	{
		sem->count = x;
		sem->lock = SPINLOCK_UNLOCKED;
		sem->queue = NULL;
	}

	/**
	 * @brief Performs a down operation in a semaphore.
	 *
	 * @param sem Target semaphore.
	 *
	 * @see SEMAPHORE_INIT(), semaphore_up()
	 */
	EXTERN void semaphore_down(struct semaphore *sem);

	/**
	 * @brief Performs an up operation in a semaphore.
	 *
	 * @param sem target semaphore.
	 *
	 * @see SEMAPHORE_INIT(), semaphore_down()
	 */
	EXTERN void semaphore_up(struct semaphore *sem);

#endif /* NANVIX_THREAD_H_ */

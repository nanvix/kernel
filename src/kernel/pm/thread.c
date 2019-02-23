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

#include <nanvix/hal/hal.h>
#include <nanvix/const.h>
#include <nanvix/thread.h>
#include <errno.h>

/*
 * Too few cores in the underlying core.
 */
#if (!defined(HAL_SMP))
	#error "architecture does not have enough cores"
#endif

/**
 * @brief Number of running threads.
 */
PRIVATE int nthreads = 1;

/**
 * @brief Next thread ID.
 */
PRIVATE int next_tid = (KTHREAD_MASTER_TID + 1);

/**
 * @brief Thread table.
 */
PUBLIC struct thread threads[KTHREAD_MAX] = {
	[0]                       = {.tid = KTHREAD_MASTER_TID, .state = THREAD_RUNNING},
	[1 ... (KTHREAD_MAX - 1)] = {.state = THREAD_NOT_STARTED}
};

/**
 * @brief Thread join conditions.
 */
PRIVATE struct condvar joincond[KTHREAD_MAX] = {
	[0 ... (KTHREAD_MAX - 1)] = COND_INITIALIZER
};

/**
 * @brief Thread exit conditions.
 */
PRIVATE struct condvar exitcond[KTHREAD_MAX] = {
	[0 ... (KTHREAD_MAX - 1)] = COND_INITIALIZER
};

/**
 * @brief Thread manager lock.
 */
PRIVATE spinlock_t lock_tm = SPINLOCK_UNLOCKED;

/*============================================================================*
 * thread_alloc()                                                             *
 *============================================================================*/

/**
 * @brief Allocates a thread.
 *
 * The thread_alloc() function allocates a new thread structure in the
 * table of threads.
 *
 * @returns Upon successful completion, a pointer to the newly
 * allocated thread is returned. Upon failure, a NULL pointer is
 * returned instead.
 *
 * @note This function is NOT thread-safe.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE struct thread *thread_alloc(void)
{
	for (int i = 0; i < KTHREAD_MAX; i++)
	{
		/* Found. */
		if (threads[i].state == THREAD_NOT_STARTED)
		{
			struct thread *new_thread;
			new_thread = &threads[i];
			new_thread->state = THREAD_STARTED;
			nthreads++;

			return (new_thread);
		}
	}

	return (NULL);
}

/*============================================================================*
 * thread_free()                                                              *
 *============================================================================*/

/**
 * @brief Releases a thread.
 *
 * The thread_free() function releases the thread entry pointed to by
 * @p t in the table of threads.
 *
 * @note This function is NOT thread-safe.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void thread_free(struct thread *t)
{
	KASSERT(t >= &threads[0]);
	KASSERT(t <= &threads[KTHREAD_MAX - 1]);

	t->state = THREAD_NOT_STARTED;
	nthreads--;
}

/*============================================================================*
 * thread_exit()                                                              *
 *============================================================================*/

/**
 * The thread_exit() function terminates the calling thread. It first
 * releases all underlying kernel resources that are linked to the
 * thread and then resets the underlying core. The return value of the
 * thread, which is pointed to by @p retval, is made available for a
 * thread that joins this one.
 *
 * @note This function does not return.
 * @note This function is thread-safe.
 *
 * @see thread_join().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC NORETURN void thread_exit(void *retval)
{
	int mycoreid;
	struct thread *curr_thread;

	UNUSED(retval);

	curr_thread = thread_get_curr();
	mycoreid = thread_get_coreid(curr_thread);

	spinlock_lock(&lock_tm);

		curr_thread->state = THREAD_TERMINATED;

		cond_broadcast(&joincond[mycoreid]);
		cond_wait(&exitcond[mycoreid], &lock_tm);

		thread_free(curr_thread);

	spinlock_unlock(&lock_tm);

	/* No rollback from this point. */

	core_reset();

	/* Never gets here. */
	while (TRUE)
		noop();
}

/*============================================================================*
 * thread_get()                                                               *
 *============================================================================*/

/**
 * @brief Returns the target thread.
 *
 * The thread_get() function performs a linear search in the table of
 * threads for the thread whose ID equals to @p tid.
 *
 * @param tid ID of the target thread.
 *
 * @returns Upon successful completion, a pointer to the target thread
 * is returned. Upon failure, a null pointed is returned instead.
 *
 * @note This function is NOT thread safe.
 */
PRIVATE struct thread *thread_get(int tid)
{
	/* Search for target thread. */
	for (int i = 0; i < KTHREAD_MAX; i++)
	{
		/* Found. */
		if (threads[i].tid == tid)
			return (&threads[i]);
	}

	return (NULL);
}

/*============================================================================*
 * thread_start()                                                             *
 *============================================================================*/

/**
 * @brief Starts a thread.
 *
 * The thread_start function is a wrapper routine for the user-level
 * thread start routine. Overall, it does some basic, kernel level
 * setup and calls the registered user-level function.
 *
 * @note This function does not return.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE NORETURN void thread_start(void)
{
	void *retval;               /* Return value.   */
	struct thread *curr_thread; /* Current thread. */

	curr_thread = thread_get_curr();

	retval = curr_thread->start(curr_thread->arg);

	thread_exit(retval);

	/* Never gets here. */
	while (TRUE)
		noop();
}

/*============================================================================*
 * thread_create()                                                            *
 *============================================================================*/

/**
 * The thread_create() function create and starts a new thread. The
 * new thread executes the start routine pointed to by @p start, with
 * @p arg begin passed as argument to this function. If @p tid is not
 * a NULL pointed, the ID of the new thread is stored into the
 * location pointed to by @p tid.
 *
 * @retval -EAGAIN Not enough resources to allocate a new thread.
 *
 * @note This function is thread safe.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int thread_create(int *tid, void*(*start)(void*), void *arg)
{
	int _tid;                  /* Unique thread identifier. */
	struct thread *new_thread; /* New thread.               */

	/* Sanity check. */
	KASSERT(start != NULL);

	spinlock_lock(&lock_tm);

		/* Allocate thread. */
		new_thread = thread_alloc();
		if (new_thread == NULL)
		{
			spinlock_unlock(&lock_tm);
			return (-EAGAIN);
		}

		/* Get thread ID. */
		_tid = next_tid++;

		/* Initialize thread structure. */
		new_thread->tid = _tid;
		new_thread->state = THREAD_RUNNING;
		new_thread->arg = arg;
		new_thread->start = start;
		new_thread->next = NULL;

	spinlock_unlock(&lock_tm);


	/* Save thread ID. */
	if (tid != NULL)
	{
		*tid = _tid;
		hal_dcache_invalidate();
	}

	core_start(thread_get_coreid(new_thread), thread_start);

	return (0);
}

/*============================================================================*
 * thread_join()                                                              *
 *============================================================================*/

/**
 * The thread_join() function causes the calling thread to block until
 * the thread whose ID equals to @p tid its terminates execution, by
 * calling thread_exit(). If @p retval is not a null pointed, then the
 * return value of the terminated thread is stored in the location
 * pointed to by @p retval.
 *
 * @retval -EINVAL Invalid thread ID.
 *
 * @see thread_exit().
 *
 * @todo Retrieve return value.
 */
PUBLIC int thread_join(int tid, void **retval)
{
	struct thread *t;
	int ret = -EINVAL;

	/* Sanity check. */
	KASSERT(tid >= 0);
	KASSERT(tid != thread_get_id(thread_get_curr()));
	KASSERT(tid != KTHREAD_MASTER_TID);

	UNUSED(retval);

	spinlock_lock(&lock_tm);

		/* Wait for thread termination. */
		if ((t = thread_get(tid)) != NULL)
		{
			int coreid;

			ret = 0;
			coreid = thread_get_coreid(t);

			/*
			 * The target thread is still running,
			 * so we have to block and wait for it.
			 */
			if (t->state == THREAD_RUNNING)
				cond_wait(&joincond[coreid], &lock_tm);

			cond_broadcast(&exitcond[coreid]);
		}

	spinlock_unlock(&lock_tm);

	return (ret);
}

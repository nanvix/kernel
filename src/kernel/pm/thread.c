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

/**
 * @brief Number of running threads.
 */
PRIVATE int nthreads = 1;

/**
 * @brief Next thread ID.
 */
PRIVATE int next_tid = 1;

/**
 * @brief Thread table.
 */
PUBLIC struct thread threads[THREAD_MAX] = {
	[0]                      = {.state = THREAD_RUNNING},
	[1 ... (THREAD_MAX - 1)] = {.state = THREAD_NOT_STARTED}
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
 * @note This function is thread-safe.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE struct thread *thread_alloc(void)
{
	spinlock_lock(&lock_tm);
		for (int i = 0; i < THREAD_MAX; i++)
		{
			/* Found. */
			if (threads[i].state == THREAD_NOT_STARTED)
			{
				struct thread *new_thread;
				new_thread = &threads[i];
				new_thread->state = THREAD_STARTED;
				nthreads++;

				spinlock_unlock(&lock_tm);

				return (new_thread);
			}
		}
	spinlock_unlock(&lock_tm);

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
 * @note This function is thread-safe.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void thread_free(struct thread *t)
{
	KASSERT(t >= &threads[0]);
	KASSERT(t <= &threads[THREAD_MAX - 1]);

	spinlock_lock(&lock_tm);
		t->state = THREAD_NOT_STARTED;
		nthreads--;
	spinlock_unlock(&lock_tm);
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
 *
 * @note This function is thread-safe.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC NORETURN void thread_exit(void *retval)
{
	struct thread *curr_thread;

	UNUSED(retval);

	curr_thread = thread_get_curr();
	curr_thread->state = THREAD_TERMINATED;

	/* Flush changes. */
	hal_dcache_invalidate();

	thread_free(curr_thread);

	core_reset();

	/* Never gets here. */
	while (TRUE)
		noop();
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

	/* Allocate thread. */
	if ((new_thread = thread_alloc()) == NULL)
		return (-EAGAIN);

	/* Get thread ID. */
	_tid = next_tid++;

	/* Initialize thread structure. */
	new_thread->tid = _tid;
	new_thread->state = THREAD_RUNNING;
	new_thread->arg = arg;
	new_thread->start = start;
	new_thread->next = NULL;

	/* Save thread ID. */
	if (tid != NULL)
		*tid = _tid;

	/* Flush changes. */
	hal_dcache_invalidate();

	core_start(thread_get_coreid(new_thread), thread_start);

	return (0);
}

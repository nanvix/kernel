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
	KASSERT(t < &threads[THREAD_MAX - 1]);

	spinlock_lock(&lock_tm);
		t->state = THREAD_NOT_STARTED;
		nthreads--;
	spinlock_unlock(&lock_tm);
}

/*============================================================================*
 * thread_exit()                                                              *
 *============================================================================*/

/**
 * @brief Terminates the current thread.
 *
 * The thread_exit() function terminates the current thread. It first
 * releases all underlying kernel resources that are linked to the
 * thread and then resets the underlying core.
 *
 * @note This function does not return.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE NORETURN void thread_exit(void)
{
	struct thread *curr_thread;

	curr_thread = thread_get();
	curr_thread->state = THREAD_TERMINATED;

	/* Flush changes. */
	hal_dcache_invalidate();

	thread_free(curr_thread);

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
	struct thread *curr_thread;

	curr_thread = thread_get();

	curr_thread->start(curr_thread->arg);

	thread_exit();

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

/*============================================================================*
 * thread_asleep()                                                            *
 *============================================================================*/

/**
 * The thread_asleep() function atomically places the calling thread
 * in the sleeping queue pointed to by @p queue. Before sleeping, the
 * spinlock pointed to by @p lock is released. The calling thread
 * resumes execution when another thread calls thread_wakeup() in the
 * same sleeping queue @p queue. When the thread wakes up, the
 * spinlock @p lock is unlocked. If this spinlock has to locked, it is
 * up to the caller to re-acquire this lock.
 *
 * @note This function is not thread safe.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void thread_asleep(struct thread **queue, spinlock_t *lock)
{
	struct thread *curr_thread;

	KASSERT(queue != NULL);
	KASSERT(lock != NULL);

	curr_thread = thread_get();

	/* Enqueue calling thread. */
	curr_thread->next = *queue;
	*queue = curr_thread;

	/* Flush changes. */
	hal_dcache_invalidate();

	/*
	 * Atomically put the
	 * calling thread to sleep.
	 */
	spinlock_unlock(lock);
	core_sleep();
}

/*============================================================================*
 * thread_wakeup()                                                            *
 *============================================================================*/

/**
 * The thread_wakeup() function wakes up all threads in the sleeping
 * queue pointed to by @p queue.
 *
 * @note This function is not thread safe.
 *
 * @see thread_asleep()
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void thread_wakeup(struct thread **queue)
{
	/* Wakeup all threads. */
	while (*queue != NULL)
	{
		core_wakeup(thread_get_coreid(*queue));
		*queue = (*queue)->next;
	}

	/* Flush changes. */
	hal_dcache_invalidate();
}

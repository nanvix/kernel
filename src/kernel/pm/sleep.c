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

	curr_thread = thread_get_curr();

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

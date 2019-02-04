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
 * cond_wait()                                                                *
 *============================================================================*/

/**
 * The cond_wait() function causes the calling thread to block, until
 * the condition variable pointed to by @p cond is signaled and the
 * calling thread is chosen to run. If @p lock is unlocked before the
 * calling thread blocks, and when it wakes up the lock is
 * re-acquired.
 *
 * @see cond_broadcast()
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int cond_wait(struct condvar *cond, spinlock_t *lock)
{
	struct thread *curr_thread;

	KASSERT(cond != NULL);
	KASSERT(lock != NULL);

	curr_thread = thread_get_curr();

	/* Enqueue calling thread. */
	spinlock_lock(&cond->lock);
		curr_thread->next = cond->queue;
		cond->queue = curr_thread;
	spinlock_unlock(&cond->lock);

	/* Put the calling thread to sleep. */
	spinlock_unlock(lock);
		core_sleep();

	spinlock_lock(lock);

	return (0);
}

/*============================================================================*
 * cond_broadcast()                                                           *
 *============================================================================*/

/**
 * The cond_broadcast() function sends a wakeup signal to all threads
 * that are currently blocked waiting on the conditional variable
 * pointed to by @p cond.
 *
 * @see cond_wait().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int cond_broadcast(struct condvar *cond)
{
	KASSERT(cond != NULL);

	spinlock_lock(&cond->lock);

		/* Wakeup all threads. */
		while (cond->queue != NULL)
		{
			core_wakeup(thread_get_coreid(cond->queue));
			cond->queue = cond->queue->next;
		}

	spinlock_unlock(&cond->lock);

	return (0);
}

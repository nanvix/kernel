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

#include <nanvix.h>
#include <errno.h>
#include <stdbool.h>

#if (CORES_NUM > 1)

/*============================================================================*
 * nanvix_mutex_init()                                                        *
 *============================================================================*/

/**
 * @brief Initializes a mutex.
 * 
 * @param m Target mutex.
 *
 * @param Upon sucessful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int nanvix_mutex_init(struct nanvix_mutex *m)
{
	/* Invalid mutex. */
	if (m == NULL)
		return (-EINVAL);

	m->locked = false;
	m->lock = SPINLOCK_UNLOCKED;
	for (int i = 0; i < THREAD_MAX; i++)
		m->tids[i] = -1;
	dcache_invalidate();

	return (0);
}

/*============================================================================*
 * nanvix_mutex_lock()                                                        *
 *============================================================================*/

/**
 * @brief Locks a mutex.
 * 
 * @param m Target mutex.
 *
 * @param Upon sucessful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int nanvix_mutex_lock(struct nanvix_mutex *m)
{
	kthread_t tid;

	/* Invalid mutex. */
	if (m == NULL)
		return (-EINVAL);

	tid = kthread_self();

	do
	{
		spinlock_lock(&m->lock);

			/* Dequeue kernel thread. */
			for (int i = 0; i < THREAD_MAX; i++)
			{
				if (m->tids[i] == tid)
				{
					for (int j = i; j < (THREAD_MAX - 1); j++)
						m->tids[j] = m->tids[j + 1];
					m->tids[THREAD_MAX - 1] = -1;
					break;
				}
			}

			/* Lock. */
			if (!m->locked)
			{
				m->locked = true;
				spinlock_unlock(&m->lock);
				break;
			}

			/* Enqueue kernel thread. */
			for (int i = 0; i < THREAD_MAX; i++)
			{
				if (m->tids[i] == -1)
				{
					m->tids[i] = tid;
					break;
				}
			}

		spinlock_unlock(&m->lock);

		sleep();
	} while (true);

	return (0);
}

/*============================================================================*
 * nanvix_mutex_unlock()                                                      *
 *============================================================================*/

/**
 * @brief unlocks a mutex.
 * 
 * @param m Target mutex.
 *
 * @param Upon sucessful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int nanvix_mutex_unlock(struct nanvix_mutex *m)
{
	/* Invalid mutex. */
	if (m == NULL)
		return (-EINVAL);

again:

	spinlock_lock(&m->lock);

		if (m->tids[0] != -1)
		{
			if (wakeup(m->tids[0]) != 0)
			{
				spinlock_unlock(&m->lock);
				goto again;
			}
		}

		m->locked = false;

	spinlock_unlock(&m->lock);

	return (0);
}

#endif /* CORES_NUM > 1 */

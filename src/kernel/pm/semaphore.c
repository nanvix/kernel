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

#include <nanvix/const.h>
#include <nanvix/thread.h>

/**
 * The semaphore_down() function performs a down operation in the
 * semaphore pointed to by @p sem. It atomically checks the current
 * value of @p sem. If it is greater than one, it decrements the this
 * value by one and the calling thread continue its execution flow.
 * Otherwise, the calling thread sleep until anothre thread performs a
 * semaphore_up() on this semaphore, and this value becomes greater
 * than one.
 */
PUBLIC void semaphore_down(struct semaphore *sem)
{
	while (TRUE)
	{
		spinlock_lock(&sem->lock);

			if (sem->count > 0)
				break;

		thread_asleep(&sem->queue, &sem->lock);
	}

	sem->count--;
	spinlock_unlock(&sem->lock);
}

/**
 * The semaphore_up() function performs an up operation in a semaphore
 * pointed to by @p sem. It atomically increments the current value of
 * @p and wakes up all threads that were sleeping in this semaphore,
 * waiting for a semaphore_up() operation.
 */
PUBLIC void semaphore_up(struct semaphore *sem)
{
	spinlock_lock(&sem->lock);
		sem->count++;
		thread_wakeup(&sem->queue);
	spinlock_unlock(&sem->lock);
}

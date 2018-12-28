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
#include <errno.h>

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
	enum thread_states state; /**< State.            */
	void *arg;                /**< Argument.         */
	void *(*start)(void*);    /**<Starting routine.  */
};

/**
 * @brief Number of running threads.
 */
PRIVATE int nthreads = 1;

/**
 * @brief Thread table.
 */
PRIVATE struct thread threads[THREAD_MAX] = {
	[0]                      = {.state = THREAD_RUNNING},
	[1 ... (THREAD_MAX - 1)] = {.state = THREAD_NOT_STARTED}
};

/**
 * @brief Terminates the calling thread.
 */
PRIVATE void thread_exit(void)
{
	int coreid;

	coreid = core_get_id();
	threads[coreid].state = THREAD_TERMINATED;
	hal_dcache_invalidate();
}

/**
 * @brief Starts a thread.
 */
PRIVATE void thread_start(void)
{
	int coreid;

	coreid = core_get_id();
	threads[coreid].start(threads[coreid].arg);

	thread_exit();
}

/**
 * @brief Creates a thread.
 *
 * @param tid   Place to store the ID of the thread.
 * @param start Starting routine.
 * @param arg   Argument.
 *
 * @returns Upon successful completion zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
PUBLIC int thread_create(tid_t *tid, void*(*start)(void*), void *arg)
{
	/* Too many threads running. */
	if (nthreads >= THREAD_MAX)
		return (-EAGAIN);

	/* Initialize thread structure. */
	threads[nthreads].arg = arg;
	threads[nthreads].start = start;
	threads[nthreads].state = THREAD_RUNNING;
	hal_dcache_invalidate();

	core_wakeup(nthreads, thread_start);

	*tid = nthreads++;

	return (0);
}

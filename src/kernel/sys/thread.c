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
 * @see thread_get_id().
 */
PUBLIC int sys_thread_get_id(void)
{
	return (thread_get_id(thread_get_curr()));
}

/**
 * @see thread_create().
 */
PUBLIC int sys_thread_create(int *tid, void*(*start)(void*), void *arg)
{
	return (thread_create(tid, start, arg));
}

/**
 * @see thread_exit().
 */
PUBLIC int sys_thread_exit(void *retval)
{
	thread_exit(retval);

	return (-EAGAIN);
}

/**
 * @see thread_join().
 */
PUBLIC int sys_thread_join(int tid, void **retval)
{
	return (thread_join(tid, retval));
}

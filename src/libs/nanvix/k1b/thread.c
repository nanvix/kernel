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
#include <vbsp.h>

/*============================================================================*
 * kthread_self()                                                             *
 *============================================================================*/

/*
 * @see sys_kthread_get_id()
 */
kthread_t kthread_self(void)
{
	kthread_t tid;

	tid = __k1_club_syscall0(
		NR_thread_get_id
	);

	return (tid);
}

/*============================================================================*
 * kthread_create()                                                           *
 *============================================================================*/

/*
 * @see sys_kthread_create()
 */
kthread_t kthread_create(
	kthread_t *tid,
	void *(*start)(void*),
	void *arg
)
{
	int ret;

	ret = __k1_club_syscall3(
		NR_thread_create,
		(unsigned) tid,
		(unsigned) start,
		(unsigned) arg
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/*============================================================================*
 * kthread_exit()                                                             *
 *============================================================================*/

/*
 * @see sys_kthread_exit().
 */
int kthread_exit(void *retval)
{
	int ret;

	ret = __k1_club_syscall1(
		NR_thread_exit,
		(unsigned) retval
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/*============================================================================*
 * kthread_join()                                                             *
 *============================================================================*/

/*
 * @see sys_kthread_join()
 */
int kthread_join(
	kthread_t tid,
	void **retval
)
{
	int ret;

	ret = __k1_club_syscall2(
		NR_thread_join,
		(unsigned) tid,
		(unsigned) retval
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/*============================================================================*
 * sleep()                                                                    *
 *============================================================================*/

/*
 * @see sys_sleep()
 */
int sleep(void)
{
	int ret;

	ret = __k1_club_syscall0(
		(unsigned) NR_sleep
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/*============================================================================*
 * wakeup()                                                                   *
 *============================================================================*/

/*
 * @see sys_wakeup()
 */
int wakeup(kthread_t tid)
{
	int ret;

	ret = __k1_club_syscall1(
		(unsigned) NR_wakeup,
		(unsigned) tid
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

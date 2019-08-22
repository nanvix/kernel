/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

/*============================================================================*
 * kthread_self()                                                             *
 *============================================================================*/

/*
 * @see sys_kthread_get_id()
 */
kthread_t kthread_self(void)
{
	return (kcall0(NR_thread_get_id));
}

/*============================================================================*
 * kthread_create()                                                           *
 *============================================================================*/

/*
 * @see sys_kthread_create()
 */
int kthread_create(
	kthread_t *tid,
	void *(*start)(void*),
	void *arg
)
{
	int ret;

	ret = kcall3(
		NR_thread_create,
		(word_t) tid,
		(word_t) start,
		(word_t) arg
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

	ret = kcall1(
		NR_thread_exit,
		(word_t) retval
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

	ret = kcall2(
		NR_thread_join,
		(word_t) tid,
		(word_t) retval
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
 * ksleep()                                                                   *
 *============================================================================*/

/*
 * @see sys_sleep()
 */
int ksleep(void)
{
	int ret;

	ret = kcall0(NR_sleep);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/*============================================================================*
 * kwakeup()                                                                  *
 *============================================================================*/

/*
 * @see sys_wakeup()
 */
int kwakeup(kthread_t tid)
{
	int ret;

	ret = kcall1(
		NR_wakeup,
		(word_t) tid
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

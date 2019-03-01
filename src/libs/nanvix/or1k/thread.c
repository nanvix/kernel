/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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
	register kthread_t tid
		__asm__("r11") = NR_thread_get_id;

	__asm__ volatile (
		"l.sys 1"
		: "=r" (tid)
		: "r"  (tid)
	);

	return (tid);
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
	register unsigned arg0
		__asm__("r3") = (unsigned) tid;
	register unsigned arg1
		__asm__("r4") = (unsigned) start;
	register unsigned arg2
		__asm__("r5") = (unsigned) arg;

	register int ret
		__asm__("r11") = NR_thread_create;

	__asm__ volatile (
		"l.sys 1"
		: "=r" (ret)
		: "r" (ret),
		"r" (arg0),
		"r" (arg1),
		"r" (arg2)
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
	register unsigned arg0
		__asm__("r3") = (unsigned) retval;

	register int ret
		__asm__("r11") = NR_thread_exit;

	__asm__ volatile (
		"l.sys 1"
		: "=r" (ret)
		: "r" (ret),
		"r" (arg0)
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
	register unsigned arg0
		__asm__("r3") = (unsigned) tid;
	register unsigned arg1
		__asm__("r4") = (unsigned) retval;

	register int ret
		__asm__("r11") = NR_thread_join;

	__asm__ volatile (
		"l.sys 1"
		: "=r" (ret)
		: "r" (ret),
		"r" (arg0),
		"r" (arg1)
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
	register int ret
		__asm__("r11") = NR_sleep;

	__asm__ volatile (
		"l.sys 1"
		: "=r" (ret)
		: "r"  (ret)
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
	register unsigned arg0
		__asm__("r3") = (unsigned) tid;

	register int ret
		__asm__("r11") = NR_wakeup;

	__asm__ volatile (
		"l.sys 1"
		: "=r" (ret)
		: "r" (ret),
		"r" (arg0)
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

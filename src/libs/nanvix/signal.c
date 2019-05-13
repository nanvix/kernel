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
 * ksigclt()                                                                  *
 *============================================================================*/

/*
 * @see sys_sigclt()
 */
int ksigclt(
	int signum,
	struct sigaction * sigact
)
{
	int ret;

	ret = syscall2(
		NR_sigclt,
		(word_t) signum,
		(word_t) sigact
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	dcache_invalidate();

	return (ret);
}

/*============================================================================*
 * kalarm()                                                                   *
 *============================================================================*/

/*
 * @see sys_alarm()
 */
int kalarm(int seconds)
{
	int ret;

	ret = syscall1(
		NR_alarm,
		(word_t) seconds
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
 * ksigsend()                                                                 *
 *============================================================================*/

/*
 * @see sys_sigsend().
 */
int ksigsend(int signum, int tid)
{
	int ret;

	ret = syscall2(
		NR_sigsend,
		(word_t) signum,
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

/*============================================================================*
 * ksigwait()                                                                 *
 *============================================================================*/

/*
 * @see sys_sigwait()
 */
int ksigwait(
	int signum
)
{
	int ret;

	ret = syscall1(
		NR_sigwait,
		(word_t) signum
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
 * ksigreturn()                                                               *
 *============================================================================*/

/*
 * @see sys_sigreturn()
 */
int ksigreturn(void)
{
	int ret;

	ret = syscall0(
		NR_sigreturn
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

	



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
 * ksigctl()                                                                  *
 *============================================================================*/

/**
 * The ksigctl() function modifies the treatment of a signal.
 */
int ksigctl(
	int signum,
	struct ksigaction *sigact
)
{
	int ret;

	ret = kcall2(
		NR_sigctl,
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

/**
 * The kalarm() function schedule an alarm signal to trigger when
 * the @seconds seconds pass.
 */
int kalarm(int seconds)
{
	int ret;

	ret = kcall1(
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

/**
 * The ksigsend() function sends a signal @signum to another thread @tid.
 */
int ksigsend(int signum, int tid)
{
	int ret;

	ret = kcall2(
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

/**
 * The ksigwait() function waits for the receipt of a @signum signal.
 */
int ksigwait(
	int signum
)
{
	int ret;

	ret = kcall1(
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

/**
 * The ksigreturn() function returns from a signal handler, restoring the
 * execution stream.
 */
int ksigreturn(void)
{
	int ret;

	ret = kcall0(
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

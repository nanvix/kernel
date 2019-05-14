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
#include <nanvix/mm.h>
#include <nanvix/signal.h>
#include <errno.h>

/*============================================================================*
 * sys_sigclt()                                                               *
 *============================================================================*/

PUBLIC int sys_sigclt(int signum, struct sigaction * sigact)
{
	if (sigact == NULL)
		return (-EAGAIN);

	/* Bad struct location. */
	if (!mm_check_area(VADDR(sigact), sizeof(struct sigaction), UMEM_AREA))
		return (-EFAULT);

	/* Bad handler address. */
	if (!mm_check_area(VADDR(sigact->handler), 0, UMEM_AREA))
		return (-EFAULT);

	return sigclt(signum, sigact);
}

/*============================================================================*
 * sys_alarm()                                                                *
 *============================================================================*/

PUBLIC int sys_alarm(int seconds)
{
	return alarm(seconds);
}

/*============================================================================*
 * sys_sigsend()                                                              *
 *============================================================================*/

PUBLIC int sys_sigsend(int signum, int tid)
{
	return sigsend(signum, tid);
}

/*============================================================================*
 * sys_sigwait()                                                              *
 *============================================================================*/

PUBLIC int sys_sigwait(int signum)
{
	return sigwait(signum);
}

/*============================================================================*
 * sys_sigreturn()                                                            *
 *============================================================================*/

PUBLIC void sys_sigreturn(void)
{
	sigreturn();
}
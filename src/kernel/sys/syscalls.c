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

#include <nanvix/const.h>
#include <nanvix/hal/hal.h>
#include <nanvix/thread.h>
#include <nanvix/klib.h>
#include <nanvix/syscall.h>
#include <errno.h>

/**
 * @brief Semaphore variable for system call dispatcher.
 */
PRIVATE struct semaphore syssem = SEMAPHORE_INITIALIZER(0);

/**
 * @brief System call scoreboard.
 */
PRIVATE struct sysboard
{
	int arg0;                /**< First argument of system call.   */
	int arg1;                /**< Second argument of system call.  */
	int arg2;                /**< Third argument of system call.   */
	int arg3;                /**< Fourth argument of system call.  */
	int arg4;                /**< Fifth argument of system call.   */
	int syscall_nr;          /**< System call number.              */
	int ret;                 /**< Return value of system call.     */
	struct semaphore syssem; /**< Semaphore.                       */
	int pending;
} ALIGN(CACHE_LINE_SIZE) sysboard[CORES_NUM];

/**
 * @brief Handles a system call IPI.
 */
PUBLIC void do_kcall2(void)
{
	int coreid = 0 ;
	int ret = -ENOSYS;

	semaphore_down(&syssem);

		for (int i = 0; i < CORES_NUM; i++)
		{
			if (sysboard[i].pending)
			{
				coreid = i;
				break;
			}
		}

		/* Invalid system call number. */
		if ((sysboard[coreid].syscall_nr < 0) || (sysboard[coreid].syscall_nr >= NR_SYSCALLS))
		{
			ret = -EINVAL;
			goto out;
		}

		/* Parse system call number. */
		switch (sysboard[coreid].syscall_nr)
		{
			case NR__exit:
				sys_exit((int) sysboard[coreid].arg0);
				break;

			case NR_write:
				ret = sys_write(
					(int) sysboard[coreid].arg0,
					(const char *) sysboard[coreid].arg1,
					(size_t) sysboard[coreid].arg2
				);
				break;

#if (THREAD_MAX > 1)

			case NR_thread_create:
				ret = sys_thread_create(
					(int *) sysboard[coreid].arg0,
					(void *(*)(void *)) sysboard[coreid].arg1,
					(void *) sysboard[coreid].arg2
				);
				break;

			case NR_wakeup:
				ret = sys_wakeup(
					(int) sysboard[coreid].arg0
				);
				break;

#endif

			case NR_sigclt:
				ret = sys_sigclt(
					(int) sysboard[coreid].arg0,
					(struct sigaction *) sysboard[coreid].arg1
				);
				break;

			default:
				break;
		}

	out:

		sysboard[coreid].pending = 0;
		sysboard[coreid].ret = ret;

	semaphore_up(&sysboard[coreid].syssem);
}

/**
 * @brief System call dispatcher.
 *
 * @param arg0       First system call argument.
 * @param arg1       Second system call argument.
 * @param arg2       Third system call argument.
 * @param arg3       Fourth system call argument.
 * @param arg4       Fifth system call argument.
 * @param arg5       Sixth system call argument.
 * @param arg6       Seventh system call argument.
 * @param syscall_nr System call number.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_kcall(
	unsigned arg0,
	unsigned arg1,
	unsigned arg2,
	unsigned arg3,
	unsigned arg4,
	unsigned syscall_nr)
{
	int ret = -EINVAL;

	/* Parse system call number. */
	switch (syscall_nr)
	{
		case NR_thread_get_id:
			ret = sys_thread_get_id();
			break;

#if (THREAD_MAX > 1)

		case NR_thread_exit:
			sys_thread_exit((void *) arg0);
			break;

		case NR_thread_join:
			ret = sys_thread_join(
				(int) arg0,
				(void **) arg1
			);
			break;

		case NR_sleep:
			ret = sys_sleep();
			break;

#endif

		case NR_shutdown:
			ret = sys_shutdown();
			break;

		case NR_alarm:
			ret = sys_alarm((int) arg0);
			break;

		case NR_sigsend:
			ret = sys_sigsend(
				(int) arg0,
				(int) arg1
			);
			break;

		case NR_sigwait:
			ret = sys_sigwait((int) arg0);
			break;

		case NR_sigreturn:
			sys_sigreturn();
			ret = 0;
			break;

		/* Forward system call. */
		default:
		{
			int coreid;

			coreid = core_get_id();

			/* Fillup system call board. */
			sysboard[coreid].arg0 = arg0;
			sysboard[coreid].arg1 = arg1;
			sysboard[coreid].arg2 = arg2;
			sysboard[coreid].arg3 = arg3;
			sysboard[coreid].arg4 = arg4;
			sysboard[coreid].syscall_nr = syscall_nr;
			sysboard[coreid].pending = 1;
			semaphore_init(&sysboard[coreid].syssem, 0);

			semaphore_up(&syssem);
			semaphore_down(&sysboard[coreid].syssem);

			ret = sysboard[coreid].ret;
		} break;
	}

	return (ret);
}

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
			case NR_shutdown:
				ret = kernel_shutdown();
				break;

			case NR__exit:
				kernel_exit((int)(long) sysboard[coreid].arg0);
				break;

			case NR_write:
				ret = kernel_write(
					(int)(long) sysboard[coreid].arg0,
					(const char *)(long) sysboard[coreid].arg1,
					(size_t)(long) sysboard[coreid].arg2
				);
				break;

#if (THREAD_MAX > 1)

			case NR_thread_create:
				ret = kernel_thread_create(
					(int *)(long) sysboard[coreid].arg0,
					(void *(*)(void *))(long) sysboard[coreid].arg1,
					(void *)(long) sysboard[coreid].arg2
				);
				break;

			case NR_wakeup:
				ret = kernel_wakeup(
					(int)(long) sysboard[coreid].arg0
				);
				break;

#endif

			case NR_sigctl:
				ret = kernel_sigctl(
					(int)(long) sysboard[coreid].arg0,
					(struct ksigaction *)(long) sysboard[coreid].arg1
				);
				break;

			case NR_sync_create:
				ret = kernel_sync_create(
					(const int *)(long) sysboard[coreid].arg0,
					(int) sysboard[coreid].arg1,
					(int) sysboard[coreid].arg2
				);
				break;

			case NR_sync_open:
				ret = kernel_sync_open(
					(const int *)(long) sysboard[coreid].arg0,
					(int) sysboard[coreid].arg1,
					(int) sysboard[coreid].arg2
				);
				break;

			case NR_sync_unlink:
				ret = kernel_sync_unlink(
					(int) sysboard[coreid].arg0
				);
				break;

			case NR_sync_close:
				ret = kernel_sync_close(
					(int) sysboard[coreid].arg0
				);
				break;

			case NR_sync_signal:
				ret = kernel_sync_signal(
					(int) sysboard[coreid].arg0
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
			ret = kernel_thread_get_id();
			break;

#if (THREAD_MAX > 1)

		case NR_thread_exit:
			kernel_thread_exit((void *)(long) arg0);
			break;

		case NR_thread_join:
			ret = kernel_thread_join(
				(int) arg0,
				(void **)(long) arg1
			);
			break;

		case NR_sleep:
			ret = kernel_sleep();
			break;

#endif

		case NR_alarm:
			ret = kernel_alarm((int) arg0);
			break;

		case NR_sigsend:
			ret = kernel_sigsend(
				(int) arg0,
				(int) arg1
			);
			break;

		case NR_sigwait:
			ret = kernel_sigwait((int) arg0);
			break;

		case NR_sigreturn:
			kernel_sigreturn();
			ret = 0;
			break;

		case NR_sync_wait:
			ret = kernel_sync_wait(
				(int) arg0
			);
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

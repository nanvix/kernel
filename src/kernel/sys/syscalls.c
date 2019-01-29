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
#include <nanvix/hal/hal.h>
#include <nanvix/thread.h>
#include <nanvix/klib.h>
#include <nanvix/syscall.h>
#include <errno.h>

/**
 * @brief Master semaphore.
 */
PRIVATE struct semaphore syssem = SEMAPHORE_INIT(0);

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
	int arg5;                /**< Sixth argument of system call.   */
	int arg6;                /**< Seventh argument of system call. */
	int syscall_nr;          /**< System call number.              */
	int ret;                 /**< Return value of system call.     */
	struct semaphore syssem; /**< Semaphore.                       */
	int pending;
} __attribute__((aligned(CACHE_LINE_SIZE))) sysboard[HAL_NUM_CORES];

/**
 * @brief Handles a system call IPI.
 */
PUBLIC void do_syscall2(void)
{
	int coreid = 0 ;
	int ret = -ENOSYS;

	semaphore_down(&syssem);

	hal_dcache_invalidate();

	for (int i = 0; i < HAL_NUM_CORES; i++)
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
		case NR_nosyscall:
			ret = sys_nosyscall((unsigned) sysboard[coreid].arg0);
			break;

		case NR_cache_flush:
			ret = sys_cache_flush();
			break;

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

		default:
			break;
	}

out:

	sysboard[coreid].pending = 0;
	sysboard[coreid].ret = ret;
	hal_dcache_invalidate();

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
PUBLIC int do_syscall1(
	int arg0,
	int arg1,
	int arg2,
	int arg3,
	int arg4,
	int arg5,
	int arg6,
	int syscall_nr)
{
	int coreid;

	coreid = core_get_id();

	/* Fillup system call scoreboard. */
	sysboard[coreid].arg0 = arg0;
	sysboard[coreid].arg1 = arg1;
	sysboard[coreid].arg2 = arg2;
	sysboard[coreid].arg3 = arg3;
	sysboard[coreid].arg4 = arg4;
	sysboard[coreid].arg5 = arg5;
	sysboard[coreid].arg6 = arg6;
	sysboard[coreid].syscall_nr = syscall_nr;
	sysboard[coreid].pending = 1;
	semaphore_init(&sysboard[coreid].syssem, 0);
	hal_dcache_invalidate();

	semaphore_up(&syssem);
	semaphore_down(&sysboard[coreid].syssem);

	hal_dcache_invalidate();

	return (sysboard[coreid].ret);
}

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
#include <nanvix/klib.h>
#include <nanvix/syscall.h>
#include <errno.h>

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
PUBLIC int do_syscall(
	int arg0,
	int arg1,
	int arg2,
	int arg3,
	int arg4,
	int arg5,
	int arg6,
	int syscall_nr)
{
	int ret = -ENOSYS;

	/* Invalid system call number. */
	if ((syscall_nr < 0) || (syscall_nr >= NR_SYSCALLS))
		return (-EINVAL);

	UNUSED(arg3);
	UNUSED(arg4);
	UNUSED(arg5);
	UNUSED(arg6);

	/* Parse system call number. */
	switch (syscall_nr)
	{
		case NR_nosyscall:
			ret = sys_nosyscall((unsigned) arg0);
			break;

		case NR_cache_flush:
			ret = sys_cache_flush();
			break;

		case NR__exit:
			sys_exit((int) arg0);
			break;

		case NR_write:
			ret = sys_write((int) arg0, (const char *) arg1, (size_t) arg2);
			break;

		default:
			break;
	}

	return (ret);
}

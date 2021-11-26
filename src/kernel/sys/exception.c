/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

#include <nanvix/kernel/thread.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/excp.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <posix/errno.h>

#if (THREAD_MAX > 1)

/**
 * The kernel_excp_ctrl() function sets @p h as the user-space handler for
 * handling the exception @p excpnum.
 */
PUBLIC int kernel_excp_ctrl(int excpnum, int action)
{
	/* Invalid exception. */
	if (!exception_is_valid(excpnum))
		return (-EINVAL);

	/* Invalid action. */
	if ((action != EXCP_ACTION_IGNORE) && (action != EXCP_ACTION_HANDLE))
		return (-EINVAL);

	return (exception_control(excpnum, action));
}

#if __NANVIX_USE_EXCEPTION_WITH_TASKS

/**
 * The kernel_excp_ctrl() function sets @p h as the user-space handler for
 * handling the exception @p excpnum.
 */
PUBLIC int kernel_excp_set_handler(int excpnum, exception_handler_fn handler)
{
	/* Invalid exception. */
	if (!exception_is_valid(excpnum))
		return (-EINVAL);

	return (exception_set_handler(excpnum, handler));
}

#endif

/**
 * The kernel_excp_resume() function pauses the user-space handler and
 * waits for the develiery of the exception @p excpnum.
 */
PUBLIC int kernel_excp_pause(struct exception *excp)
{
	/* Invalid store location. */
	if (excp != NULL)
	{
		/* Bad store location. */
		if (!mm_check_area(VADDR(excp), sizeof(struct exception), UMEM_AREA))
			return (-EFAULT);
	}

	return (exception_pause(excp));
}

/**
 * The kernel_excp_resume() function resumes the kernel-space handler of
 * the exception @p excpnum.
 */
PUBLIC int kernel_excp_resume(void)
{
	return (exception_resume());
}

#endif /* THREAD_MAX > 1*/

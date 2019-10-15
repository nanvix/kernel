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

#include <nanvix/hal/hal.h>
#include <nanvix/kernel/mailbox.h>
#include <nanvix/kernel/mm.h>
#include <posix/stdarg.h>

#if __TARGET_HAS_MAILBOX

/*============================================================================*
 * kernel_mailbox_create()                                                    *
 *============================================================================*/

/**
 * @see do_vmailbox_create().
 */
PUBLIC int kernel_mailbox_create(int local)
{
	/* Invalid local ID. */
	if (!WITHIN(local, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	return (do_vmailbox_create(local));
}

/*============================================================================*
 * kernel_mailbox_open()                                                      *
 *============================================================================*/

/**
 * @see do_vmailbox_open().
 */
PUBLIC int kernel_mailbox_open(int remote)
{
	/* Invalid remote ID. */
	if (!WITHIN(remote, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	return (do_vmailbox_open(remote));
}

/*============================================================================*
 * kernel_mailbox_unlink()                                                    *
 *============================================================================*/

/**
 * @see do_vmailbox_unlink().
 */
PUBLIC int kernel_mailbox_unlink(int mbxid)
{
	/* Invalid mailbox ID. */
	if (mbxid < 0)
		return (-EINVAL);

	return (do_vmailbox_unlink(mbxid));
}

/*============================================================================*
 * kernel_mailbox_close()                                                     *
 *============================================================================*/

/**
 * @see do_vmailbox_close().
 */
PUBLIC int kernel_mailbox_close(int mbxid)
{
	/* Invalid mailbox ID. */
	if (mbxid < 0)
		return (-EINVAL);

	return (do_vmailbox_close(mbxid));
}

/*============================================================================*
 * kernel_mailbox_awrite()                                                    *
 *============================================================================*/

/**
 * @see do_vmailbox_awrite().
 */
PUBLIC int kernel_mailbox_awrite(int mbxid, const void *buffer, size_t size)
{
	/* Invalid mailbox ID. */
	if (mbxid < 0)
		return (-EINVAL);

	/* Invalid write size. */
	if (size != MAILBOX_MSG_SIZE)
		return (-EINVAL);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Bad buffer location. */
	if (!mm_check_area(VADDR(buffer), size, UMEM_AREA))
		return (-EFAULT);

	return (do_vmailbox_awrite(mbxid, buffer, size));
}

/*============================================================================*
 * kernel_mailbox_aread()                                                     *
 *============================================================================*/

/**
 * @see do_vmailbox_aread().
 */
PUBLIC int kernel_mailbox_aread(int mbxid, void *buffer, size_t size)
{
	/* Invalid mailbox ID. */
	if (mbxid < 0)
		return (-EINVAL);

	/* Invalid read size. */
	if (size != MAILBOX_MSG_SIZE)
		return (-EINVAL);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Bad buffer location. */
	if (!mm_check_area(VADDR(buffer), size, UMEM_AREA))
		return (-EFAULT);

	return (do_vmailbox_aread(mbxid, buffer, size));
}

/*============================================================================*
 * kernel_mailbox_wait()                                                      *
 *============================================================================*/

/**
 * @see do_vmailbox_wait().
 */
PUBLIC int kernel_mailbox_wait(int mbxid)
{
	/* Invalid mailbox ID. */
	if (mbxid < 0)
		return (-EINVAL);

	return (do_vmailbox_wait(mbxid));
}

/*============================================================================*
 * kernel_mailbox_ioctl()                                                     *
 *============================================================================*/

/**
 * @see do_vmailbox_ioctl().
 */
PUBLIC int kernel_mailbox_ioctl(int mbxid, unsigned request, va_list *args)
{
	int ret;

	/* Invalid mailbox ID. */
	if (mbxid < 0)
		return (-EINVAL);

	/* Bad args. */
	if (args == NULL)
		return (-EINVAL);

	dcache_invalidate();
		ret = do_vmailbox_ioctl(mbxid, request, *args);
	dcache_invalidate();

	return (ret);
}

#endif /* __TARGET_HAS_MAILBOX */

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

#include <nanvix/hal/hal.h>
#include <nanvix/kernel/mailbox.h>
#include <posix/errno.h>

#if __TARGET_HAS_MAILBOX

/*============================================================================*
 * kernel_mailbox_create()                                                    *
 *============================================================================*/

/**
 * @see _mailbox_create().
 */
PUBLIC int kernel_mailbox_create(int local)
{
	return (_mailbox_create(local));
}

/*============================================================================*
 * kernel_mailbox_open()                                                      *
 *============================================================================*/

/**
 * @see _mailbox_open().
 */
PUBLIC int kernel_mailbox_open(int remote)
{
	return (_mailbox_open(remote));
}

/**
 * @see _mailbox_unlink().
 */
PUBLIC int kernel_mailbox_unlink(int mbxid)
{
	return (_mailbox_unlink(mbxid));
}

/**
 * @see _mailbox_close().
 */
PUBLIC int kernel_mailbox_close(int mbxid)
{
	return (_mailbox_close(mbxid));
}

/**
 * @see _mailbox_awrite().
 */
PUBLIC int kernel_mailbox_awrite(int mbxid, const void *buffer, size_t size)
{
	return (_mailbox_awrite(mbxid, buffer, size));
}

/**
 * @see _mailbox_aread().
 */
PUBLIC int kernel_mailbox_aread(int mbxid, void *buffer, size_t size)
{
	return (_mailbox_aread(mbxid, buffer, size));
}

/**
 * @see _mailbox_wait().
 */
PUBLIC int kernel_mailbox_wait(int mbxid)
{
	return (_mailbox_wait(mbxid));
}

#endif /* __TARGET_HAS_MAILBOX */

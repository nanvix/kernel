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

#include <nanvix.h>
#include <errno.h>

#if __TARGET_HAS_MAILBOX

/*============================================================================*
 * kmailbox_create()                                                          *
 *============================================================================*/

/*
 * @see sys_mailbox_create()
 */
int kmailbox_create(int local)
{
	int ret;

	ret = kcall1(
		NR_mailbox_create,
		(word_t) local
	);

	return (ret);
}

/*============================================================================*
 * kmailbox_open()                                                            *
 *============================================================================*/

/*
 * @see sys_mailbox_open()
 */
int kmailbox_open(int remote)
{
	int ret;

	ret = kcall1(
		NR_mailbox_open,
		(word_t) remote
	);

	return (ret);
}

/*============================================================================*
 * kmailbox_unlink()                                                          *
 *============================================================================*/

/*
 * @see sys_mailbox_unlink()
 */
int kmailbox_unlink(int mbxid)
{
	int ret;

	ret = kcall1(
		NR_mailbox_unlink,
		(word_t) mbxid
	);

	return (ret);
}

/*============================================================================*
 * kmailbox_close()                                                           *
 *============================================================================*/

/*
 * @see sys_mailbox_close()
 */
int kmailbox_close(int mbxid)
{
	int ret;

	ret = kcall1(
		NR_mailbox_close,
		(word_t) mbxid
	);

	return (ret);
}

/*============================================================================*
 * kmailbox_awrite()                                                          *
 *============================================================================*/

/*
 * @see sys_mailbox_awrite()
 */
int kmailbox_awrite(int mbxid, const void *buffer, size_t size)
{
	int ret;

	ret = kcall3(
		NR_mailbox_awrite,
		(word_t) mbxid,
		(word_t) buffer,
		(word_t) size
	);

	return (ret);
}

/*============================================================================*
 * kmailbox_aread()                                                           *
 *============================================================================*/

/*
 * @see sys_mailbox_aread()
 */
int kmailbox_aread(int mbxid, void *buffer, size_t size)
{
	int ret;

	ret = kcall3(
		NR_mailbox_aread,
		(word_t) mbxid,
		(word_t) buffer,
		(word_t) size
	);

	return (ret);
}

/*============================================================================*
 * kmailbox_wait()                                                            *
 *============================================================================*/

/*
 * @see sys_mailbox_wait()
 */
int kmailbox_wait(int mbxid)
{
	int ret;

	ret = kcall1(
		NR_mailbox_wait,
		(word_t) mbxid
	);

	return (ret);
}

#endif /* __TARGET_HAS_MAILBOX */

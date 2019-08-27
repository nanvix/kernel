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

#if __TARGET_HAS_PORTAL

/*============================================================================*
 * kportal_create()                                                           *
 *============================================================================*/

/*
 * @see sys_portal_create()
 */
int kportal_create(int local)
{
	int ret;

	ret = kcall1(
		NR_portal_create,
		(word_t) local
	);

	return (ret);
}

/*============================================================================*
 * kportal_allow()                                                            *
 *============================================================================*/

/*
 * @see sys_portal_allow()
 */
int kportal_allow(int portalid, int remote)
{
	int ret;

	ret = kcall2(
		NR_portal_allow,
		(word_t) portalid,
		(word_t) remote
	);

	return (ret);
}

/*============================================================================*
 * kportal_open()                                                             *
 *============================================================================*/

/*
 * @see sys_portal_open()
 */
int kportal_open(int local, int remote)
{
	int ret;

	ret = kcall2(
		NR_portal_open,
		(word_t) local,
		(word_t) remote
	);

	return (ret);
}

/*============================================================================*
 * kportal_unlink()                                                           *
 *============================================================================*/

/*
 * @see sys_portal_unlink()
 */
int kportal_unlink(int portalid)
{
	int ret;

	ret = kcall1(
		NR_portal_unlink,
		(word_t) portalid
	);

	return (ret);
}

/*============================================================================*
 * kportal_close()                                                            *
 *============================================================================*/

/*
 * @see sys_portal_close()
 */
int kportal_close(int portalid)
{
	int ret;

	ret = kcall1(
		NR_portal_close,
		(word_t) portalid
	);

	return (ret);
}

/*============================================================================*
 * kportal_aread()                                                            *
 *============================================================================*/

/*
 * @see sys_portal_aread()
 */
int kportal_aread(int portalid, void * buffer, size_t size)
{
	int ret;

	ret = kcall3(
		NR_portal_aread,
		(word_t) portalid,
		(word_t) buffer,
		(word_t) size
	);

	return (ret);
}

/*============================================================================*
 * kportal_awrite()                                                           *
 *============================================================================*/

/*
 * @see sys_portal_awrite()
 */
int kportal_awrite(int portalid, const void * buffer, size_t size)
{
	int ret;

	ret = kcall3(
		NR_portal_awrite,
		(word_t) portalid,
		(word_t) buffer,
		(word_t) size
	);

	return (ret);
}

/*============================================================================*
 * kportal_wait()                                                             *
 *============================================================================*/

/*
 * @see sys_portal_wait()
 */
int kportal_wait(int portalid)
{
	int ret;

	ret = kcall1(
		NR_portal_wait,
		(word_t) portalid
	);

	return (ret);
}

#endif /* __TARGET_HAS_PORTAL */

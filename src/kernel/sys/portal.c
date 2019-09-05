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
#include <nanvix/kernel/portal.h>
#include <posix/stdarg.h>

#if __TARGET_HAS_PORTAL

/*============================================================================*
 * kernel_portal_create()                                                     *
 *============================================================================*/

/**
 * @see do_portal_create().
 */
PUBLIC int kernel_portal_create(int local)
{
	return (do_portal_create(local));
}

/*============================================================================*
 * kernel_portal_allow()                                                      *
 *============================================================================*/

/**
 * @see do_portal_allow().
 */
PUBLIC int kernel_portal_allow(int portalid, int remote)
{
	return (do_portal_allow(portalid, remote));
}

/*============================================================================*
 * kernel_portal_open()                                                       *
 *============================================================================*/

/**
 * @see do_portal_open().
 */
PUBLIC int kernel_portal_open(int local, int remote)
{
	return (do_portal_open(local, remote));
}

/*============================================================================*
 * kernel_portal_unlink()                                                     *
 *============================================================================*/

/**
 * @see do_portal_unlink().
 */
PUBLIC int kernel_portal_unlink(int portalid)
{
	return (do_portal_unlink(portalid));
}

/*============================================================================*
 * kernel_portal_close()                                                      *
 *============================================================================*/

/**
 * @see do_portal_close().
 */
PUBLIC int kernel_portal_close(int portalid)
{
	return (do_portal_close(portalid));
}

/*============================================================================*
 * kernel_portal_awrite()                                                     *
 *============================================================================*/

/**
 * @see do_portal_awrite().
 */
PUBLIC int kernel_portal_awrite(int portalid, const void * buffer, size_t size)
{
	return (do_portal_awrite(portalid, buffer, size));
}

/*============================================================================*
 * kernel_portal_aread()                                                      *
 *============================================================================*/

/**
 * @see do_portal_aread().
 */
PUBLIC int kernel_portal_aread(int portalid, void * buffer, size_t size)
{
	return (do_portal_aread(portalid, buffer, size));
}

/*============================================================================*
 * kernel_portal_wait()                                                       *
 *============================================================================*/

/**
 * @see do_portal_wait().
 */
PUBLIC int kernel_portal_wait(int portalid)
{
	return (do_portal_wait(portalid));
}

/*============================================================================*
 * kernel_portal_wait()                                                       *
 *============================================================================*/

/**
 * @see do_portal_ioctl().
 */
PUBLIC int kernel_portal_ioctl(int mbxid, unsigned request, va_list *args)
{
	int ret;

	dcache_invalidate();
		ret = do_portal_ioctl(mbxid, request, *args);
	dcache_invalidate();

	return (ret);
}

#endif /* __TARGET_HAS_PORTAL */

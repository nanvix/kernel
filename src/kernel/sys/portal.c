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
#include <nanvix/syscall.h>
#include <errno.h>

/*============================================================================*
 * kernel_portal_create()                                                        *
 *============================================================================*/

/**
 * @see _portal_create().
 */
PUBLIC int kernel_portal_create(int local)
{
	return (_portal_create(local));
}

/*============================================================================*
 * kernel_portal_allow()                                                         *
 *============================================================================*/

/**
 * @see _portal_allow().
 */
PUBLIC int kernel_portal_allow(int portalid, int remote)
{
	return (_portal_allow(portalid, remote));
}

/*============================================================================*
 * kernel_portal_open()                                                          *
 *============================================================================*/

/**
 * @see _portal_open().
 */
PUBLIC int kernel_portal_open(int local, int remote)
{
	return (_portal_open(local, remote));
}

/*============================================================================*
 * kernel_portal_unlink()                                                        *
 *============================================================================*/

/**
 * @see _portal_unlink().
 */
PUBLIC int kernel_portal_unlink(int portalid)
{
	return (_portal_unlink(portalid));
}

/*============================================================================*
 * kernel_portal_close()                                                         *
 *============================================================================*/

/**
 * @see _portal_close().
 */
PUBLIC int kernel_portal_close(int portalid)
{
	return (_portal_close(portalid));
}

/*============================================================================*
 * kernel_portal_awrite()                                                        *
 *============================================================================*/

/**
 * @see _portal_awrite().
 */
PUBLIC int kernel_portal_awrite(int portalid, const void * buffer, size_t size)
{
	return (_portal_awrite(portalid, buffer, size));
}

/*============================================================================*
 * kernel_portal_aread()                                                          *
 *============================================================================*/

/**
 * @see _portal_aread().
 */
PUBLIC int kernel_portal_aread(int portalid, void * buffer, size_t size)
{
	return (_portal_aread(portalid, buffer, size));
}

/*============================================================================*
 * kernel_portal_wait()                                                          *
 *============================================================================*/

/**
 * @see _portal_wait().
 */
PUBLIC int kernel_portal_wait(int portalid)
{
	return (_portal_wait(portalid));
}

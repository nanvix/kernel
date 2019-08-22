/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
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

#include <nanvix/sync.h>
#include <nanvix/klib.h>
#include <errno.h>

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Table of portal points.
 */
PRIVATE struct portal
{
	struct resource resource; /**< Underlying resource.        */
	int refcount;             /**< References count.           */
	int fd;                   /**< Underlying file descriptor. */
	int local;                /**< Local num.                  */
	int remote;               /**< Target num.                 */
} ALIGN(sizeof(dword_t)) portaltab[(PORTAL_CREATE_MAX + PORTAL_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool portalpool = {
	portaltab, (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX), sizeof(struct portal)
};

/*============================================================================*
 * _portal_is_valid()                                                         *
 *============================================================================*/

/**
 * @brief Asserts whether or not a synchronization point is valid.
 *
 * @param portalid ID of the target synchronization point.
 *
 * @returns One if the target synchronization point is valid, and false
 * otherwise.
 *
 * @note This function is non-blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int _portal_is_valid(int portalid)
{
	return WITHIN(portalid, 0, (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX));
}

/*============================================================================*
 * _portal_allow()                                                            *
 *============================================================================*/

/**
 * @brief Creates a portal.
 *
 * @param local Logic ID of the Local Node.
 *
 * @returns Upon successful completion, the ID of a newly created
 * portal is returned. Upon failure, a negative error code is returned
 * instead.
 */
PUBLIC int _portal_create(int local)
{
	int fd;       /* File descriptor. */
	int portalid; /* Portal ID.       */

	/* Allocate a portal. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	if ((fd = portal_create(local)) < 0)
	{
		resource_free(&portalpool, portalid);
		return (fd);
	}

	/* Initialize portal. */
	portaltab[portalid].fd       = fd;
	portaltab[portalid].refcount = 1;
	portaltab[portalid].local    = local;
	portaltab[portalid].remote   = -1;
	resource_set_rdonly(&portaltab[portalid].resource);
	resource_set_notbusy(&portaltab[portalid].resource);

	dcache_invalidate();

	return (portalid);
}

/*============================================================================*
 * _portal_allow()                                                            *
 *============================================================================*/

/**
 * @brief Enables read operations from a remote.
 *
 * @param portalid Logic ID of the Target Portal.
 * @param remote   Logic ID of Target Node.
 *
 * @returns Upons successful completion zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
PUBLIC int _portal_allow(int portalid, int remote)
{
	int ret; /* HAL function return. */

	if (!_portal_is_valid(portalid))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EINVAL);

	/* Bad portal. */
	if (!resource_is_readable(&portaltab[portalid].resource))
		return (-EINVAL);

	if ((ret = portal_allow(portaltab[portalid].fd, remote)) < 0)
		return (ret);

	portaltab[portalid].remote = remote;

	return (0);
}


/*============================================================================*
 * _portal_open()                                                             *
 *============================================================================*/

/**
 * @brief Opens a portal.
 *
 * @param local  Logic ID of the Local Node.
 * @param remote Logic ID of the Target Node.
 *
 * @returns Upon successful completion, the ID of the target portal is
 * returned. Upon failure, a negative error code is returned instead.
 */
PUBLIC int _portal_open(int local, int remote)
{
	int fd;       /* File descriptor. */
	int portalid; /* Portal ID.       */

	/* Allocate a Portal. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	if ((fd = portal_open(local, remote)) < 0)
	{
		resource_free(&portalpool, portalid);
		return (fd);
	}

	portaltab[portalid].fd       = fd;
	portaltab[portalid].refcount = 1;
	portaltab[portalid].local    = local;
	portaltab[portalid].remote   = remote;
	resource_set_wronly(&portaltab[portalid].resource);
	resource_set_notbusy(&portaltab[portalid].resource);

	dcache_invalidate();

	return (portalid);
}

/*============================================================================*
 * _portal_unlink()                                                           *
 *============================================================================*/

/**
 * @brief Destroys a portal.
 *
 * @param portalid ID of the Target Portal.
 *
 * @returns Upon successful completion zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
PUBLIC int _portal_unlink(int portalid)
{
	int ret; /* HAL function return. */

	/* Invalid portal. */
	if (!_portal_is_valid(portalid))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EAGAIN);

	/* Bad portal. */
	if (!resource_is_readable(&portaltab[portalid].resource))
		return (-EAGAIN);

	if ((ret = portal_unlink(portaltab[portalid].fd)) < 0)
		return (ret);
	
	portaltab[portalid].fd = -1;
	resource_free(&portalpool, portalid);

	return (0);
}

/*============================================================================*
 * _portal_close()                                                            *
 *============================================================================*/

/**
 * @brief Closes a portal.
 *
 * @param portalid ID of the Target Portal.
 *
 * @returns Upon successful completion zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
PUBLIC int _portal_close(int portalid)
{
	int ret; /* HAL function return. */

	/* Invalid portal. */
	if (!_portal_is_valid(portalid))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EAGAIN);

	/* Bad portal. */
	if (!resource_is_writable(&portaltab[portalid].resource))
		return (-EAGAIN);

	/* Closes portal. */
	if ((ret = portal_close(portaltab[portalid].fd)) < 0)
		return (ret);

	portaltab[portalid].fd = -1;
	resource_free(&portalpool, portalid);

	return (0);
}

/*============================================================================*
 * _portal_aread()                                                            *
 *============================================================================*/

/**
 * @brief Reads data asynchronously from a portal.
 *
 * @param portalid ID of the Target Portal.
 * @param buffer   Location from where data should be written.
 * @param size     Number of bytes to read.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int _portal_aread(int portalid, void * buffer, size_t size)
{
	/* Invalid portal. */
	if (!_portal_is_valid(portalid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EAGAIN);

	/* Invalid read size. */
	if (size == 0 || size > PORTAL_MAX_SIZE)
		return (-EAGAIN);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EAGAIN);

	/* Bad portal. */
	if (!resource_is_readable(&portaltab[portalid].resource))
		return (-EAGAIN);

	/* Configures underlying async read. */
	return portal_aread(portaltab[portalid].fd, buffer, size);
}

/*============================================================================*
 * _portal_awrite()                                                            *
 *============================================================================*/

/**
 * @brief Writes data to a portal.
 *
 * @param portalid ID of the Target Portal.
 * @param buffer   Buffer where the data should be read from.
 * @param size     Number of bytes to write.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int _portal_awrite(int portalid, const void * buffer, size_t size)
{
	/* Invalid portal. */
	if (!_portal_is_valid(portalid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EAGAIN);

	/* Invalid write size. */
	if (size == 0 || size > PORTAL_MAX_SIZE)
		return (-EAGAIN);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EAGAIN);

	/* Bad portal. */
	if (!resource_is_writable(&portaltab[portalid].resource))
		return (-EAGAIN);

	/* Configures underlying async write. */
	return portal_awrite(portaltab[portalid].fd, buffer, size);
}

/*============================================================================*
 * _portal_wait()                                                            *
 *============================================================================*/

/**
 * @brief Waits for an asynchronous operation on a portal to complete.
 *
 * @param portalid ID of the Target Portal.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int _portal_wait(int portalid)
{
	/* Invalid portal. */
	if (!_portal_is_valid(portalid))
		return (-EBADF);

	dcache_invalidate();

	/* Waits. */
	return portal_wait(portaltab[portalid].fd);
}

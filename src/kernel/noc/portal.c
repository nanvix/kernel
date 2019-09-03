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
#include <nanvix/klib.h>
#include <posix/errno.h>

#if __TARGET_HAS_PORTAL

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
	int remote;               /**< Target node number.         */
} ALIGN(sizeof(dword_t)) portaltab[(PORTAL_CREATE_MAX + PORTAL_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool portalpool = {
	portaltab, (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX), sizeof(struct portal)
};

/*============================================================================*
 * do_portal_is_valid()                                                       *
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
PRIVATE int do_portal_is_valid(int portalid)
{
	return WITHIN(portalid, 0, (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX));
}

/*============================================================================*
 * do_portal_allow()                                                          *
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
PRIVATE int _do_portal_create(int local)
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
	portaltab[portalid].local    = local;
	portaltab[portalid].remote   = -1;
	portaltab[portalid].refcount = 1;

	resource_set_rdonly(&portaltab[portalid].resource);
	resource_set_notbusy(&portaltab[portalid].resource);

	return (portalid);
}

/**
 * @see _do_portal_create().
 */
PUBLIC int do_portal_create(int local)
{
	int portalid; /* Portal ID. */

	/* Invalid local ID. */
	if (!WITHIN(local, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Searchs for existing portals. */
	for (int i = 0; i < (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&portaltab[i].resource))
			continue;

		if (!resource_is_readable(&portaltab[i].resource))
			continue;

		/* Not the same local node? */
		if (portaltab[i].local != local)
			continue;

		/* Does it have a remote allowed? */
		if (portaltab[i].remote != -1)
			continue;

		portalid = i;
		portaltab[i].refcount++;

		goto found;
	}

	/* Alloc a new portal. */
	portalid = _do_portal_create(local);

found:
	dcache_invalidate();

	return (portalid);
}

/*============================================================================*
 * do_portal_allow()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_allow(int portalid, int remote)
{
	int ret; /* HAL function return. */

	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	/* Invalid remote ID. */
	if (!WITHIN(remote, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

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
 * do_portal_open()                                                           *
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
PRIVATE int _do_portal_open(int local, int remote)
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
	portaltab[portalid].local    = local;
	portaltab[portalid].remote   = remote;
	portaltab[portalid].refcount = 1;

	resource_set_wronly(&portaltab[portalid].resource);
	resource_set_notbusy(&portaltab[portalid].resource);

	return (portalid);
}

/**
 * @see _do_portal_open().
 */
PUBLIC int do_portal_open(int local, int remote)
{
	int portalid; /* Portal ID. */

	/* Invalid local ID. */
	if (!WITHIN(local, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Invalid remote ID. */
	if (!WITHIN(remote, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Searchs for existing portals. */
	for (int i = 0; i < (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&portaltab[i].resource))
			continue;

		if (!resource_is_writable(&portaltab[i].resource))
			continue;

		/* Not the same local node? */
		if (portaltab[i].local != local)
			continue;

		/* Does it have a remote allowed? */
		if (portaltab[i].remote != remote)
			continue;

		portalid = i;
		portaltab[i].refcount++;

		goto found;
	}

	/* Alloc a new portal. */
	portalid = _do_portal_open(local, remote);

found:
	dcache_invalidate();

	return (portalid);
}

/*============================================================================*
 * _do_portal_release()                                                       *
 *============================================================================*/

/**
 * @brief Relase a portal resource.
 *
 * @param portalid   ID of the target portal.
 * @param release_fn Underlying release function.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PRIVATE int _do_portal_release(int portalid, int (*release_fn)(int))
{
	int ret; /* HAL function return. */

	portaltab[portalid].refcount--;

	if (portaltab[portalid].refcount == 0)
	{
		if ((ret = release_fn(portaltab[portalid].fd)) < 0)
			return (ret);

		portaltab[portalid].fd     = -1;
		portaltab[portalid].local  = -1;
		portaltab[portalid].remote = -1;

		resource_free(&portalpool, portalid);

		dcache_invalidate();
	}

	return (0);
}

/*============================================================================*
 * do_portal_unlink()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_unlink(int portalid)
{
	/* Invalid portal. */
	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&portaltab[portalid].resource))
		return (-EBADF);

	/* Release resource. */
	return (_do_portal_release(portalid, portal_unlink));
}

/*============================================================================*
 * do_portal_close()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_close(int portalid)
{
	/* Invalid portal. */
	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_writable(&portaltab[portalid].resource))
		return (-EBADF);

	/* Release resource. */
	return (_do_portal_release(portalid, portal_close));
}

/*============================================================================*
 * do_portal_aread()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_aread(int portalid, void * buffer, size_t size)
{
	/* Invalid portal. */
	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Invalid read size. */
	if (size == 0 || size > PORTAL_MAX_SIZE)
		return (-EINVAL);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&portaltab[portalid].resource))
		return (-EBADF);

	/* Configures underlying async read. */
	return (portal_aread(portaltab[portalid].fd, buffer, size));
}

/*============================================================================*
 * do_portal_awrite()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_awrite(int portalid, const void * buffer, size_t size)
{
	/* Invalid portal. */
	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Invalid write size. */
	if (size == 0 || size > PORTAL_MAX_SIZE)
		return (-EINVAL);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_writable(&portaltab[portalid].resource))
		return (-EBADF);

	/* Configures underlying async write. */
	return (portal_awrite(portaltab[portalid].fd, buffer, size));
}

/*============================================================================*
 * do_portal_wait()                                                           *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_wait(int portalid)
{
	/* Invalid portal. */
	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	dcache_invalidate();

	/* Waits. */
	return (portal_wait(portaltab[portalid].fd));
}

#endif /* __TARGET_HAS_PORTAL */

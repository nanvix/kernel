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
	/* Control variables */
	struct resource resource; /**< Underlying resource.        */
	int refcount;             /**< References count.           */
	int fd;                   /**< Underlying file descriptor. */
	int local;                /**< Local num.                  */
	int remote;               /**< Target node number.         */

	/* Experiments variables */
	size_t volume;            /**< Amount of data transferred. */
	uint64_t latency;         /**< Transfer latency.           */
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
	portaltab[portalid].volume   = 0ULL;
	portaltab[portalid].latency  = 0ULL;

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
	portaltab[portalid].volume   = 0ULL;
	portaltab[portalid].latency  = 0ULL;

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
	int ret;     /* HAL function return.           */
	uint64_t t1; /* Clock value before aread call. */
	uint64_t t2; /* Clock value after aread call.  */

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

	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_aread(portaltab[portalid].fd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Updates latency variable. */
	portaltab[portalid].latency += (t2 - t1);

	/* Updates volume variable. */
	portaltab[portalid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_portal_awrite()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_awrite(int portalid, const void * buffer, size_t size)
{
	int ret;     /* HAL function return.            */
	uint64_t t1; /* Clock value before awrite call. */
	uint64_t t2; /* Clock value after awrite call.  */

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

	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_awrite(portaltab[portalid].fd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Updates latency variable. */
	portaltab[portalid].latency += (t2 - t1);

	/* Updates volume variable. */
	portaltab[portalid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_portal_wait()                                                           *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_portal_wait(int portalid)
{
	int ret;     /* HAL function return.          */
	uint64_t t1; /* Clock value before wait call. */
	uint64_t t2; /* Clock value after wait call.  */

	/* Invalid portal. */
	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	dcache_invalidate();

	t1 = clock_read();

		ret = portal_wait(portaltab[portalid].fd);

	t2 = clock_read();

	/* Updates latency variable. */
	portaltab[portalid].latency += (t2 - t1);

	return (ret);
}

/*============================================================================*
 * do_portal_ioctl()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
int do_portal_ioctl(int portalid, unsigned request, va_list args)
{
	int ret = 0;

	/* Invalid portal. */
	if (!do_portal_is_valid(portalid))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&portaltab[portalid].resource))
		return (-EBADF);

	/* Server request. */
	switch (request)
	{
		/* Get the amount of data transfered so far. */
		case PORTAL_IOCTL_GET_VOLUME:
		{
			size_t *volume;
			volume = va_arg(args, size_t *);
			*volume = portaltab[portalid].volume;
		} break;

		/* Get the cummulative transfer latency. */
		case PORTAL_IOCTL_GET_LATENCY:
		{
			uint64_t *latency;
			latency = va_arg(args, uint64_t *);
			*latency = portaltab[portalid].latency;
		} break;

		/* Operation not supported. */
		default:
			ret = (-ENOTSUP);
			break;
	}

	return (ret);
}

#endif /* __TARGET_HAS_PORTAL */

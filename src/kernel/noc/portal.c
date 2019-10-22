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
#include <nanvix/hlib.h>
#include <posix/errno.h>

#if __TARGET_HAS_PORTAL

/**
 * @brief Search types for do_portal_search().
 */
enum portal_search_type {
	PORTAL_SEARCH_INPUT = 0,
	PORTAL_SEARCH_OUTPUT = 1
} resource_type_enum_t;

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Table of virtual portals.
 */
PRIVATE struct
{
	/**
	 * @name Control Variables
	 */
	/**@{*/
	int fd;      /**< Index to table of active portals.    */
	int allowed; /**< Input portal allows remote to write? */
	/**@}*/

	/**
	 * @name Performance Statistics
	 */
	/**@{*/
	size_t volume;    /**< Amount of data transferred. */
	uint64_t latency; /**< Transfer latency.           */
	/**@}*/
} ALIGN(sizeof(dword_t)) virtual_portals[KPORTAL_MAX] = {
	[0 ... (KPORTAL_MAX - 1)] = {
		.fd = -1,
		.allowed = 0
	},
};

/**
 * @brief Table of active portals.
 */
PRIVATE struct portal
{
	struct resource resource; /**< Underlying resource.        */
	int refcount;             /**< References count.           */
	int hwfd;                 /**< Underlying file descriptor. */
	int local;                /**< Local node number.          */
	int remote;               /**< Target node number.         */
} active_portals[(PORTAL_CREATE_MAX + PORTAL_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool portalpool = {
	active_portals, (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX), sizeof(struct portal)
};

/*============================================================================*
 * do_vportal_is_valid()                                                      *
 *============================================================================*/

/**
 * @brief Asserts whether or not a synchronization point is valid.
 *
 * @param portalid ID of the target virtual portal.
 *
 * @returns One if the target synchronization point is valid, and false
 * otherwise.
 *
 * @note This function is non-blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int do_vportal_is_valid(int portalid)
{
	return WITHIN(portalid, 0, (KPORTAL_MAX));
}

/*============================================================================*
 * do_vportal_alloc()                                                         *
 *============================================================================*/

/**
 * @brief Searches for a free virtual portal.
 *
 * @returns Upon successful completion, the index of the virtual
 * portal found is returned. Upon failure, a negative number is
 * returned instead.
 */
PRIVATE int do_vportal_alloc(void)
{
	for (int i = 0; i < KPORTAL_MAX; ++i)
	{
		/* Found. */
		if (virtual_portals[i].fd < 0)
			return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_portal_search()                                                         *
 *============================================================================*/

/**
 * @name Helper Macros for do_portal_search()
 */
/**@{*/

/**
 * @brief Asserts an input portal.
 */
#define PORTAL_SEARCH_IS_INPUT(portalid,type) \
	((type == PORTAL_SEARCH_INPUT) && !resource_is_readable(&active_portals[portalid].resource))

/**
 * @brief Asserts an output portal.
 */
#define PORTAL_SEARCH_IS_OUTPUT(portalid,type) \
	 ((type == PORTAL_SEARCH_OUTPUT) && !resource_is_writable(&active_portals[portalid].resource))
/**@}*/

/**
 * @brief Searches for a hardware portal in active_portals table.
 *
 * @param local       Logic ID of the local node.
 * @param remote      Logic ID of the remote node.
 * @param search_type Type of the searched resource.
 *
 * @returns Upon successful completion, the ID of the found portal is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_portal_search(int local, int remote, enum portal_search_type search_type)
{
	for (unsigned i = 0; i < (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&active_portals[i].resource))
			continue;

		if (PORTAL_SEARCH_IS_INPUT(i, search_type))
			continue;

		else if (PORTAL_SEARCH_IS_OUTPUT(i, search_type))
			continue;

		/* Not the same local node? */
		if (active_portals[i].local != local)
			continue;

		/* Does it have a remote allowed? */
		if (active_portals[i].remote != remote)
			continue;

		return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_vportal_create()                                                        *
 *============================================================================*/

/**
 * @brief Creates a hardware portal.
 *
 * @param local Logic ID of the Local Node.
 *
 * @returns Upon successful completion, the ID of a newly created hardware
 * portal is returned. Upon failure, a negative error code is returned
 * instead.
 */
PRIVATE int _do_portal_create(int local)
{
	int hwfd;     /* File descriptor. */
	int portalid; /* Portal ID.       */

	/* Search target hardware portal. */
	if ((portalid = do_portal_search(local, -1, PORTAL_SEARCH_INPUT)) >= 0)
		return (portalid);

	/* Allocate resource. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	if ((hwfd = portal_create(local)) < 0)
	{
		resource_free(&portalpool, portalid);
		return (hwfd);
	}

	/* Initialize portal. */
	active_portals[portalid].hwfd     = hwfd;
	active_portals[portalid].local    = local;
	active_portals[portalid].remote   = -1;
	active_portals[portalid].refcount = 0;
	resource_set_rdonly(&active_portals[portalid].resource);
	resource_set_notbusy(&active_portals[portalid].resource);

	return (portalid);
}

/**
 * @brief Creates a virtual portal.
 *
 * @param local Logic ID of the Local Node.
 *
 * @returns Upon successful completion, the ID of a newly created virtual
 * portal is returned. Upon failure, a negative error code is returned
 * instead.
 */
PUBLIC int do_vportal_create(int local)
{
	int portalid;  /* Hardware portal ID. */
	int vportalid; /* Virtual portal ID.  */

	/* Allocate a virtual portal. */
	if ((vportalid = do_vportal_alloc()) < 0)
		return (-EAGAIN);

	/* Creates a hardware portal. */
	if ((portalid = _do_portal_create(local)) < 0)
		return (portalid);

	/* Initialize the new virtual portal. */
	virtual_portals[vportalid].fd      = portalid;
	virtual_portals[vportalid].volume  = 0ULL;
	virtual_portals[vportalid].latency = 0ULL;
	active_portals[portalid].refcount++;

	dcache_invalidate();
	return (vportalid);
}

/*============================================================================*
 * do_vportal_allow()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vportal_allow(int portalid, int remote)
{
	int ret; /* HAL function return.    */
	int fd;  /* Active portal logic ID. */

	if (!do_vportal_is_valid(portalid))
		return (-EINVAL);

	fd = virtual_portals[portalid].fd;

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	/* Read operation is already ongoing on this HW portal. */
	if (active_portals[fd].remote != -1)
		return (-EBUSY);

	if ((ret = portal_allow(active_portals[fd].hwfd, remote)) < 0)
		return (ret);

	virtual_portals[portalid].allowed = 1;
	active_portals[fd].remote = remote;

	return (0);
}

/*============================================================================*
 * do_vportal_open()                                                          *
 *============================================================================*/

/**
 * @brief Opens a hardware portal.
 *
 * @param local  Logic ID of the Local Node.
 * @param remote Logic ID of the Target Node.
 *
 * @returns Upon successful completion, the ID of the target portal is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int _do_portal_open(int local, int remote)
{
	int hwfd;     /* File descriptor. */
	int portalid; /* Portal ID.       */

	/* Search target hardware portal. */
	if ((portalid = do_portal_search(local, remote, PORTAL_SEARCH_OUTPUT)) >= 0)
		return (portalid);

	/* Allocate a Portal. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	if ((hwfd = portal_open(local, remote)) < 0)
	{
		resource_free(&portalpool, portalid);
		return (hwfd);
	}

	active_portals[portalid].hwfd     = hwfd;
	active_portals[portalid].local    = local;
	active_portals[portalid].remote   = remote;
	active_portals[portalid].refcount = 0;
	resource_set_wronly(&active_portals[portalid].resource);
	resource_set_notbusy(&active_portals[portalid].resource);

	return (portalid);
}

/**
 * @brief Opens a virtual portal.
 *
 * @param local  Logic ID of the Local Node.
 * @param remote Logic ID of the Target Node.
 *
 * @returns Upon successful completion, the ID of the newly opened virtual
 * portal is returned. Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_open(int local, int remote)
{
	int portalid;  /* Hardware portal ID. */
	int vportalid; /* Virtual portal ID.  */

	/* Allocate a virtual portal. */
	if ((vportalid = do_vportal_alloc()) < 0)
		return (-EAGAIN);

	/* Opens a hardware portal. */
	if ((portalid = _do_portal_open(local, remote)) < 0)
		return (portalid);

	/* Initialize the new virtual portal. */
	virtual_portals[vportalid].fd      = portalid;
	virtual_portals[vportalid].volume  = 0ULL;
	virtual_portals[vportalid].latency = 0ULL;
	active_portals[portalid].refcount++;

	dcache_invalidate();
	return (vportalid);
}

/*============================================================================*
 * _do_portal_release()                                                       *
 *============================================================================*/

/**
 * @brief Releases a hardware portal.
 *
 * @param portalid   ID of the target hardware portal.
 * @param release_fn Underlying release function.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PRIVATE int _do_portal_release(int portalid, int (*release_fn)(int))
{
	int ret; /* HAL function return. */

	if ((ret = release_fn(active_portals[portalid].hwfd)) < 0)
		return (ret);

	active_portals[portalid].hwfd   = -1;
	active_portals[portalid].local  = -1;
	active_portals[portalid].remote = -1;
	resource_free(&portalpool, portalid);

	dcache_invalidate();
	return (0);
}

/*============================================================================*
 * do_vportal_unlink()                                                        *
 *============================================================================*/

/**
 * @brief Unlinks a created (input) virtual portal.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_unlink(int portalid)
{
	int fd; /* Active portal logic ID. */

	/* Invalid portal. */
	if (!do_vportal_is_valid(portalid))
		return (-EINVAL);

	fd = virtual_portals[portalid].fd;

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	/* Unlink hardware portal. */
	virtual_portals[portalid].fd = -1;
	virtual_portals[portalid].allowed = 0;
	active_portals[fd].refcount--;

	/* Release underlying hardware portal. */
	if (active_portals[fd].refcount == 0)
		return (_do_portal_release(fd, portal_unlink));

	return (0);
}

/*============================================================================*
 * do_vportal_close()                                                         *
 *============================================================================*/

/**
 * @brief Closes an opened (output) virtual portal.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_close(int portalid)
{
	int fd; /* Active portal logic ID. */

	/* Invalid portal. */
	if (!do_vportal_is_valid(portalid))
		return (-EINVAL);

	fd = virtual_portals[portalid].fd;

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_writable(&active_portals[fd].resource))
		return (-EBADF);

	/* Close hardware portal. */
	virtual_portals[portalid].fd = -1;
	active_portals[fd].refcount--;

	/* Release underlying hardware portal. */
	if (active_portals[fd].refcount == 0)
		return (_do_portal_release(fd, portal_close));

	return (0);
}

/*============================================================================*
 * do_vportal_aread()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vportal_aread(int portalid, void * buffer, size_t size)
{
	int ret;     /* HAL function return.           */
	int fd;      /* Hardware portal logic index.   */
	uint64_t t1; /* Clock value before aread call. */
	uint64_t t2; /* Clock value after aread call.  */

	/* Invalid portal. */
	if (!do_vportal_is_valid(portalid))
		return (-EINVAL);

	fd = virtual_portals[portalid].fd;

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	/* Unallowed operation. */
	if (!virtual_portals[portalid].allowed)
		return (-EACCES);

	resource_set_async(&active_portals[fd].resource);

	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_aread(active_portals[fd].hwfd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Revoke allow. */
	virtual_portals[portalid].allowed = 0;
	active_portals[fd].remote = -1;

	/* Updates performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);
	virtual_portals[portalid].volume  += ret;

	return (ret);
}

/*============================================================================*
 * do_vportal_awrite()                                                        *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vportal_awrite(int portalid, const void * buffer, size_t size)
{
	int ret;     /* HAL function return.            */
	int fd;      /* Hardware portal logic index.    */
	uint64_t t1; /* Clock value before awrite call. */
	uint64_t t2; /* Clock value after awrite call.  */

	/* Invalid portal. */
	if (!do_vportal_is_valid(portalid))
		return (-EINVAL);

	fd = virtual_portals[portalid].fd;

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_writable(&active_portals[fd].resource))
		return (-EBADF);

	resource_set_async(&active_portals[fd].resource);

	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_awrite(active_portals[fd].hwfd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Update performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);
	virtual_portals[portalid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_vportal_wait()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vportal_wait(int portalid)
{
	int ret;     /* HAL function return.          */
	int fd;      /* Hardware portal logic index.  */
	uint64_t t1; /* Clock value before wait call. */
	uint64_t t2; /* Clock value after wait call.  */

	/* Invalid portal. */
	if (!do_vportal_is_valid(portalid))
		return (-EINVAL);

	fd = virtual_portals[portalid].fd;

	/* Bad virtual portal. */
	if (!resource_is_async(&active_portals[fd].resource))
		return (-EBADF);

	dcache_invalidate();

	t1 = clock_read();

		/* Wait for asynchronous operation. */
		ret = portal_wait(active_portals[fd].hwfd);

	t2 = clock_read();

	/* Updates performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);

	return (ret);
}

/*============================================================================*
 * do_vportal_ioctl()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
int do_vportal_ioctl(int portalid, unsigned request, va_list args)
{
	int ret = 0;

	/* Invalid portal. */
	if (!do_vportal_is_valid(portalid))
		return (-EINVAL);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[virtual_portals[portalid].fd].resource))
		return (-EBADF);

	/* Server request. */
	switch (request)
	{
		/* Get the amount of data transfered so far. */
		case PORTAL_IOCTL_GET_VOLUME:
		{
			size_t *volume;
			volume = va_arg(args, size_t *);
			*volume = virtual_portals[portalid].volume;
		} break;

		/* Get the cummulative transfer latency. */
		case PORTAL_IOCTL_GET_LATENCY:
		{
			uint64_t *latency;
			latency = va_arg(args, uint64_t *);
			*latency = virtual_portals[portalid].latency;
		} break;

		/* Operation not supported. */
		default:
			ret = (-ENOTSUP);
			break;
	}

	return (ret);
}

#endif /* __TARGET_HAS_PORTAL */

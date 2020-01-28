/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

#include <nanvix/hal.h>
#include <nanvix/kernel/sync.h>
#include <nanvix/hlib.h>
#include <posix/errno.h>

#if __TARGET_HAS_SYNC

/**
 * @brief Search types for do_sync_search().
 */
enum sync_search_type {
	SYNC_SEARCH_INPUT = 0,
	SYNC_SEARCH_OUTPUT = 1
} resource_type_enum_t;

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Table of virtual synchronization points.
 */
PRIVATE struct
{
	int fd; /**< Index to table of active syncs. */
} ALIGN(sizeof(dword_t)) virtual_syncs[KSYNC_MAX] = {
	[0 ... (KSYNC_MAX - 1)] = { .fd = -1 },
};

/**
 * @brief Table of active synchronization points.
 */
PRIVATE struct sync
{
	struct resource resource; /**< Underlying resource.        */
	int refcount;             /**< References count.           */
	int hwfd;                 /**< Underlying file descriptor. */
	int masternum;            /**< Node number of the ONE.     */
	int type;                 /**< Sync type.                  */
	uint64_t nodeslist;       /**< Node ID list.               */
} active_syncs[(SYNC_CREATE_MAX + SYNC_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool syncpool = {
	active_syncs, (SYNC_CREATE_MAX + SYNC_OPEN_MAX), sizeof(struct sync)
};

/*============================================================================*
 * do_vsync_is_valid()                                                         *
 *============================================================================*/

/**
 * @brief Asserts whether or not a virtual synchronization point is valid.
 *
 * @param syncid ID of the target virtual synchronization point.
 *
 * @returns One if the virtual synchronization point is valid, and false
 * otherwise.
 *
 * @note This function is non-blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int do_vsync_is_valid(int syncid)
{
	return WITHIN(syncid, 0, KSYNC_MAX);
}

/*============================================================================*
 * do_sync_search()                                                        *
 *============================================================================*/

/**
 * @name Helper Macros for do_sync_search()
 */
/**@{*/

/**
 * @brief Asserts an input sync.
 */
#define SYNC_SEARCH_IS_INPUT(syncid,type) \
	((type == SYNC_SEARCH_INPUT) && !resource_is_readable(&active_syncs[syncid].resource))

/**
 * @brief Asserts an output sync.
 */
#define SYNC_SEARCH_IS_OUTPUT(syncid,type) \
	 ((type == SYNC_SEARCH_OUTPUT) && !resource_is_writable(&active_syncs[syncid].resource))
/**@}*/

/**
 * @brief Searches for a sync.
 *
 * Searches for an already existing sync in active_syncs.
 *
 * @param masternum    Logic ID of the master node.
 * @param nodeslist    Target nodes list.
 * @param sync_type    Type of synchronization type.
 * @param search_type  Type of the searched resource.
 *
 * @returns Upon successful completion, the ID of the sync found is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_sync_search(int masternum, uint64_t nodeslist, int sync_type, enum sync_search_type search_type)
{
	for (int i = 0; i < (SYNC_CREATE_MAX + SYNC_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&active_syncs[i].resource))
			continue;

		if (SYNC_SEARCH_IS_INPUT(i, search_type))
			continue;

		else if (SYNC_SEARCH_IS_OUTPUT(i, search_type))
			continue;

		/* Not the same master? */
		if (active_syncs[i].masternum != masternum)
			continue;

		/* Not the same node list? */
		if (active_syncs[i].nodeslist != nodeslist)
			continue;

		/* Not the same type operation? */
		if (active_syncs[i].type != sync_type)
			continue;

		return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_vsync_alloc()                                                        *
 *============================================================================*/

/**
 * @brief Searches for a free virtual synchronization point.
 *
 * @returns Upon successful completion, the index of the virtual sync found
 * is returned. Upon failure, a negative number is returned instead.
 */
PRIVATE int do_vsync_alloc(void)
{
	for (int i = 0; i < KSYNC_MAX; ++i)
	{
		/* Found. */
		if (virtual_syncs[i].fd < 0)
			return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_vsync_create()                                                           *
 *============================================================================*/

/**
 * @brief Creates a hardware synchronization point.
 *
 * @param nodes     Logic IDs of target nodes.
 * @param nnodes    Number of target nodes.
 * @param type      Type of synchronization point.
 * @param nodeslist Target nodes footprint.
 *
 * @returns Upon successful completion, the ID of the newly created
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 */
PRIVATE int _do_sync_create(const int *nodes, int nnodes, int type, uint64_t nodeslist)
{
	int hwfd;   /* File descriptor.       */
	int syncid; /* Synchronization point. */

	/* Search target hardware synchronization point. */
	if ((syncid = do_sync_search(nodes[0], nodeslist, type, SYNC_SEARCH_INPUT)) >= 0)
		return (syncid);

	/* Allocate a synchronization point. */
	if ((syncid = resource_alloc(&syncpool)) < 0)
		return (-EAGAIN);

	if ((hwfd = sync_create(nodes, nnodes, type)) < 0)
	{
		resource_free(&syncpool, syncid);
		return (hwfd);
	}

	/* Initialize synchronization point. */
	active_syncs[syncid].hwfd      = hwfd;
	active_syncs[syncid].type      = type;
	active_syncs[syncid].refcount  = 0;
	active_syncs[syncid].masternum = nodes[0];
	active_syncs[syncid].nodeslist = nodeslist;

	resource_set_rdonly(&active_syncs[syncid].resource);
	resource_set_notbusy(&active_syncs[syncid].resource);

	return (syncid);
}

/**
 * @brief Creates a virtual synchronization point.
 *
 * @param nodes     Logic IDs of target nodes.
 * @param nnodes    Number of target nodes.
 * @param type      Type of synchronization point.
 * @param nodeslist Target nodes list.
 *
 * @returns Upon successful completion, the ID of the newly created
 * virtual synchronization point is returned. Upon failure, a negative
 * error code is returned instead.
 */
PUBLIC int do_vsync_create(const int *nodes, int nnodes, int type)
{
	int syncid;         /* HW sync point ID.      */
	int vsyncid;        /* Virtual sync point ID. */
	uint64_t nodeslist; /* Target nodes list.     */

	/* Allocate a virtual synchronization point. */
	if ((vsyncid = do_vsync_alloc()) < 0)
		return (-EAGAIN);

	nodeslist = 0ULL;
	for (int j = 0; j < nnodes; j++)
		nodeslist |= (1ULL << nodes[j]);

	/* Create a synchronization point. */
	if ((syncid = _do_sync_create(nodes, nnodes, type, nodeslist)) < 0)
		return (syncid);

	/* Initialize virtual synchronization point. */
	virtual_syncs[vsyncid].fd = syncid;
	active_syncs[syncid].refcount++;

	dcache_invalidate();
	return (vsyncid);
}

/*============================================================================*
 * do_vsync_open()                                                             *
 *============================================================================*/

/**
 * @brief Opens a hardware synchronization point.
 *
 * @param nodes     Logic IDs of target nodes.
 * @param nnodes    Number of target nodes.
 * @param type      Type of synchronization point.
 * @param nodeslist Target nodes list.
 *
 * @returns Upon successful completion, the ID of the opened
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 *
 * @todo Check for Invalid Remote
 */
PRIVATE int _do_sync_open(const int *nodes, int nnodes, int type, uint64_t nodeslist)
{
	int hwfd;   /* File descriptor.       */
	int syncid; /* Synchronization point. */

	/* Search target hardware synchronization point. */
	if ((syncid = do_sync_search(nodes[0], nodeslist, type, SYNC_SEARCH_OUTPUT)) >= 0)
		return (syncid);

	/* Allocate a synchronization point. */
	if ((syncid = resource_alloc(&syncpool)) < 0)
		return (-EAGAIN);

	/* Open connector. */
	if ((hwfd = sync_open(nodes, nnodes, type)) < 0)
	{
		resource_free(&syncpool, syncid);
		return (hwfd);
	}

	/* Initialize synchronization point. */
	active_syncs[syncid].hwfd      = hwfd;
	active_syncs[syncid].type      = type;
	active_syncs[syncid].refcount  = 0;
	active_syncs[syncid].masternum = nodes[0];
	active_syncs[syncid].nodeslist = nodeslist;

	resource_set_wronly(&active_syncs[syncid].resource);
	resource_set_notbusy(&active_syncs[syncid].resource);

	return (syncid);
}

/**
 * @brief Opens a virtual synchronization point.
 *
 * @param nodes     Logic IDs of target nodes.
 * @param nnodes    Number of target nodes.
 * @param type      Type of synchronization point.
 * @param nodeslist Target nodes list.
 *
 * @returns Upon successful completion, the ID of the opened virtual
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 *
 * @todo Check for Invalid Remote
 */
PUBLIC int do_vsync_open(const int *nodes, int nnodes, int type)
{
	int syncid;         /* HW sync point ID.      */
	int vsyncid;        /* Virtual sync point ID. */
	uint64_t nodeslist; /* Target nodes list.      */

	/* Allocate a virtual synchronization point. */
	if ((vsyncid = do_vsync_alloc()) < 0)
		return (-EAGAIN);

	nodeslist = 0ULL;
	for (int j = 0; j < nnodes; j++)
		nodeslist |= (1ULL << nodes[j]);

	/* Create a synchronization point. */
	if ((syncid = _do_sync_open(nodes, nnodes, type, nodeslist)) < 0)
		return (syncid);

	/* Initialize virtual synchronization point. */
	virtual_syncs[vsyncid].fd = syncid;
	active_syncs[syncid].refcount++;

	dcache_invalidate();
	return (vsyncid);
}

/*============================================================================*
 * _do_sync_release()                                                         *
 *============================================================================*/

/**
 * @brief Release a synchronization resource.
 *
 * @param syncid     ID of the target synchronization point.
 * @param release_fn Underlying release function.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PRIVATE int _do_sync_release(int syncid, int (*release_fn)(int))
{
	int ret; /* HAL function return. */

	if ((ret = release_fn(active_syncs[syncid].hwfd)) < 0)
		return (ret);

	active_syncs[syncid].hwfd      = -1;
	active_syncs[syncid].masternum = -1;
	active_syncs[syncid].nodeslist = 0ULL;

	resource_free(&syncpool, syncid);

	dcache_invalidate();
	return (0);
}

/*============================================================================*
 * do_vsync_unlink()                                                           *
 *============================================================================*/

/**
 * @brief Unlinks a created virtual synchronization point.
 *
 * @param syncid Logic ID of the target virtual synchronization point.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vsync_unlink(int syncid)
{
	int fd; /* Active_syncs table index. */

	/* Invalid sync. */
	if (!do_vsync_is_valid(syncid))
		return (-EINVAL);

	fd = virtual_syncs[syncid].fd;

	/* Bad sync. */
	if (!resource_is_used(&active_syncs[fd].resource))
		return (-EBADF);

	/* Bad sync. */
	if (!resource_is_readable(&active_syncs[fd].resource))
		return (-EBADF);

	/* Unlink virtual synchronization point. */
	virtual_syncs[syncid].fd = -1;
	active_syncs[fd].refcount--;

	/* Release underlying resource. */
	if (active_syncs[fd].refcount == 0)
		return (_do_sync_release(fd, sync_unlink));

	return (0);
}

/*============================================================================*
 * do_vsync_close()                                                            *
 *============================================================================*/

/**
 * @brief Closes an opened virtual synchronization point.
 *
 * @param syncid Logic ID of the target virtual synchronization point.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vsync_close(int syncid)
{
	int fd; /* Active_syncs table index. */

	/* Invalid sync. */
	if (!do_vsync_is_valid(syncid))
		return (-EINVAL);

	fd = virtual_syncs[syncid].fd;

	/* Bad sync. */
	if (!resource_is_used(&active_syncs[fd].resource))
		return (-EBADF);

	/* Bad sync. */
	if (!resource_is_writable(&active_syncs[fd].resource))
		return (-EBADF);

	/* Close virtual synchronization point. */
	virtual_syncs[syncid].fd = -1;
	active_syncs[fd].refcount--;

	/* Release underlying resource. */
	if (active_syncs[fd].refcount == 0)
		return (_do_sync_release(fd, sync_close));

	return (0);
}

/*============================================================================*
 * do_vsync_wait()                                                             *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vsync_wait(int syncid)
{
	int fd; /* Active_syncs table index. */

	/* Invalid sync. */
	if (!do_vsync_is_valid(syncid))
		return (-EINVAL);

	fd = virtual_syncs[syncid].fd;

	dcache_invalidate();

	/* Bad sync. */
	if (!resource_is_used(&active_syncs[fd].resource))
		return (-EBADF);

	/* Bad sync. */
	if (!resource_is_readable(&active_syncs[fd].resource))
		return (-EBADF);

	/* Waits. */
	return (sync_wait(active_syncs[fd].hwfd));
}

/*============================================================================*
 * do_vsync_signal()                                                           *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vsync_signal(int syncid)
{
	int fd; /* Active_syncs table index. */

	/* Invalid sync. */
	if (!do_vsync_is_valid(syncid))
		return (-EINVAL);

	fd = virtual_syncs[syncid].fd;

	dcache_invalidate();

	/* Bad sync. */
	if (!resource_is_used(&active_syncs[fd].resource))
		return (-EBADF);

	/* Bad sync. */
	if (!resource_is_writable(&active_syncs[fd].resource))
		return (-EBADF);

	/* Sends signal. */
	return (sync_signal(active_syncs[fd].hwfd));
}

#endif /* __TARGET_SYNC */

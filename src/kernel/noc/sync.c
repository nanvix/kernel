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
 * @brief Table of synchronization points.
 */
PRIVATE struct sync
{
	struct create
	{
		struct resource resource; /**< Underlying resource.        */
		int refcount;             /**< References count.           */
		int fd;                   /**< Underlying file descriptor. */
		int type;                 /**< Sync type.                  */
		int nodenum;              /**< Relative sync point.        */
	} ALIGN(sizeof(dword_t)) creates[SYNC_CREATE_MAX];

	struct open
	{
		struct resource resource; /**< Underlying resource.        */
		int refcount;             /**< References count.           */
		int fd;                   /**< Underlying file descriptor. */
		int type;                 /**< Sync's type.                */
		uint64_t nodes;           /**< Target node IDs.            */
	} ALIGN(sizeof(dword_t)) opens[SYNC_OPEN_MAX];
} synctab;

/**
 * @brief Resource pool.
 */
PRIVATE struct syncpools
{
	const struct resource_pool create_pools;
	const struct resource_pool open_pools;
} ALIGN(sizeof(dword_t)) syncpools = {
	.create_pools = {synctab.creates, SYNC_CREATE_MAX, sizeof(struct create)},
	.open_pools   = {synctab.opens,   SYNC_OPEN_MAX,   sizeof(struct open)  },
};

/*============================================================================*
 * _sync_is_valid()                                                           *
 *============================================================================*/

/**
 * @brief Asserts whether or not a synchronization point is valid.
 *
 * @param syncid ID of the target synchronization point.
 *
 * @returns One if the target synchronization point is valid, and false
 * otherwise.
 *
 * @note This function is non-blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int _sync_is_valid(int syncid, int limit)
{
	return WITHIN(syncid, 0, limit);
}

/*============================================================================*
 * _sync_create()                                                             *
 *============================================================================*/

/**
 * @brief Creates a synchronization point.
 *
 * @param nodes  Logic IDs of Target Nodes.
 * @param nnodes Number of Target Nodes.
 * @param type   Type of synchronization point.
 *
 * @returns Upon successful completion, the ID of the newly created
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 */
PUBLIC int _sync_create(const int *nodes, int nnodes, int type)
{
	int fd;		/* File descriptor.       */
	int syncid; /* Synchronization point. */

	for (int i = 0; i < SYNC_CREATE_MAX; i++)
	{
		if (!resource_is_used(&synctab.creates[i].resource))
			continue;

		if (synctab.creates[i].nodenum != nodes[0])
			continue;

		/* @todo: Is this check correctly and necessary? */
		if (synctab.creates[i].type != type)
			return (-EAGAIN);

		syncid = i;
		synctab.creates[i].refcount++;

		goto found;
	}

	/* Allocate a synchronization point. */
	if ((syncid = resource_alloc(&syncpools.create_pools)) < 0)
		return (-EAGAIN);

	if ((fd = sync_create(nodes, nnodes, type)) < 0)
	{
		resource_free(&syncpools.create_pools, syncid);
		return (fd);
	}

	/* Initialize synchronization point. */
	synctab.creates[syncid].fd       = fd;
	synctab.creates[syncid].type     = type;
	synctab.creates[syncid].refcount = 1;
	synctab.creates[syncid].nodenum  = nodes[0];
	resource_set_rdonly(&synctab.creates[syncid].resource);
	resource_set_notbusy(&synctab.creates[syncid].resource);

	dcache_invalidate();

found:
	return (syncid);
}

/*============================================================================*
 * _sync_open()                                                               *
 *============================================================================*/

/**
 * @brief Opens a synchronization point.
 *
 * @param nodes  Logic IDs of Target Nodes.
 * @param nnodes Number of Target Nodes.
 * @param type   Type of synchronization point.
 *
 * @returns Upon successful completion, the ID of the target
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 *
 * @todo Check for Invalid Remote
 */
PUBLIC int _sync_open(const int *nodes, int nnodes, int type)
{
	int fd;		     /* File descriptor.        */
	int syncid;      /* Synchronization point.  */
	uint64_t _nodes; /* Target nodes footprint. */

	if (nodes == NULL || nnodes > PROCESSOR_NOC_NODES_NUM)
		return (-EINVAL);

	_nodes = 0ULL;
	for (int j = 0; j < nnodes; j++)
		_nodes |= (1ULL << nodes[j]);

	for (int i = 0; i < SYNC_OPEN_MAX; i++)
	{
		if (!resource_is_used(&synctab.opens[i].resource))
			continue;

		if (synctab.opens[i].type != type)
			continue;

		if (synctab.opens[i].nodes == _nodes)
		{
			syncid = i;
			synctab.opens[syncid].refcount++;

			goto found;
		}
	}

	/* Allocate a synchronization point. */
	if ((syncid = resource_alloc(&syncpools.open_pools)) < 0)
		return (-EAGAIN);

	/* Open connector. */
	if ((fd = sync_open(nodes, nnodes, type)) < 0)
	{
		resource_free(&syncpools.open_pools, syncid);
		return (fd);
	}

	/* Initialize synchronization point. */
	synctab.opens[syncid].fd       = fd;
	synctab.opens[syncid].type     = type;
	synctab.opens[syncid].nodes    = _nodes;
	synctab.opens[syncid].refcount = 1;

	resource_set_wronly(&synctab.opens[syncid].resource);
	resource_set_notbusy(&synctab.opens[syncid].resource);

	dcache_invalidate();

found:
	return (syncid);
}


/*============================================================================*
 * _sync_unlink()                                                             *
 *============================================================================*/

/**
 * @brief Destroys a synchronization point.
 *
 * @param syncid ID of the target synchronization point.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int _sync_unlink(int syncid)
{
	int ret; /* HAL function return. */

	/* Invalid sync. */
	if (!_sync_is_valid(syncid, SYNC_CREATE_MAX))
		return (-EBADF);

	/* Bad sync. */
	if (!resource_is_used(&synctab.creates[syncid].resource))
		return (-EINVAL);

	if (--synctab.creates[syncid].refcount == 0)
	{
		if ((ret = sync_unlink(synctab.creates[syncid].fd)) < 0)
			return (ret);

		synctab.creates[syncid].fd = -1;
		resource_free(&syncpools.create_pools, syncid);
	}

	return (0);
}

/*============================================================================*
 * _sync_close()                                                              *
 *============================================================================*/

/**
 * @brief Closes a synchronization point.
 *
 * @param syncid ID of the target synchronization point.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int _sync_close(int syncid)
{
	int ret; /* HAL function return. */

	/* Invalid sync. */
	if (!_sync_is_valid(syncid, SYNC_OPEN_MAX))
		return (-EBADF);

	/* Bad sync. */
	if (!resource_is_used(&synctab.opens[syncid].resource))
		return (-EINVAL);

	if (--synctab.opens[syncid].refcount == 0)
	{
		if ((ret = sync_close(synctab.opens[syncid].fd)) < 0)
			return (ret);

		synctab.opens[syncid].fd    = -1;
		synctab.opens[syncid].nodes = 0;
		resource_free(&syncpools.open_pools, syncid);
	}

	return (0);
}

/*============================================================================*
 * _sync_wait()                                                               *
 *============================================================================*/

/**
 * @brief Waits on a synchronization point.
 *
 * @param syncid ID of the target synchronization point.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int _sync_wait(int syncid)
{
	/* Invalid sync. */
	if (!_sync_is_valid(syncid, SYNC_CREATE_MAX))
		return (-EBADF);

	dcache_invalidate();

	/* Waits. */
	return sync_wait(synctab.creates[syncid].fd);
}

/*============================================================================*
 * _sync_signal()                                                             *
 *============================================================================*/

/**
 * @brief Signals Waits on a synchronization point.
 *
 * @param syncid ID of the target synchronization point.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int _sync_signal(int syncid)
{
	/* Invalid sync. */
	if (!_sync_is_valid(syncid, SYNC_OPEN_MAX))
		return (-EBADF);

	/* Bad sync. */
	if (!resource_is_used(&synctab.opens[syncid].resource))
		return (-EINVAL);

	/* Sends signal. */
	return sync_signal(synctab.opens[syncid].fd);
}

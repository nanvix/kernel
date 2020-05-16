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

/* Must come first. */
#define __NEED_RESOURCE

#include <nanvix/hal.h>
#include <nanvix/kernel/sync.h>
#include <nanvix/kernel/syscall.h>
#include <nanvix/hlib.h>
#include <posix/errno.h>

#if __TARGET_HAS_SYNC

/**
 * @brief Search types for do_sync_search().
 */
#define VSYNC_TYPE_INPUT  (0)
#define VSYNC_TYPE_OUTPUT (1)

typedef int (* hw_alloc_fn)(const int *, int, int);
typedef int (* hw_operation_fn)(int);
typedef int (* hw_release_fn)(int);

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Table of active synchronization points.
 */
PRIVATE struct sync
{
	/*
	 * XXX: Don't Touch! This Must Come First!
	 */
	struct resource resource; /**< Generic resource information. */

	int refcount;             /**< Reference counter.            */
	int hwfd;                 /**< Underlying file descriptor.   */
	int mode;                 /**< Mode of the operation.        */
	int master;               /**< Node number of the ONE.       */
	uint64_t nodeslist;       /**< Nodeslist.                    */
} ALIGN(sizeof(dword_t)) synctab[(SYNC_CREATE_MAX + SYNC_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool syncpool = {
	synctab, (SYNC_CREATE_MAX + SYNC_OPEN_MAX), sizeof(struct sync)
};

/**
 * @brief Global lock.
 */
PRIVATE spinlock_t sync_lock = SPINLOCK_UNLOCKED;

/*============================================================================*
 * do_sync_search()                                                           *
 *============================================================================*/

/**
 * @brief Searches for a sync.
 *
 * Searches for an already existing sync in syncs.
 *
 * @param master    Logic ID of the master node.
 * @param nodeslist Involved nodes list.
 *
 * @returns Upon successful completion, the ID of the HW sync found is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_sync_search(int master, uint64_t nodeslist, int mode, int type)
{
	for (int i = 0; i < (SYNC_CREATE_MAX + SYNC_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&synctab[i].resource))
			continue;

		if (type == VSYNC_TYPE_INPUT)
		{
			if (!resource_is_readable(&synctab[i].resource))
				continue;
		} 

		/* type == VSYNC_TYPE_OUTPUT */
		else if (!resource_is_writable(&synctab[i].resource))
			continue;

		/* Not the same mode? */
		if (synctab[i].mode != mode)
			continue;

		/* Not the same master? */
		if (synctab[i].master != master)
			continue;

		/* Not the same node list? */
		if (synctab[i].nodeslist != nodeslist)
			continue;

		return (i);
	}

	return (-1);
}

/*============================================================================*
 * sync_build_nodeslist()                                                   *
 *============================================================================*/

/**
 * @brief Node list validation.
 *
 * @param nodes  IDs of target NoC nodes.
 * @param nnodes Number of target NoC nodes.
 *
 * @return Non zero if node list is valid and zero otherwise.
 */
PRIVATE uint64_t sync_build_nodeslist(const int * nodes, int nnodes)
{
	uint64_t nodeslist = 0ULL;

	for (int j = 0; j < nnodes; j++)
		nodeslist |= (1ULL << nodes[j]);

	return (nodeslist);
}

/*============================================================================*
 * do_vsync_create()                                                          *
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
PRIVATE int _do_sync_alloc(
	const int * nodes,
	int nnodes,
	int mode,
	int type,
	hw_alloc_fn do_alloc
)
{
	int ret;            /* Return value.           */
	int hwfd;           /* File descriptor.        */
	int syncid;         /* Synchronization point.  */
	uint64_t nodeslist; /* Target nodes list.      */

	nodeslist = sync_build_nodeslist(nodes, nnodes);

	spinlock_lock(&sync_lock);

		ret = (-EBUSY);

		/* Allocate a synchronization point. */
		if ((syncid = do_sync_search(nodes[0], nodeslist, mode, type)) >= 0)
		{
			synctab[syncid].refcount++;
			ret = (syncid);
			goto error;
		}

		ret = (-EAGAIN);

		/* Allocate a synchronization point. */
		if ((syncid = resource_alloc(&syncpool)) < 0)
			goto error;

		if ((hwfd = do_alloc(nodes, nnodes, mode)) < 0)
		{
			resource_free(&syncpool, syncid);
			ret = (hwfd);

			goto error;
		}

		/* Initialize synchronization point. */
		synctab[syncid].refcount  = 1;
		synctab[syncid].hwfd      = hwfd;
		synctab[syncid].mode      = mode;
		synctab[syncid].master    = nodes[0];
		synctab[syncid].nodeslist = nodeslist;

		if (type == VSYNC_TYPE_INPUT)
			resource_set_rdonly(&synctab[syncid].resource);
		else
			resource_set_wronly(&synctab[syncid].resource);

		ret = (syncid);
error:
	spinlock_unlock(&sync_lock);

	return (ret);
}

/*============================================================================*
 * do_vsync_create()                                                          *
 *============================================================================*/

/**
 * @brief Creates a hardware synchronization point.
 *
 * @param nodes     Logic IDs of target nodes.
 * @param nnodes    Number of target nodes.
 * @param mode      Type of synchronization point.
 *
 * @returns Upon successful completion, the ID of the newly created
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 */
PUBLIC int do_vsync_create(const int *nodes, int nnodes, int mode)
{
	return (
		_do_sync_alloc(
			nodes,
			nnodes,
			mode,
			VSYNC_TYPE_INPUT,
			sync_create
		)
	);
}


/*============================================================================*
 * do_vsync_open()                                                          *
 *============================================================================*/

/**
 * @brief Creates a hardware synchronization point.
 *
 * @param nodes     Logic IDs of target nodes.
 * @param nnodes    Number of target nodes.
 * @param mode      Type of synchronization point.
 *
 * @returns Upon successful completion, the ID of the newly created
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 */
PUBLIC int do_vsync_open(const int * nodes, int nnodes, int mode)
{
	return (
		_do_sync_alloc(
			nodes,
			nnodes,
			mode,
			VSYNC_TYPE_OUTPUT,
			sync_open
		)
	);
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
PRIVATE int _do_sync_release(int syncid, int type, hw_release_fn do_release)
{
	int ret; /* Return value. */

	spinlock_lock(&sync_lock);

		ret = (-EBADF);

		/* Bad sync. */
		if (!resource_is_used(&synctab[syncid].resource))
			goto error;


		/* Bad sync. */
		if (type == VSYNC_TYPE_INPUT)
		{
			if (!resource_is_readable(&synctab[syncid].resource))
				goto error;
		} 

		/* type == VSYNC_TYPE_OUTPUT */
		else if (!resource_is_writable(&synctab[syncid].resource))
			goto error;

		ret = (-EBUSY);

		/* Sync not set as busy. */
		if (resource_is_busy(&synctab[syncid].resource))
			goto error;

		/* Releases the virtual sync. */
		if ((--synctab[syncid].refcount) == 0)
		{
			/* Releases the hardware sync. */
			if ((ret = do_release(synctab[syncid].hwfd)) < 0)
				goto error;

			synctab[syncid].hwfd      = -1;
			synctab[syncid].master    = -1;
			synctab[syncid].nodeslist = 0ULL; 

			resource_free(&syncpool, syncid);
		}

		ret = (0);
error:
	spinlock_unlock(&sync_lock);

	return (ret);
}

/*============================================================================*
 * do_vsync_unlink()                                                          *
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
	return (_do_sync_release(syncid, VSYNC_TYPE_INPUT, sync_unlink));
}

/*============================================================================*
 * do_vsync_close()                                                           *
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
	return (_do_sync_release(syncid, VSYNC_TYPE_OUTPUT, sync_close));
}

/*============================================================================*
 * _do_sync_operate()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int _do_sync_operate(int syncid, int type, hw_operation_fn do_operation)
{
	int ret; /* Return value. */

	spinlock_lock(&sync_lock);

		ret = (-EBADF);

		/* Bad sync. */
		if (!resource_is_used(&synctab[syncid].resource))
			goto error;

		/* Bad sync. */
		if (type == VSYNC_TYPE_INPUT)
		{
			if (!resource_is_readable(&synctab[syncid].resource))
				goto error;
		} 

		/* type == VSYNC_TYPE_OUTPUT */
		else if (!resource_is_writable(&synctab[syncid].resource))
			goto error;

		ret = (-EBUSY);

		/* Sync not set as busy. */
		if (resource_is_busy(&synctab[syncid].resource))
			goto error;

		resource_set_busy(&synctab[syncid].resource);

	spinlock_unlock(&sync_lock);

	ret = do_operation(synctab[syncid].hwfd);

	spinlock_lock(&sync_lock);
		resource_set_notbusy(&synctab[syncid].resource);
error:
	spinlock_unlock(&sync_lock);

	return (ret);
}
/*============================================================================*
 * do_vsync_wait()                                                            *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vsync_wait(int syncid)
{
	return (_do_sync_operate(syncid, VSYNC_TYPE_INPUT, sync_wait));
}

/*============================================================================*
 * do_vsync_signal()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vsync_signal(int syncid)
{
	return (_do_sync_operate(syncid, VSYNC_TYPE_OUTPUT, sync_signal));
}

/*============================================================================*
 * vsync_init()                                                               *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC void vsync_init(void)
{
	kprintf("[kernel][noc] initializing the ksync facility");
}

#endif /* __TARGET_SYNC */

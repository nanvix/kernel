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
#include <nanvix/kernel/syscall.h>
#include <nanvix/hlib.h>
#include <posix/errno.h>

#if __TARGET_HAS_SYNC

/**
 * @brief Search types for do_sync_search().
 */
enum sync_type {
	SYNC_INPUT = 0,
	SYNC_OUTPUT = 1
} resource_type_enum_t;

/**
 * @brief Virtual sync flags.
 */
#define VSYNC_STATUS_USED (1 << 0) /**< Used vsync? */

/**
 * @brief Asserts if the virtual sync is used.
 */
#define VSYNC_IS_USED(vsyncid) \
	(virtual_syncs[vsyncid].status & VSYNC_STATUS_USED)

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Auxiliar array to hold nodes list.
 */
int nodes_array[PROCESSOR_NOC_NODES_NUM];

/**
 * @brief Table of virtual synchronization points.
 */
PRIVATE struct
{
	unsigned short status;    /**< VSync status flags.         */
	enum sync_type sync_type; /**< Input / Output ?            */
	int type;                 /**< ALL_FOR_ONE / ONE_FOR_ALL ? */
	int masternum;            /**< Master node number.         */
	int nnodes;               /**< Number of nodes involved.   */
	uint64_t nodeslist;       /**< Node ID list.               */
} ALIGN(sizeof(dword_t)) virtual_syncs[KSYNC_MAX] = {
	[0 ... (KSYNC_MAX - 1)] = {
		.status = 0,
		.type = -1,
		.masternum = -1,
		.nnodes = 0,
		.nodeslist = 0ULL
	},
};

/**
 * @brief Table of active synchronization points.
 */
PRIVATE struct sync
{
	struct resource resource; /**< Underlying resource.        */
	int hwfd;                 /**< Underlying file descriptor. */
	int masternum;            /**< Node number of the ONE.     */
	uint64_t nodeslist;       /**< Nodeslist.                  */
} ALIGN(sizeof(dword_t)) active_syncs[(SYNC_CREATE_MAX + SYNC_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool syncpool = {
	active_syncs, (SYNC_CREATE_MAX + SYNC_OPEN_MAX), sizeof(struct sync)
};

/*============================================================================*
 * do_sync_search()                                                           *
 *============================================================================*/

/**
 * @brief Searches for a sync.
 *
 * Searches for an already existing sync in active_syncs.
 *
 * @param masternum Logic ID of the master node.
 * @param nodeslist Involved nodes list.
 *
 * @returns Upon successful completion, the ID of the HW sync found is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_sync_search(int masternum, uint64_t nodeslist)
{
	for (int i = 0; i < (SYNC_CREATE_MAX + SYNC_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&active_syncs[i].resource))
			continue;

		if (!resource_is_readable(&active_syncs[i].resource))
			continue;

		/* Not the same master? */
		if (active_syncs[i].masternum != masternum)
			continue;

		/* Not the same node list? */
		if (active_syncs[i].nodeslist != nodeslist)
			continue;

		return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_vsync_alloc()                                                           *
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
		if (!VSYNC_IS_USED(i))
			return (i);
	}

	return (-1);
}

/*============================================================================*
 * sync_nodelist_is_valid()                                                   *
 *============================================================================*/

/**
 * @brief Node list validation.
 *
 * @param local  Logic ID of local node.
 * @param nodes  IDs of target NoC nodes.
 * @param nnodes Number of target NoC nodes.
 *
 * @return Non zero if node list is valid and zero otherwise.
 */
PRIVATE int sync_nodelist_is_valid(int local, const int *nodes, int nnodes)
{
	uint64_t checks;

	checks = 0ULL;

	/* Build list of RX NoC nodes. */
	for (int i = 0; i < nnodes; ++i)
	{
		/* Does a node appear twice? */
		if (checks & (1ULL << nodes[i]))
			return (0);

		checks |= (1ULL << nodes[i]);
	}

	/* Local Node found. */
	return (checks & (1ULL << local));
}

/*============================================================================*
 * sync_is_local()                                                            *
 *============================================================================*/

/**
 * @brief Sync local point validation.
 *
 * @param nodenum Logic ID of local node.
 * @param nodes   IDs of target NoC nodes.
 * @param nnodes  Number of target NoC nodes.
 *
 * @return Non zero if local point is valid and zero otherwise.
 */
PRIVATE int sync_is_local(int nodenum, const int *nodes, int nnodes)
{
	/* Underlying NoC node SHOULD be here. */
	if (nodenum != nodes[0])
		return (0);

	/* Underlying NoC node SHOULD NOT be here. */
	for (int i = 1; i < nnodes; i++)
	{
		if (nodenum == nodes[i])
			return (0);
	}

	return (1);
}

/*============================================================================*
 * sync_is_remote()                                                           *
 *============================================================================*/

/**
 * @brief Sync remote point validation.
 *
 * @param nodenum Logic ID of local node.
 * @param nodes   IDs of target NoC nodes.
 * @param nnodes  Number of target NoC nodes.
 *
 * @return Non zero if remote point is valid and zero otherwise.
 */
PRIVATE int sync_is_remote(int nodenum, const int *nodes, int nnodes)
{
	int found = 0;

	/* Underlying NoC node SHOULD NOT be here. */
	if (nodenum == nodes[0])
		return (0);

	/* Underlying NoC node SHOULD be here. */
	for (int i = 1; i < nnodes; i++)
	{
		if (nodenum == nodes[i])
			found++;
	}

	if (found != 1)
		return (0);

	return (1);
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
PRIVATE int _do_sync_create(const int *nodes, int nnodes, int type)
{
	int hwfd;           /* File descriptor.       */
	int syncid;         /* Synchronization point. */
	uint64_t nodeslist; /* Target nodes list.     */

	nodeslist = 0ULL;
	for (int j = 0; j < nnodes; j++)
		nodeslist |= (1ULL << nodes[j]);

	/* Allocate a synchronization point. */
	if ((syncid = do_sync_search(nodes[0], nodeslist)) >= 0)
		return (-EBUSY);

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
 *
 * @returns Upon successful completion, the ID of the newly created
 * virtual synchronization point is returned. Upon failure, a negative
 * error code is returned instead.
 */
PUBLIC int do_vsync_create(const int *nodes, int nnodes, int type)
{
	int vsyncid;        /* Virtual sync point ID. */
	uint64_t nodeslist; /* Target nodes list.     */
	int nodenum;        /* Local node num.        */

	nodenum = processor_node_get_num(0);

	/* Invalid nodes list. */
	if (!sync_nodelist_is_valid(nodenum, nodes, nnodes))
		return (-EINVAL);

	/* Checks the nodes list corretude. */
	if (type == SYNC_ONE_TO_ALL)
	{
		if (!sync_is_remote(nodenum, nodes, nnodes))
			return (-EINVAL);
	}
	else
	{
		if (!sync_is_local(nodenum, nodes, nnodes))
			return (-EINVAL);
	}

	/* Allocate a virtual synchronization point. */
	if ((vsyncid = do_vsync_alloc()) < 0)
		return (-EAGAIN);

	nodeslist = 0ULL;
	for (int j = 0; j < nnodes; j++)
		nodeslist |= (1ULL << nodes[j]);

	/* Initialize virtual synchronization point. */
	virtual_syncs[vsyncid].status   |= VSYNC_STATUS_USED;
	virtual_syncs[vsyncid].type      = type;
	virtual_syncs[vsyncid].sync_type = SYNC_INPUT;
	virtual_syncs[vsyncid].masternum = nodes[0];
	virtual_syncs[vsyncid].nnodes    = nnodes;
	virtual_syncs[vsyncid].nodeslist = nodeslist;

	dcache_invalidate();
	return (vsyncid);
}

/*============================================================================*
 * do_vsync_open()                                                            *
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
PRIVATE int _do_sync_open(const int *nodes, int nnodes, int type)
{
	int hwfd;   /* File descriptor.       */
	int syncid; /* Synchronization point. */

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
	active_syncs[syncid].masternum = nodes[0];

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
 *
 * @returns Upon successful completion, the ID of the opened virtual
 * synchronization point is returned. Upon failure, a negative error
 * code is returned instead.
 *
 * @todo Check for Invalid Remote
 */
PUBLIC int do_vsync_open(const int *nodes, int nnodes, int type)
{
	int vsyncid;        /* Virtual sync point ID. */
	uint64_t nodeslist; /* Target nodes list.     */
	int nodenum;        /* Local node num.        */

	nodenum = processor_node_get_num(0);

	/* Invalid nodes list. */
	if (!sync_nodelist_is_valid(nodenum, nodes, nnodes))
		return (-EINVAL);

	/* Checks the nodes list corretude. */
	if (type == SYNC_ONE_TO_ALL)
	{
		if (!sync_is_local(nodenum, nodes, nnodes))
			return (-EINVAL);
	}
	else
	{
		if (!sync_is_remote(nodenum, nodes, nnodes))
			return (-EINVAL);
	}

	/* Allocate a virtual synchronization point. */
	if ((vsyncid = do_vsync_alloc()) < 0)
		return (-EAGAIN);

	nodeslist = 0ULL;
	for (int j = 0; j < nnodes; j++)
		nodeslist |= (1ULL << nodes[j]);

	/* Initialize virtual synchronization point. */
	virtual_syncs[vsyncid].status   |= VSYNC_STATUS_USED;
	virtual_syncs[vsyncid].type      = type;
	virtual_syncs[vsyncid].sync_type = SYNC_OUTPUT;
	virtual_syncs[vsyncid].masternum = nodes[0];
	virtual_syncs[vsyncid].nnodes    = nnodes;
	virtual_syncs[vsyncid].nodeslist = nodeslist;

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

	resource_free(&syncpool, syncid);

	dcache_invalidate();
	return (0);
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
	/* Bad sync. */
	if (!VSYNC_IS_USED(syncid))
		return (-EBADF);

	/* Bad sync. */
	if (virtual_syncs[syncid].sync_type != SYNC_INPUT)
		return (-EBADF);

	/* Unlink virtual synchronization point. */
	virtual_syncs[syncid].status = 0;

	return (0);
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
	/* Bad sync. */
	if (!VSYNC_IS_USED(syncid))
		return (-EBADF);

	/* Bad sync. */
	if (virtual_syncs[syncid].sync_type != SYNC_OUTPUT)
		return (-EBADF);

	/* Close virtual synchronization point. */
	virtual_syncs[syncid].status = 0;

	return (0);
}

/*============================================================================*
 * do_vsync_wait()                                                            *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vsync_wait(int syncid)
{
	int fd;           /* HW file descriptor.         */
	int ret;          /* Hal function return.        */
	int nsignals = 1; /* Number of signals received. */
	int nodenum = processor_node_get_num(core_get_id());
	uint64_t nodeslist;

	dcache_invalidate();

	/* Bad sync. */
	if (!VSYNC_IS_USED(syncid))
		return (-EBADF);

	/* Bad sync. */
	if (virtual_syncs[syncid].sync_type != SYNC_INPUT)
		return (-EBADF);

	/* Waits. */
	if (virtual_syncs[syncid].type == SYNC_ONE_TO_ALL)
	{
		nodeslist = 0ULL | (1ULL << virtual_syncs[syncid].masternum) | (1ULL << nodenum);
		KASSERT((fd = do_sync_search(virtual_syncs[syncid].masternum, nodeslist)) >= 0);

		/* Waits for the ONE release signal. */
		ret = sync_wait(active_syncs[fd].hwfd);
	}
	else
	{
		for (int j = 0; j < PROCESSOR_NOC_NODES_NUM; ++j)
		{
			nodeslist = 0ULL | (1ULL << virtual_syncs[syncid].masternum);

			if (virtual_syncs[syncid].nodeslist & (1ULL << j))
			{
				/* Doesn't wait for local signal. */
				if (j == virtual_syncs[syncid].masternum)
					continue;

				nodeslist |= 1ULL << j;

				/* Search for the correct fd to wait on. */
				KASSERT((fd = do_sync_search(j, nodeslist)) >= 0);

				/* Waits for the desired signal. */
				if ((ret = sync_wait(active_syncs[fd].hwfd)) < 0)
					return (ret);

				ret = -1;

				/* Check if all signals were received. */
				if (++nsignals == virtual_syncs[syncid].nnodes)
				{
					ret = 0;
					break;
				}

				nodeslist &= ~(1ULL << j);
			}
		}
	}

	return (ret);
}

/*============================================================================*
 * do_vsync_signal()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vsync_signal(int syncid)
{
	int fd;      /* Active_syncs table index. */
	int ret = 0; /* Hal function return.      */

	/* Bad sync. */
	if (!VSYNC_IS_USED(syncid))
		return (-EBADF);

	/* Bad sync. */
	if (virtual_syncs[syncid].sync_type != SYNC_OUTPUT)
		return (-EBADF);

	nodes_array[0] = processor_node_get_num(core_get_id());

	dcache_invalidate();

	if (virtual_syncs[syncid].type == SYNC_ONE_TO_ALL)
	{
		/* Sends each one of ALL the signals to ONE. */
		for (int i = 0, nsignals = 1; i < PROCESSOR_NOC_NODES_NUM; ++i)
		{
			/* Discards uninvolved nodes. */
			if (!(virtual_syncs[syncid].nodeslist & (1ULL << i)))
				continue;

			/* Discards the master node. */
			if (nodes_array[0] == i)
				continue;

			nodes_array[1] = i;

			/* Configure the output synchronization point. */
			if ((fd = _do_sync_open(nodes_array, 2, SYNC_ONE_TO_ALL)) < 0)
				return (fd);

			/* Sends signal. */
			if ((ret = sync_signal(active_syncs[fd].hwfd)) < 0)
				return (ret);

			/* Releases the HW signal sender. */
			ret = _do_sync_release(fd, sync_close);

			if (++nsignals == virtual_syncs[syncid].nnodes)
				break;
		}
	}
	else
	{
		nodes_array[1] = virtual_syncs[syncid].masternum;

		/* Configure the output synchronization point. */
		if ((fd = _do_sync_open(nodes_array, 2, SYNC_ONE_TO_ALL)) < 0)
			return (fd);

		/* Sends signal. */
		if ((ret = sync_signal(active_syncs[fd].hwfd)) < 0)
			return (ret);

		/* Releases the HW signal sender. */
		ret = _do_sync_release(fd, sync_close);
	}

	return (ret);
}

/*============================================================================*
 * ksync_init()                                                               *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC void ksync_init(void)
{
	int nodes[2];
	int nodenum = processor_node_get_num(0);

	kprintf("[kernel][noc] initializing the ksync facility");

	if (cluster_is_iocluster(cluster_get_num()))
	{
		for (int i = 0; i < PROCESSOR_NOC_IONODES_NUM / PROCESSOR_IOCLUSTERS_NUM; ++i)
		{
			nodes[1] = nodenum + i;

			/* Creates all sync interfaces. */
			for (int j = 0; j < PROCESSOR_NOC_NODES_NUM; ++j)
			{
				if (j == nodes[1])
					continue;

				nodes[0] = j;
				KASSERT(_do_sync_create(nodes, 2, SYNC_ONE_TO_ALL) >= 0);
			}
		}
	}
	else
	{
		nodes[1] = nodenum;

		/* Creates all sync interfaces. */
		for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		{
			if (i == nodes[1])
				continue;

			nodes[0] = i;
			KASSERT(_do_sync_create(nodes, 2, SYNC_ONE_TO_ALL) >= 0);
		}
	}
}

#endif /* __TARGET_SYNC */

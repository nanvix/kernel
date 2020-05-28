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
#include <nanvix/kernel/mm.h>
#include <posix/errno.h>

#if __TARGET_HAS_SYNC

#if !__NANVIX_IKC_USES_ONLY_MAILBOX

/*============================================================================*
 * sync_nodelist_is_valid()                                                   *
 *============================================================================*/

/**
 * @brief Node list validation.
 *
 * @param nodes      IDs of target NoC nodes.
 * @param nnodes     Number of target NoC nodes.
 * @param is_the_one True if the local node is the ONE of the rule.
 *
 * @return Non zero if node list is valid and zero otherwise.
 */
PRIVATE int sync_nodelist_is_valid(const int * nodes, int nnodes, int is_the_one)
{
	int local;       /* Local node.          */
	uint64_t checks; /* Bit-stream of nodes. */

	checks = 0ULL;
	local  = processor_node_get_num();

	/* Is the local the one? */
	if (is_the_one && (nodes[0] != local))
		return (0);

	/* Isn't the local the one? */
	if (!is_the_one && (nodes[0] == local))
		return (0);

	/* Build nodelist. */
	for (int i = 0; i < nnodes; ++i)
	{
		/* Invalid node. */
		if (!WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
			return (0);

		/* Does a node appear twice? */
		if (checks & (1ULL << nodes[i]))
			return (0);

		checks |= (1ULL << nodes[i]);
	}

	/* Is the local node founded? */
	return (checks & (1ULL << local));
}

#endif

/*============================================================================*
 * kernel_sync_create()                                                       *
 *============================================================================*/

/**
 * @see vsync_create().
 *
 * @retval -EINVAL IDs of nodes need to exist.
 * @retval -EINVAL At least 2 nodes and at most PROCESSOR_NOC_NODES_NUM must be involved.
 * @retval -EINVAL The type must be SYNC_ONE_TO_ALL or SYNC_ALL_TO_ONE.
 */
PUBLIC int kernel_sync_create(const int * nodes, int nnodes, int type)
{
#if !__NANVIX_IKC_USES_ONLY_MAILBOX

	/* Invalid nodes list. */
	if (nodes == NULL)
		return (-EINVAL);

	/* Invalid number of nodes. */
	if (!WITHIN(nnodes, 2, (PROCESSOR_NOC_NODES_NUM + 1)))
		return(-EINVAL);

	/* Bad sync type. */
	if ((type != SYNC_ONE_TO_ALL) && (type != SYNC_ALL_TO_ONE))
		return (-EINVAL);

	/* Bad nodes list location. */
	if (!mm_check_area(VADDR(nodes), sizeof(int) * nnodes, UMEM_AREA))
		return(-EFAULT);

	/* Invalid nodes list. */
	if (!sync_nodelist_is_valid(nodes, nnodes, (type == SYNC_ALL_TO_ONE)))
		return (-EINVAL);

	return (do_vsync_create(nodes, nnodes, type));

#else
	UNUSED(nodes);
	UNUSED(nnodes);
	UNUSED(type);

	return (-ENOSYS);
#endif
}

/*============================================================================*
 * kernel_sync_open()                                                         *
 *============================================================================*/

/**
 * @see vsync_open().
 *
 * @retval -EINVAL IDs of nodes need to exist.
 * @retval -EINVAL At least 2 nodes and at most PROCESSOR_NOC_NODES_NUM must be involved.
 * @retval -EINVAL The type must be SYNC_ONE_TO_ALL or SYNC_ALL_TO_ONE.
 */
PUBLIC int kernel_sync_open(const int *nodes, int nnodes, int type)
{
#if !__NANVIX_IKC_USES_ONLY_MAILBOX

	/* Invalid nodes list. */
	if (nodes == NULL)
		return (-EINVAL);

	/* Invalid number of nodes. */
	if (!WITHIN(nnodes, 2, (PROCESSOR_NOC_NODES_NUM + 1)))
		return(-EINVAL);

	/* Bad sync type. */
	if ((type != SYNC_ONE_TO_ALL) && (type != SYNC_ALL_TO_ONE))
		return (-EINVAL);

	/* Bad nodes list location. */
	if (!mm_check_area(VADDR(nodes), sizeof(int) * nnodes, UMEM_AREA))
		return(-EFAULT);

	/* Invalid nodes list. */
	if (!sync_nodelist_is_valid(nodes, nnodes, (type == SYNC_ONE_TO_ALL)))
		return (-EINVAL);

	return (do_vsync_open(nodes, nnodes, type));

#else
	UNUSED(nodes);
	UNUSED(nnodes);
	UNUSED(type);

	return (-ENOSYS);
#endif
}

/*============================================================================*
 * kernel_sync_wait()                                                         *
 *============================================================================*/

/**
 * @see vsync_wait().
 */
PUBLIC int kernel_sync_wait(int syncid)
{
#if !__NANVIX_IKC_USES_ONLY_MAILBOX

	/* Invalid sync ID. */
	if (!WITHIN(syncid, 0, KSYNC_MAX))
		return (-EINVAL);

	return (do_vsync_wait(syncid));

#else
	UNUSED(syncid);

	return (-ENOSYS);
#endif
}

/*============================================================================*
 * kernel_sync_signal()                                                       *
 *============================================================================*/

/**
 * @see vsync_signal().
 */
PUBLIC int kernel_sync_signal(int syncid)
{
#if !__NANVIX_IKC_USES_ONLY_MAILBOX

	/* Invalid sync ID. */
	if (!WITHIN(syncid, 0, KSYNC_MAX))
		return (-EINVAL);

	return (do_vsync_signal(syncid));

#else
	UNUSED(syncid);

	return (-ENOSYS);
#endif
}

/*============================================================================*
 * kernel_sync_close()                                                        *
 *============================================================================*/

/**
 * @see vsync_close().
 */
PUBLIC int kernel_sync_close(int syncid)
{
#if !__NANVIX_IKC_USES_ONLY_MAILBOX

	/* Invalid sync ID. */
	if (!WITHIN(syncid, 0, KSYNC_MAX))
		return (-EINVAL);

	return (do_vsync_close(syncid));

#else
	UNUSED(syncid);

	return (-ENOSYS);
#endif
}

/*============================================================================*
 * kernel_sync_unlink()                                                       *
 *============================================================================*/

/**
 * @see vsync_unlink().
 */
PUBLIC int kernel_sync_unlink(int syncid)
{
#if !__NANVIX_IKC_USES_ONLY_MAILBOX

	/* Invalid sync ID. */
	if (!WITHIN(syncid, 0, KSYNC_MAX))
		return (-EINVAL);

	return (do_vsync_unlink(syncid));

#else
	UNUSED(syncid);

	return (-ENOSYS);
#endif
}

/*============================================================================*
 * kernel_sync_wait()                                                         *
 *============================================================================*/

/**
 * @see do_vsync_ioctl().
 */
PUBLIC int kernel_sync_ioctl(int syncid, unsigned request, va_list *args)
{
#if !__NANVIX_IKC_USES_ONLY_MAILBOX

	int ret;

	/* Invalid sync ID. */
	if (!WITHIN(syncid, 0, KSYNC_MAX))
		return (-EINVAL);

	/* Bad args. */
	if (args == NULL)
		return (-EINVAL);

	dcache_invalidate();
		ret = do_vsync_ioctl(syncid, request, *args);
	dcache_invalidate();

	return (ret);

#else
	UNUSED(syncid);
	UNUSED(request);
	UNUSED(args);

	return (-ENOSYS);
#endif
}

#endif /* __TARGET_SYNC */

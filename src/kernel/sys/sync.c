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
#include <nanvix/kernel/sync.h>
#include <nanvix/kernel/mm.h>
#include <posix/errno.h>

#if __TARGET_HAS_SYNC

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
PUBLIC int kernel_sync_create(const int *nodes, int nnodes, int type)
{
	/* Invalid nodes list. */
	if (nodes == NULL)
		return (-EINVAL);

	/* Invalid number of nodes. */
	if (!WITHIN(nnodes, 2, (PROCESSOR_NOC_NODES_NUM + 1)))
		return(-EINVAL);

	/* Bad nodes list location. */
	if (!mm_check_area(VADDR(nodes), sizeof(int) * nnodes, UMEM_AREA))
		return(-EFAULT);

	/* Bad nodes list. */
	for (int i = 0; i < nnodes; i++)
	{
		if (!WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
			return (-EINVAL);
	}

	/* Bad sync type. */
	if ((type != SYNC_ONE_TO_ALL) && (type != SYNC_ALL_TO_ONE))
		return (-EINVAL);

	return (do_vsync_create(nodes, nnodes, type));
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
	/* Invalid nodes list. */
	if (nodes == NULL)
		return (-EINVAL);

	/* Invalid number of nodes. */
	if (!WITHIN(nnodes, 2, (PROCESSOR_NOC_NODES_NUM + 1)))
		return(-EINVAL);

	/* Bad nodes list location. */
	if (!mm_check_area(VADDR(nodes), sizeof(int) * nnodes, UMEM_AREA))
		return(-EFAULT);

	/* Bad nodes list. */
	for (int i = 0; i < nnodes; i++)
	{
		if (!WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
			return (-EINVAL);
	}

	/* Bad sync type. */
	if ((type != SYNC_ONE_TO_ALL) && (type != SYNC_ALL_TO_ONE))
		return (-EINVAL);

	return (do_vsync_open(nodes, nnodes, type));
}

/*============================================================================*
 * kernel_sync_wait()                                                         *
 *============================================================================*/

/**
 * @see vsync_wait().
 */
PUBLIC int kernel_sync_wait(int syncid)
{
	/* Invalid sync ID. */
	if (syncid < 0)
		return (-EINVAL);

	return (do_vsync_wait(syncid));
}

/*============================================================================*
 * kernel_sync_signal()                                                       *
 *============================================================================*/

/**
 * @see vsync_signal().
 */
PUBLIC int kernel_sync_signal(int syncid)
{
	/* Invalid sync ID. */
	if (syncid < 0)
		return (-EINVAL);

	return (do_vsync_signal(syncid));
}

/*============================================================================*
 * kernel_sync_close()                                                        *
 *============================================================================*/

/**
 * @see vsync_close().
 */
PUBLIC int kernel_sync_close(int syncid)
{
	/* Invalid sync ID. */
	if (syncid < 0)
		return (-EINVAL);

	return (do_vsync_close(syncid));
}

/*============================================================================*
 * kernel_sync_unlink()                                                       *
 *============================================================================*/

/**
 * @see vsync_unlink().
 */
PUBLIC int kernel_sync_unlink(int syncid)
{
	/* Invalid sync ID. */
	if (syncid < 0)
		return (-EINVAL);

	return (do_vsync_unlink(syncid));
}

#endif /* __TARGET_SYNC */

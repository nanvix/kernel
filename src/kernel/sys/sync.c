/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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
#include <errno.h>

#if __TARGET_HAS_SYNC

/*============================================================================*
 * kernel_sync_create()                                                       *
 *============================================================================*/

/**
 * @see sync_create().
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
	if (nnodes < 2)
		return (-EINVAL);

	/* Bad nodes list. */
	for (int i = 0; i < nnodes; i++)
	{
		if (!WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
			return (-EINVAL);
	}

	/* Bad sync type. */
	if ((type != SYNC_ONE_TO_ALL) && (type != SYNC_ALL_TO_ONE))
		return (-EINVAL);

	return (_sync_create(nodes, nnodes, type));
}

/*============================================================================*
 * kernel_sync_open()                                                         *
 *============================================================================*/

/**
 * @see sync_open().
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
	if (nnodes < 2)
		return (-EINVAL);

	/* Bad nodes list. */
	for (int i = 0; i < nnodes; i++)
	{
		if (!WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
			return (-EINVAL);
	}

	/* Bad sync type. */
	if ((type != SYNC_ONE_TO_ALL) && (type != SYNC_ALL_TO_ONE))
		return (-EINVAL);
	return (_sync_open(nodes, nnodes, type));
}

/**
 * @see sync_wait().
 */
PUBLIC int kernel_sync_wait(int syncid)
{
	return (_sync_wait(syncid));
}

/**
 * @see sync_signal().
 */
PUBLIC int kernel_sync_signal(int syncid)
{
	return (_sync_signal(syncid));
}

/**
 * @see sync_close().
 */
PUBLIC int kernel_sync_close(int syncid)
{
	return (_sync_close(syncid));
}

/**
 * @see sync_unlink().
 */
PUBLIC int kernel_sync_unlink(int syncid)
{
	return (_sync_unlink(syncid));
}

#endif /* __TARGET_SYNC */

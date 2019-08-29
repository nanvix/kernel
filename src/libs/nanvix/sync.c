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
#include <nanvix.h>
#include <errno.h>

#if __TARGET_HAS_SYNC

/*============================================================================*
 * ksync_sort()                                                               *
 *============================================================================*/

/**
 * @brief Sort the list of RX/TX NoC nodes.
 *
 * @param ranks  Target list of RX NoC nodes.
 * @param nodes  IDs of target NoC nodes.
 * @param nnodes Number of target NoC nodes.
 *
 * @return Zero if only one occurrence of nodenum was found and non zero
 * otherwise.
 *
 * @note This function is non-blocking.
 */
PRIVATE int ksync_sort(int nodenum, int *_nodes, const int *nodes, int nnodes)
{
	int found;
	int j, tmp;

	j = 1;
	found = 0;

	/* Build list of RX NoC nodes. */
	for (int i = 0; i < nnodes; i++)
	{
		if (!WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
			return (-EINVAL);

		if (nodenum == nodes[i])
		{
			j = i;
			found++;
		}

		_nodes[i] = nodes[i];
	}

	if (found != 1)
		return (-EINVAL);

	tmp = _nodes[1];
	_nodes[1] = _nodes[j];
	_nodes[j] = tmp;

	return (0);
}

/*============================================================================*
 * ksync_create()                                                             *
 *============================================================================*/

/*
 * @see sys_sync_create()
 */
int ksync_create(const int *nodes, int nnodes, int type)
{
	int ret;
	int nodenum;
	int _nodes[PROCESSOR_NOC_NODES_NUM];

	/* Invalid nodes list. */
	if (nodes == NULL)
		return (-EINVAL);

	/* Invalid number of nodes. */
	if ((nnodes < 2) || (nnodes > PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	nodenum = processor_node_get_num();

	/* Invalid type. */
	if (type == SYNC_ONE_TO_ALL)
	{
		if (nodenum == nodes[0])
			return (-EINVAL);

		if (ksync_sort(nodenum, _nodes, nodes, nnodes) < 0)
			return (-EINVAL);
	}
	else if (type == SYNC_ALL_TO_ONE)
	{
		if (nodenum != nodes[0])
			return (-EINVAL);

		_nodes[0] = nodes[0];
		for (int i = 1; i < nnodes; i++)
		{
			if (nodenum == nodes[i] || !WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
				return (-EINVAL);

			_nodes[i] = nodes[i];
		}
	}
	else
		return (-EINVAL);

	ret = kcall3(
		NR_sync_create,
		(word_t) _nodes,
		(word_t) nnodes,
		(word_t) type
	);

	return (ret);
}

/*============================================================================*
 * ksync_open()                                                               *
 *============================================================================*/

/*
 * @see sys_sync_open()
 */
int ksync_open(const int *nodes, int nnodes, int type)
{
	int ret;
	int nodenum;
	int _nodes[PROCESSOR_NOC_NODES_NUM];

	/* Invalid list of nodes. */
	if (nodes == NULL)
		return (-EINVAL);

	/* Invalid number of nodes. */
	if ((nnodes < 2) || (nnodes > PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	nodenum = processor_node_get_num();

	/* Invalid type. */
	if (type == SYNC_ONE_TO_ALL)
	{
		if (nodenum != nodes[0])
			return (-EINVAL);

		_nodes[0] = nodes[0];
		for (int i = 1; i < nnodes; i++)
		{
			if (nodenum == nodes[i] || !WITHIN(nodes[i], 0, PROCESSOR_NOC_NODES_NUM))
				return (-EINVAL);

			_nodes[i] = nodes[i];
		}
	}
	else if (type == SYNC_ALL_TO_ONE)
	{
		if (nodenum == nodes[0])
			return (-EINVAL);

		if (ksync_sort(nodenum, _nodes, nodes, nnodes) < 0)
			return (-EINVAL);
	}
	else
		return (-EINVAL);

	ret = kcall3(
		NR_sync_open,
		(word_t) _nodes,
		(word_t) nnodes,
		(word_t) type
	);

	return (ret);
}

/*============================================================================*
 * ksync_wait()                                                               *
 *============================================================================*/

/*
 * @see sys_sync_wait()
 */
int ksync_wait(int syncid)
{
	int ret;

	ret = kcall1(
		NR_sync_wait,
		(word_t) syncid
	);

	return (ret);
}

/*============================================================================*
 * ksync_signal()                                                             *
 *============================================================================*/

/*
 * @see sys_sync_signal()
 */
int ksync_signal(int syncid)
{
	int ret;

	ret = kcall1(
		NR_sync_signal,
		(word_t) syncid
	);

	return (ret);
}

/*============================================================================*
 * ksync_close()                                                              *
 *============================================================================*/

/*
 * @see sys_sync_close()
 */
int ksync_close(int syncid)
{
	int ret;

	ret = kcall1(
		NR_sync_close,
		(word_t) syncid
	);

	return (ret);
}

/*============================================================================*
 * ksync_unlink()                                                             *
 *============================================================================*/

/*
 * @see sys_sync_unlink()
 */
int ksync_unlink(int syncid)
{
	int ret;

	ret = kcall1(
		NR_sync_unlink,
		(word_t) syncid
	);

	return (ret);
}

#endif /* __TARGET_SYNC */

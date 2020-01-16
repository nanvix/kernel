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

#include <nanvix/kernel/thread.h>
#include <net/noc.h>

#if __NANVIX_HAS_NETWORK

/**
 * @brief NoC structure.
 */
int noc[PROCESSOR_CLUSTERS_NUM] = {
    [0 ... (PROCESSOR_CLUSTERS_NUM - 1)] = 1
};

/**
 * @brief Map of core ids to nodenums.
 */
PRIVATE int net_corenums[CORES_NUM];

/*============================================================================*
 * net_processor_node_get_id()                                                *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description to this function.
 */
PRIVATE int net_processor_node_get_id(void)
{
	return (net_cluster_get_num());
}

/*============================================================================*
 * net_processor_noc_setup()                                                  *
 *============================================================================*/

/**
 * @brief Initializes the noc interface.
 */
PUBLIC void net_processor_noc_setup(void)
{
	for (int i = 0; i < CORES_NUM; ++i)
		net_corenums[i] = net_processor_node_get_id();
}

/*============================================================================*
 * net_processor_noc_node_to_cluster_num()                                    *
 *============================================================================*/

/**
 * @brief Converts a NoC node number to cluster number.
 *
 * @param nodenum Target node number.
 *
 * @returns The logical cluster number in which the node number @p
 * nodenum is located.
 */
PRIVATE int net_processor_noc_node_to_cluster_num(int nodenum)
{
	KASSERT((nodenum >= 0) && (nodenum <= PROCESSOR_NOC_NODES_NUM));

	return (nodenum);
}

/*============================================================================*
 * net_processor_noc_is_ionode()                                              *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description to this function.
 */
PUBLIC int net_processor_noc_is_ionode(int nodenum)
{
	return (nodenum == PROCESSOR_NODENUM_MASTER);
}

/*============================================================================*
 * net_processor_noc_is_cnode()                                               *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description to this function.
 */
PUBLIC int net_processor_noc_is_cnode(int nodenum)
{
	return (nodenum != PROCESSOR_NODENUM_MASTER);
}

/*============================================================================*
 * net_processor_node_get_num()                                               *
 *============================================================================*/

/**
 * @brief Gets the logic number of the target NoC node
 * attached with a core.
 *
 * @param coreid Attached core ID.
 *
 * @returns The logic number of the target NoC node attached
 * with the @p coreid.
 */
PUBLIC int net_processor_node_get_num(int coreid)
{
	/* Invalid coreid. */
	if (!WITHIN(coreid, 0, CORES_NUM))
		return (-EINVAL);

	return (NET_PROCESSOR_NODENUM);
}

/*============================================================================*
 * net_processor_node_set_num()                                               *
 *============================================================================*/

/**
 * @brief Exchange the logic number of the target NoC node
 * attached with a core.
 *
 * @param coreid  Attached core ID.
 * @param nodenum Logic ID of the target NoC node.
 *
 * @returns Zero if the target NoC node is successfully attached
 * to the requested @p coreid, and non zero otherwise.
 */
PUBLIC int net_processor_node_set_num(int coreid, int nodenum)
{
	/* Invalid coreid. */
	if (!WITHIN(coreid, 0, CORES_NUM))
		return (-EINVAL);

	/* Invalid coreid. */
	if (!WITHIN(nodenum, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Invalid nodenum. */
	if (cluster_get_num() != net_processor_noc_node_to_cluster_num(nodenum))
		return (-EINVAL);

	net_corenums[coreid] = nodenum;

	return (0);
}

#endif /* __NANVIX_HAS_NETWORK */

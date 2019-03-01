/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef PROCESSOR_BOSTAN_NOC_NODE_H_
#define PROCESSOR_BOSTAN_NOC_NODE_H_

	/* Cluster API. */
	#include <arch/processor/bostan/_bostan.h>

	/**
	 * @name D-NoC tag offset
	 */
	/**@{*/
	#define K1B_DNOC_RX_OFFSET 0 /**< Receive tag offset.  */
	#define K1B_DNOC_TX_OFFSET 0 /**< Transfer tag offset. */
	#define K1B_DNOC_UX_OFFSET 0 /**< UCore thread offset. */
	/**@}*/

	/**
	 * @brief Gets the ID of the NoC node attached to the underlying core.
	 *
	 * @returns The ID of the NoC node attached to the underlying core is
     * returned.
	 */
	EXTERN int k1b_node_get_id(void);

	/**
	 * @brief Gets the logic number of the target NoC node.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns The logic number of the target NoC node.
	 */
	EXTERN int k1b_node_get_num(int nodeid);

	/**
	 * @brief Converts a nodes list.
	 *
	 * @param _nodes Place to store converted list.
	 * @param nodes  Target nodes list.
	 * @param nnodes Number of nodes in the list.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int k1b_nodes_convert(int *_nodes, const int *nodes, int nnodes);

/*============================================================================*
 * NoC Interface                                                              *
 *============================================================================*/

	/**
	 * @name Number of NoC nodes attached to an IO device.
	 */
	#define K1B_NR_NOC_IONODES 8

	/**
	 * @name Number of NoC nodes not attached to an IO device.
	 */
	#define K1B_NR_NOC_CNODES 16

	/**
	 * @name Number of NoC nodes.
	 */
	#define K1B_NR_NOC_NODES (K1B_NR_NOC_IONODES + K1B_NR_NOC_CNODES)

	/**
	 * @brief k1b NoC handler.
	 */
	typedef void(* k1b_noc_handler_fn)(void *args);

	/**
	 * @brief Asserts whether a NoC node is attached to IO cluster 0.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns One if the target NoC node is attached to IO cluster 0,
	 * and zero otherwise.
	 */
	EXTERN int k1b_noc_is_ionode0(int nodeid);

	/**
	 * @brief Asserts whether a NoC node is attached to IO cluster 1.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns One if the target NoC node is attached to IO cluster 1,
	 * and zero otherwise.
	 */
	EXTERN int k1b_noc_is_ionode1(int nodeid);

	/**
	 * @brief Asserts whether a NoC node is attached to an IO cluster.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns One if the target NoC node is attached to an IO cluster,
	 * and zero otherwise.
	 */
	EXTERN int k1b_noc_is_ionode(int nodeid);

	/**
	 * @brief Asserts whether a NoC node is attached to a compute cluster.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns One if the target NoC node is attached to a compute
	 * cluster, and zero otherwise.
	 */
	EXTERN int k1b_noc_is_cnode(int nodeid);

	/**
	 * @brief Returns the mailbox NoC tag for a target NoC node ID.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns The NoC tag attached to the underlying node ID is
	 * returned.
	 */
	EXTERN int k1b_noc_mailbox_tag(int nodeid);

	/**
	 * @brief Returns the portal NoC tag for a target NoC node ID.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns The NoC tag attached to the underlying node ID is
	 * returned.
	 */
	EXTERN int k1b_noc_portal_tag(int nodeid);

	/**
	 * @brief Returns the synchronization NoC tag for a target NoC node ID.
	 *
	 * @param nodeid ID of the target NoC node.
	 * @returns The NoC tag attached to the underlying node ID is
	 * returned.
	 */
	EXTERN int k1b_noc_sync_tag(int nodeid);

	/**
	 * @brief Asserts whether a NoC tag is valid.
	 *
	 * @param tag Number tag.
	 * @param type Buffer type.
	 * @returns One if the tag is valid, and zero otherwise.
	 */
	EXTERN int k1b_noc_tag_is_valid(int tag, int type);


#endif /* PROCESSOR_BOSTAN_NOC_NODE_H_ */

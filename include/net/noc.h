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

#ifndef _NET_NOC_H_
#define _NET_NOC_H_

	#include <nanvix/hlib.h>
	#include <posix/stdint.h>
	#include <nanvix/kernel/config.h>
	#include <nanvix/const.h>

	/**
	 * @brief Initializes the noc interface.
	 */
	EXTERN void net_processor_noc_setup(void);

	/**
	 * @brief Asserts whether a NoC node is attached to an IO cluster.
	 *
	 * @param nodenum Logical number of the target NoC node.
	 *
	 * @returns One if the target NoC node is attached to an IO cluster,
	 * and zero otherwise.
	 */
	EXTERN int net_processor_noc_is_ionode(int nodenum);

	/**
	 * @brief Asserts whether a NoC node is attached to a compute cluster.
	 *
	 * @param nodenum Logical number of the target NoC node.
	 *
	 * @returns One if the target NoC node is attached to a compute
	 * cluster, and zero otherwise.
	 */
	EXTERN int net_processor_noc_is_cnode(int nodenum);

	/**
	 * @brief Gets the logic number of the target NoC node
	 * attached with a core.
	 *
	 * @param coreid Attached core ID.
	 *
	 * @returns The logic number of the target NoC node attached
	 * with the @p coreid.
	 */
	EXTERN int net_processor_node_get_num(int coreid);

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
	EXTERN int net_processor_node_set_num(int coreid, int nodenum);

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

	/**
	 * @name Exported Constans
	 */
	/**@{*/
	#define PROCESSOR_NOC_IONODES_NUM NET_PROCESSOR_NOC_IONODES_NUM /**< NET_PROCESSOR_NOC_IONODES_NUM */
	#define PROCESSOR_NOC_CNODES_NUM  NET_PROCESSOR_NOC_CNODES_NUM  /**< NET_PROCESSOR_NOC_CNODES_NUM  */
	#define PROCESSOR_NODENUM_MASTER  NET_PROCESSOR_NODENUM_MASTER  /**< NET_PROCESSOR_NODENUM_MASTER  */
	/**@}*/

	/**
	 * @name Exported Functions
	 */
	/**@{*/
	#define __processor_noc_setup_fn     /**< processor_noc_setup()     */
	#define __processor_noc_is_ionode_fn /**< processor_noc_is_ionode() */
	#define __processor_noc_is_cnode_fn  /**< processor_noc_is_cnode()  */
	#define __processor_node_get_num_fn  /**< processor_node_get_num()  */
	#define __processor_node_set_num_fn  /**< processor_node_set_num()  */
	/**@}*/

	/**
	 * @name Total number of NoC nodes.
	 */
	#define PROCESSOR_NOC_NODES_NUM \
		(PROCESSOR_NOC_IONODES_NUM + PROCESSOR_NOC_CNODES_NUM)

	/**
	 * @brief Dummy operation.
	 */
	static inline void processor_noc_setup(void)
	{
		net_processor_noc_setup();
	}

	/**
	 * @see net_processor_noc_is_ionode()
	 */
	static inline int processor_noc_is_ionode(int nodenum)
	{
		return (net_processor_noc_is_ionode(nodenum));
	}

	/**
	 * @see net_processor_noc_is_cnode()
	 */
	static inline int processor_noc_is_cnode(int nodenum)
	{
		return (net_processor_noc_is_cnode(nodenum));
	}

	/**
	 * @see net_processor_node_get_num().
	 */
	static inline int processor_node_get_num(int coreid)
	{
		return (net_processor_node_get_num(coreid));
	}

	/**
	 * @see net_processor_node_set_num().
	 */
	static inline int processor_node_set_num(int coreid, int nodenum)
	{
		return (net_processor_node_set_num(coreid, nodenum));
	}

#endif /* _NET_NOC_H_ */

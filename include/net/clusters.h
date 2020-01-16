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

#ifndef _NET_CLUSTERS_H_
#define _NET_CLUSTERS_H_

	#include <nanvix/hlib.h>
	#include <posix/stdint.h>
	#include <nanvix/kernel/config.h>
	#include <nanvix/const.h>

	/**
	 * @brief Retrieves the logical number of the underlying cluster.
	 */
	EXTERN int net_cluster_get_num(void);

	/**
	 * @brief Asserts whether or not the target cluster is a compute cluster.
	 *
	 * @param clusterid ID of the target cluster.
	 *
	 * @return Non zero if the target cluster is a compute cluster and
	 * zero otherwise.
	 */
	EXTERN int net_cluster_is_compute(int clusternum);

	/**
	 * @brief Asserts whether or not the target cluster is an IO cluster.
	 *
	 * @param clusterid ID of the target cluster.
	 *
	 * @return Non zero if the target cluster is an IO cluster and
	 * zero otherwise.
	 */
	EXTERN int net_cluster_is_io(int clusternum);

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

	/**
	 * @name Provided Constants
	 */
	/**@{*/
	#define PROCESSOR_CCLUSTERS_NUM     NET_PROCESSOR_CCLUSTERS_NUM     /**< @see NET_PROCESSOR_CCLUSTERS_NUM     */
	#define PROCESSOR_IOCLUSTERS_NUM    NET_PROCESSOR_IOCLUSTERS_NUM    /**< @see NET_PROCESSOR_IOCLUSTERS_NUM    */
	#define PROCESSOR_CLUSTERNUM_MASTER NET_PROCESSOR_CLUSTERNUM_MASTER /**< @see NET_PROCESSOR_CLUSTERNUM_MASTER */
	/**@}*/

	/**
	 * @name Provided Functions
	 */
	/**@{*/
	#define __cluster_get_num_fn      /**< net_cluster_cluster_get_num() */
	#define __cluster_is_ccluster_fn  /**< net_cluster_is_compute()      */
	#define __cluster_is_iocluster_fn /**< net_cluster_is_io()           */
	/**@}*/

	/**
	 * @brief Total number of clusters in the processor.
	 */
	#define PROCESSOR_CLUSTERS_NUM \
		(PROCESSOR_IOCLUSTERS_NUM + PROCESSOR_CCLUSTERS_NUM)

	/**
	 * @see net_processor_cluster_get_num().
	 */
	static inline int cluster_get_num(void)
	{
		return (net_cluster_get_num());
	}

	/**
	 * @see net_processor_cluster_is_ccluster().
	 */
	static inline int cluster_is_ccluster(int clusternum)
	{
		return (net_cluster_is_compute(clusternum));
	}

	/**
	 * @see net_processor_cluster_is_iocluster().
	 */
	static inline int cluster_is_iocluster(int clusternum)
	{
		return (net_cluster_is_io(clusternum));
	}

#endif /* _NET_CLUSTERS_H_ */

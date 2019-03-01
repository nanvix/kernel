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

#ifndef HAL_PROCESSOR_CLUSTERS_H_
#define HAL_PROCESSOR_CLUSTERS_H_

	/* Processor Interface Implementation */
	#include <nanvix/hal/processor/_processor.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Multicluster Processor*/
	#if (PROCESSOR_IS_MULTICLUSTER)

		/* Functions */
		#ifndef __cluster_get_id_fn
		#error "cluster_get_id() not defined?"
		#endif
		#ifndef __cluster_is_ccluster_fn
		#error "cluster_is_ccluster() not defined?"
		#endif
		#ifndef __cluster_is_iocluster_fn
		#error "cluster_is_iocluster() not defined?"
		#endif

	#endif

/*============================================================================*
 * Cache Interface                                                             *
 *============================================================================*/

/**
 * @defgroup kernel-hal-processor-clusters Clusters
 * @ingroup kernel-hal-processor
 *
 * @brief Processor Clusters HAL Interface
 */
/**@{*/

	/**
	 * @brief Gets the ID of the underlying cluster.
	 *
	 * @returns The ID of the underlying cluster.
	 */
#if (PROCESSOR_IS_MULTICLUSTER)
	EXTERN int cluster_get_id(void);
#else
	static inline int cluster_get_id(void)
	{
		return (0);
	}
#endif

	/**
	 * @brief Asserts if a cluster is a compute cluster.
	 *
	 * @param clusterid ID of the target cluster.
	 *
	 * @returns Non-zero if the target cluster @p clusterid is a
	 * compute cluster and zero otherwise.
	 */
#if (PROCESSOR_IS_MULTICLUSTER)
	EXTERN int cluster_is_ccluster(int clusterid);
#else
	static inline int cluster_is_ccluster(int clusterid)
	{
		((void) clusterid);

		return (0);
	}
#endif

	/**
	 * @brief Asserts if a cluster is an I/O cluster.
	 *
	 * @param clusterid ID of the target cluster.
	 *
	 * @returns Non-zero if the target cluster @p clusterid is an I/O
	 * cluster and zero otherwise.
	 */
#if (PROCESSOR_IS_MULTICLUSTER)
	EXTERN int cluster_is_iocluster(int clusterid);
#else
	static inline int cluster_is_iocluster(int clusterid)
	{
		((void) clusterid);

		return (0);
	}
#endif

/**@}*/

#endif /* HAL_PROCESSOR_CLUSTERS_H_ */


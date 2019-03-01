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

#ifndef PROCESSOR_BOSTAN_CLUSTERS_H_
#define PROCESSOR_BOSTAN_CLUSTERS_H_

	/* Processor Interface Implementation */
	#include <arch/processor/bostan/_bostan.h>

/**
 * @addtogroup processor-bostan-clusters Clusters
 * @ingroup processor-bostan
 *
 * @brief Bostan Processor Cluster Interface
 */
/**@*/

	/**
	 * @name Cluster IDs
	 */
	/**@{*/
	#define BOSTAN_CCLUSTER0    0 /**< Compute cluster  0. */
	#define BOSTAN_CCLUSTER1    1 /**< Compute cluster  1. */
	#define BOSTAN_CCLUSTER2    2 /**< Compute cluster  2. */
	#define BOSTAN_CCLUSTER3    3 /**< Compute cluster  3. */
	#define BOSTAN_CCLUSTER4    4 /**< Compute cluster  4. */
	#define BOSTAN_CCLUSTER5    5 /**< Compute cluster  5. */
	#define BOSTAN_CCLUSTER6    6 /**< Compute cluster  6. */
	#define BOSTAN_CCLUSTER7    7 /**< Compute cluster  7. */
	#define BOSTAN_CCLUSTER8    8 /**< Compute cluster  8. */
	#define BOSTAN_CCLUSTER9    9 /**< Compute cluster  9. */
	#define BOSTAN_CCLUSTER10  10 /**< Compute cluster 10. */
	#define BOSTAN_CCLUSTER11  11 /**< Compute cluster 11. */
	#define BOSTAN_CCLUSTER12  12 /**< Compute cluster 12. */
	#define BOSTAN_CCLUSTER13  13 /**< Compute cluster 13. */
	#define BOSTAN_CCLUSTER14  14 /**< Compute cluster 14. */
	#define BOSTAN_CCLUSTER15  15 /**< Compute cluster 15. */
	#define BOSTAN_IOCLUSTER0 128 /**< IO cluster 0.       */
	#define BOSTAN_IOCLUSTER1 192 /**< IO cluster 1.       */
	/**@}*/

	/**
	 * @todo Comment this function.
	 */
	extern int bostan_cluster_get_id(void);

	/**
	 * @todo Comment this.
	 */
	extern int bostan_cluster_is_ccluster(int clusterid);

	/**
	 * @todo Comment this.
	 */
	extern int bostan_cluster_is_iocluster(int clusterid);

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond bostan
 */

	/**
	 * @name Provided Functions
	 */
	/**@{*/
	#define __cluster_get_id_fn       /**< bostan_cluster_get_id()       */
	#define __cluster_is_ccluster_fn  /**< bostan_cluster_is_ccluster()  */
	#define __cluster_is_iocluster_fn /**< bostan_cluster_is_iocluster() */
	/**@}*/

	/**
	 * @see bostan_cluster_get_id().
	 */
	static inline int cluster_get_id(void)
	{
		return (bostan_cluster_get_id());
	}

	/**
	 * @see bostan_cluster_is_ccluster().
	 */
	static inline int cluster_is_ccluster(int clusterid)
	{
		return (bostan_cluster_is_ccluster(clusterid));
	}

	/**
	 * @see bostan_cluster_is_iocluster().
	 */
	static inline int cluster_is_iocluster(int clusterid)
	{
		return (bostan_cluster_is_iocluster(clusterid));
	}

/**@endcond*/

#endif /* PROCESSOR_BOSTAN_CLUSTERS_H_ */


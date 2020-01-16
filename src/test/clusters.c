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

#include <nanvix/hal.h>
#include <nanvix/const.h>
#include <nanvix/hlib.h>

#if PROCESSOR_IS_MULTICLUSTER || __NANVIX_HAS_NETWORK

#include "test.h"

/**
 * @brief Launch verbose tests?
 */
#define TEST_CLUSTERS_VERBOSE 0

/*============================================================================*
 * API Tests                                                                  *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Query Logical Cluster ID                                                   *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Query Logical Cluster ID
 */
PRIVATE void test_clusters_get_num(void)
{
	int clusternum;

	clusternum = cluster_get_num();

#if (TEST_CLUSTERS_VERBOSE)
	kprintf("[test][processor][clusters][api] cluster %d running", clusternum);
#endif

	KASSERT(clusternum == PROCESSOR_CLUSTERNUM_MASTER);
}

/*----------------------------------------------------------------------------*
 * Query Cluster Type                                                         *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Query Cluster Type
 */
PRIVATE void test_clusters_get_type(void)
{
	KASSERT(cluster_is_iocluster(PROCESSOR_CLUSTERNUM_MASTER));
}

/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * @brief API Tests.
 */
PRIVATE struct test test_api_clusters[] = {
	{ test_clusters_get_num,  "get logical cluster id" },
	{ test_clusters_get_type, "get cluster type      " },
	{ NULL,                    NULL                    },
};

/**
 * The test_clusters() function launches regression tests on the
 * Clusters Interface of the Processor Abstraction Layer.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void test_clusters(void)
{
	/* API Tests */
	for (int i = 0; test_api_clusters[i].test_fn != NULL; i++)
	{
		test_api_clusters[i].test_fn();
		kprintf("[test][processor][clusters][api] %s [passed]",
			test_api_clusters[i].name
		);
	}
}

#endif /* PROCESSOR_IS_MULTICLUSTER */

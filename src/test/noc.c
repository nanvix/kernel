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

#if PROCESSOR_HAS_NOC || __NANVIX_HAS_NETWORK

#include "test.h"

/**
 * @brief Launch verbose tests?
 */
#define TEST_NOC_VERBOSE 0

/*============================================================================*
 * API Tests                                                                  *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Query Logical NoC Node Number                                              *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Query Logical NoC Node Number
 */
PRIVATE void test_node_get_num(void)
{
	int nodenum;

	nodenum = processor_node_get_num(COREID_MASTER);

#if (TEST_NOC_VERBOSE)
	kprintf("[test][processor][node][api] noc node %d online", nodenum);
#endif

	KASSERT(nodenum == PROCESSOR_NODENUM_MASTER);
}

/*----------------------------------------------------------------------------*
 * Query NoC Node Type                                                        *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Query NoC Node Type
 */
PRIVATE void test_node_get_type(void)
{
	KASSERT(processor_noc_is_ionode(PROCESSOR_NODENUM_MASTER));
	KASSERT(!processor_noc_is_cnode(PROCESSOR_NODENUM_MASTER));
}

/*----------------------------------------------------------------------------*
 * Exchange Logical NoC Node Number                                           *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Exchange Logical NoC Node Number
 */
PRIVATE void test_node_set_num(void)
{
	int nodenum;

	nodenum = processor_node_get_num(COREID_MASTER);
	KASSERT(nodenum == PROCESSOR_NODENUM_MASTER);

#if (TEST_NOC_VERBOSE)
	kprintf("[test][processor][node][api] noc node %d online", nodenum);
#endif

	/* New nodenum (1 % (Interfaces available in only one IO Cluster)) */
	nodenum += (1 % (PROCESSOR_NOC_IONODES_NUM / PROCESSOR_IOCLUSTERS_NUM));

#if (TEST_NOC_VERBOSE)
	kprintf("[test][processor][node][api] exchange noc node number to %d", nodenum);
#endif

	KASSERT(processor_node_set_num(COREID_MASTER, nodenum) == 0);
	KASSERT(processor_node_get_num(COREID_MASTER) == nodenum);

	/* Restoure old nodenum. */
	nodenum -= (1 % (PROCESSOR_NOC_IONODES_NUM / PROCESSOR_IOCLUSTERS_NUM));

	KASSERT(processor_node_set_num(COREID_MASTER, nodenum) == 0);
}

/**
 * @brief API Tests.
 */
PRIVATE struct
{
	void (*test_fn)(); /**< Test function.     */
	const char *name;  /**< Test Name.         */
} test_api_node[] = {
	{ test_node_get_num,  "get logical noc node num" },
	{ test_node_set_num,  "set logical noc node num" },
	{ test_node_get_type, "get noc node type       " },
	{ NULL,                NULL                      },
};

/*============================================================================*
 * FAULT Tests                                                                *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Query Logical NoC Node Number with invalid arguments                       *
 *----------------------------------------------------------------------------*/

/**
 * @brief FAULT Test: Invalid Get Logical NoC Node Number
 */
PRIVATE void test_node_invalid_get_num(void)
{
	KASSERT(processor_node_get_num(-1) == -EINVAL);
	KASSERT(processor_node_get_num(CORES_NUM) == -EINVAL);
}

/*----------------------------------------------------------------------------*
 * Exchange Logical NoC Node Number with invalid arguments                    *
 *----------------------------------------------------------------------------*/

/**
 * @brief FAULT Test: Invalid Set Logical NoC Node Number
 */
PRIVATE void test_node_invalid_set_num(void)
{
	/* Invalid coreid. */
	KASSERT(processor_node_set_num(-1, PROCESSOR_NODENUM_MASTER) == -EINVAL);
	KASSERT(processor_node_set_num(CORES_NUM, PROCESSOR_NODENUM_MASTER) == -EINVAL);

	/* Invalid nodenum. */
	KASSERT(processor_node_set_num(COREID_MASTER, -1) == -EINVAL);
	KASSERT(processor_node_set_num(COREID_MASTER, PROCESSOR_NOC_NODES_NUM) == -EINVAL);
}

/*----------------------------------------------------------------------------*
 * Exchange Logical NoC Node Number with bad arguments                        *
 *----------------------------------------------------------------------------*/

/**
 * @brief FAULT Test: Bad Set Logical NoC Node Number
 */
PRIVATE void test_node_bad_set_num(void)
{
	int nodenum;

	/* nodenum + (Interfaces available in only one IO Cluster). */
	nodenum = PROCESSOR_NODENUM_MASTER + (PROCESSOR_NOC_IONODES_NUM / PROCESSOR_IOCLUSTERS_NUM);

	/* Bad nodenum. */
	KASSERT(processor_node_set_num(COREID_MASTER, nodenum) == -EINVAL);
}

/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * @brief API Tests.
 */
PRIVATE struct test test_fault_node[] = {
	{ test_node_invalid_get_num, "invalid get logical noc node num" },
	{ test_node_invalid_set_num, "invalid set logical noc node num" },
	{ test_node_bad_set_num,     "bad set logical noc node num    " },
	{ NULL,                       NULL                              },
};

/**
 * The test_noc() function launches regression tests on the NoC
 * Interface of the Processor Abstraction Layer.
 *
 * @author Pedro Henrique Penna
 * @author João Vicente Souto
 */
PUBLIC void test_noc(void)
{
	/* API Tests */
	for (int i = 0; test_api_node[i].test_fn != NULL; i++)
	{
		test_api_node[i].test_fn();
		kprintf("[test][processor][node][api] %s [passed]",
			test_api_node[i].name
		);
	}

	/* FAULT Tests */
	for (int i = 0; test_fault_node[i].test_fn != NULL; i++)
	{
		test_fault_node[i].test_fn();
		kprintf("[test][processor][node][api] %s [passed]",
			test_fault_node[i].name
		);
	}
}

#endif /* PROCESSOR_HAS_NOC */

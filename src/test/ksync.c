/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
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

#include <nanvix.h>
#include <errno.h>

#include "test.h"

#if __TARGET_HAS_SYNC

/**
 * @brief Test's parameters
 */
#define NR_NODES       2
#define NR_NODES_MAX   PROCESSOR_NOC_NODES_NUM
#define MASTER_NODENUM 0
#define SLAVE_NODENUM  1

/**
 * @brief Auxiliar array
 */
int nodenums[NR_NODES] = {
	SLAVE_NODENUM,
	MASTER_NODENUM,
};

/*============================================================================*
 * API Test: Create Unlink                                                    *
 *============================================================================*/

/**
 * @brief API Test: Synchronization Point Create Unlink
 */
void test_api_sync_create_unlink(void)
{
	int syncid;
	int nodes[NR_NODES];

	nodes[0] = processor_node_get_num();

	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodes[0])
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) >= 0);
	test_assert(ksync_unlink(syncid) == 0);
}

/*============================================================================*
 * API Test: Open Close                                                       *
 *============================================================================*/

/**
 * @brief API Test: Synchronization Point Open Close
 */
void test_api_sync_open_close(void)
{
	int syncid;
	int nodes[NR_NODES];

	nodes[0] = processor_node_get_num();

	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodes[0])
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_open(&nodes[0], NR_NODES, SYNC_ONE_TO_ALL)) >= 0);
	test_assert(ksync_close(syncid) == 0);
}

/*============================================================================*
 * API Test: Signal Wait                                                      *
 *============================================================================*/

/**
 * @brief API Test: Synchronization Point Signal Wait
 */
void test_api_sync_signal_wait(void)
{
	int syncin;
	int syncout;
	int nodenum;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = MASTER_NODENUM;

	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == MASTER_NODENUM)
			continue;

		nodes[j++] = nodenums[i];
	}

	if (nodenum == MASTER_NODENUM)
	{
		test_assert((syncin = ksync_create(&nodes[0], NR_NODES, SYNC_ALL_TO_ONE)) >= 0);
		test_assert((syncout = ksync_open(&nodes[0], NR_NODES, SYNC_ONE_TO_ALL)) >= 0);

		for (int i = 1; i < NITERATIONS; i++)
		{
			test_assert(ksync_wait(syncin) == 0);
			test_assert(ksync_signal(syncout) == 0);
		}
	}
	else
	{
		test_assert((syncin = ksync_create(&nodes[0], NR_NODES, SYNC_ONE_TO_ALL)) >= 0);
		test_assert((syncout = ksync_open(&nodes[0], NR_NODES, SYNC_ALL_TO_ONE)) >= 0);

		for (int i = 1; i < NITERATIONS; i++)
		{
			test_assert(ksync_signal(syncout) == 0);
			test_assert(ksync_wait(syncin) == 0);
		}
	}

	test_assert(ksync_close(syncout) == 0);
	test_assert(ksync_unlink(syncin) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Create                                                 *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Invalid Create
 */
void test_fault_sync_invalid_create(void)
{
	int nodenum;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = (nodenum == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == (nodenum == MASTER_NODENUM ? SLAVE_NODENUM : MASTER_NODENUM))
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((ksync_create(NULL, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_create(nodes, -1, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_create(nodes, 0, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_create(nodes, 1, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_create(nodes, NR_NODES_MAX + 1, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_create(nodes, NR_NODES, -1)) < 0);
	nodes[0] = -1;
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
	nodes[0] = 1000000;
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
}

/*============================================================================*
 * Fault Test: Bad Create                                                     *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Bad Create
 */
void test_fault_sync_bad_create1(void)
{
	int nodenum;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();

	/* Invalid list of NoC nodes. */
	for (int i = NR_NODES - 1; i >= 0; i--)
		nodes[i] = -1;
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);

	/* Underlying NoC node is the sender. */
	nodes[0] = nodenum;
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((ksync_create(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);

	/* Underlying NoC node is not listed. */
	nodes[0] = (nodenum == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);

	/* Underlying NoC node appears twice in the list. */
	nodes[NR_NODES - 1] = nodenum;
	nodes[NR_NODES - 2] = nodenum;
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
}

/**
 * @brief Fault Test: Synchronization Point Bad Create
 */
void test_fault_sync_bad_create2(void)
{
	int nodenum;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();

	/* Invalid list of NoC nodes. */
	for (int i = NR_NODES - 1; i >= 0; i--)
		nodes[i] = -1;
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) < 0);

	/* Underlying NoC node is not the receiver. */
	nodes[0] = (nodenum == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == (nodenum == MASTER_NODENUM ? SLAVE_NODENUM : MASTER_NODENUM))
			continue;

		nodes[j++] = nodenums[i];
	}
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) < 0);

	/* Underlying NoC node is not listed. */
	nodes[0] = (nodenum == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) < 0);

	/* Underlying NoC node appears twice in the list. */
	nodes[NR_NODES - 1] = nodenum;
	nodes[NR_NODES - 2] = nodenum;
	test_assert((ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) < 0);
}

/**
 * @brief Fault Test: Synchronization Point Bad Create
 */
void test_fault_sync_bad_create(void)
{
	test_fault_sync_bad_create1();
	test_fault_sync_bad_create2();
}

/*============================================================================*
 * Fault Test: Invalid Open                                                   *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Invalid Open
 */
void test_fault_sync_invalid_open(void)
{
	int nodenum;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Build nodes list. */
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((ksync_open(NULL, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_open(nodes, -1, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_open(nodes, 0, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_open(nodes, 1, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_open(nodes, NR_NODES_MAX + 1, SYNC_ONE_TO_ALL)) < 0);
	test_assert((ksync_open(nodes, NR_NODES, -1)) < 0);
	nodes[0] = -1;
	test_assert((ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
	nodes[0] = 1000000;
	test_assert((ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
}

/*============================================================================*
 * Fault Test: Bad Open                                                       *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Bad Open
 */
void test_fault_sync_bad_open1(void)
{
	int nodenum;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Invalid list of NoC nodes. */
	for (int i = NR_NODES - 1; i >= 0; i--)
		nodes[i] = -1;
	test_assert((ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);

	/* Underlying NoC node is not the sender. */
	nodes[NR_NODES - 1] = nodenum;
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}
	test_assert((ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);

	/* Underlying NoC node is not listed. */
	test_assert((ksync_open(nodes, NR_NODES - 1, SYNC_ONE_TO_ALL)) < 0);

	/* Underlying NoC node appears twice in the list. */
	nodes[0] = nodenum;
	nodes[NR_NODES - 1] = nodenum;
	test_assert((ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) < 0);
}

/**
 * @brief Fault Test: Synchronization Point Bad Open
 */
void test_fault_sync_bad_open2(void)
{
	int nodenum;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();

	/* Invalid list of NoC nodes. */
	for (int i = NR_NODES - 1; i >= 0; i--)
		nodes[i] = -1;
	test_assert((ksync_open(nodes, NR_NODES, SYNC_ALL_TO_ONE)) < 0);

	/* Underlying NoC node is not the sender. */
	nodes[0] = nodenum;
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}
	test_assert((ksync_open(nodes, NR_NODES, SYNC_ALL_TO_ONE)) < 0);

	/* Underlying NoC node is not listed. */
	test_assert((ksync_open(&nodes[1], NR_NODES - 1, SYNC_ALL_TO_ONE)) < 0);

	/* Underlying NoC node appears twice in the list. */
	nodes[0] = nodenum;
	nodes[NR_NODES - 1] = nodenum;
	test_assert((ksync_open(&nodes[1], NR_NODES, SYNC_ALL_TO_ONE)) < 0);
}

/**
 * @brief Fault Test: Synchronization Point Bad Open
 */
void test_fault_sync_bad_open(void)
{
	test_fault_sync_bad_open1();
	test_fault_sync_bad_open2();
}

/*============================================================================*
 * Fault Test: Invalid Unlink                                                 *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Invalid Unlink
 */
void test_fault_sync_invalid_unlink(void)
{
	test_assert(ksync_unlink(-1) < 0);
	test_assert(ksync_unlink(1) < 0);
	test_assert(ksync_unlink(1000000) < 0);
}

/*============================================================================*
 * Fault Test: Bad Unlink                                                     *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Bad Unlink
 */
void test_fault_sync_bad_unlink(void)
{
	int nodenum;
	int syncid;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Build nodes list. */
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) >= 0);
	test_assert(ksync_unlink(syncid) < 0);
	test_assert(ksync_close(syncid) == 0);
}

/*============================================================================*
 * Fault Test: Double Unlink                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Double Unlink
 */
void test_fault_sync_double_unlink(void)
{
	int nodenum;
	int syncid;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Build nodes list. */
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) >= 0);
	test_assert(ksync_unlink(syncid) == 0);
	test_assert(ksync_unlink(syncid) < 0);
}

/*============================================================================*
 * Fault Test: Invalid Close                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Invalid Close
 */
void test_fault_sync_invalid_close(void)
{
	test_assert(ksync_close(-1) < 0);
	test_assert(ksync_close(1) < 0);
	test_assert(ksync_close(1000000) < 0);
}

/*============================================================================*
 * Fault Test: Bad Close                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Bad Close
 */
void test_fault_sync_bad_close(void)
{
	int nodenum;
	int syncid;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Build nodes list. */
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) >= 0);
	test_assert(ksync_close(syncid) < 0);
	test_assert(ksync_unlink(syncid) == 0);
}

/*============================================================================*
 * Fault Test: Double Close                                                   *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Double Close
 */
void test_fault_sync_double_close(void)
{
	int nodenum;
	int syncid;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Build nodes list. */
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) >= 0);
	test_assert(ksync_close(syncid) == 0);
	test_assert(ksync_close(syncid) < 0);
}

/*============================================================================*
 * Fault Test: Invalid Signal                                                 *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Invalid Signal
 */
void test_fault_sync_invalid_signal(void)
{
	test_assert(ksync_signal(-1) < 0);
	test_assert(ksync_signal(1) < 0);
	test_assert(ksync_signal(1000000) < 0);
}

/*============================================================================*
 * Fault Test: Bad Signal                                                     *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Bad Signal
 */
void test_fault_sync_bad_signal(void)
{
	int nodenum;
	int syncid;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Build nodes list. */
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_create(nodes, NR_NODES, SYNC_ALL_TO_ONE)) >= 0);
	test_assert(ksync_signal(syncid) < 0);
	test_assert(ksync_unlink(syncid) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Wait                                                   *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Invalid Wait
 */
void test_fault_sync_invalid_wait(void)
{
	test_assert(ksync_wait(-1) < 0);
	test_assert(ksync_wait(1) < 0);
	test_assert(ksync_wait(1000000) < 0);
}

/*============================================================================*
 * Fault Test: Bad Wait                                                       *
 *============================================================================*/

/**
 * @brief Fault Test: Synchronization Point Bad Wait
 */
void test_fault_sync_bad_wait(void)
{
	int nodenum;
	int syncid;
	int nodes[NR_NODES];

	nodenum = processor_node_get_num();
	nodes[0] = nodenum;

	/* Build nodes list. */
	for (int i = 0, j = 1; i < NR_NODES; i++)
	{
		if (nodenums[i] == nodenum)
			continue;

		nodes[j++] = nodenums[i];
	}

	test_assert((syncid = ksync_open(nodes, NR_NODES, SYNC_ONE_TO_ALL)) >= 0);
	test_assert(ksync_wait(syncid) < 0);
	test_assert(ksync_close(syncid) == 0);
}

/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * @brief API tests.
 */
static struct test sync_tests_api[] = {
	{ test_api_sync_create_unlink, "[test][sync][api] sync create/unlink [passed]" },
	{ test_api_sync_open_close,    "[test][sync][api] sync open/close    [passed]" },
	{ test_api_sync_signal_wait,   "[test][sync][api] sync wait          [passed]" },
	{ NULL,                         NULL                                           },
};

/**
 * @brief Fault tests.
 */
static struct test sync_tests_fault[] = {
	{ test_fault_sync_invalid_create, "[test][sync][api] sync invalid create [passed]" },
	{ test_fault_sync_bad_create,     "[test][sync][api] sync bad create     [passed]" },
	{ test_fault_sync_invalid_open,   "[test][sync][api] sync invalid open   [passed]" },
	{ test_fault_sync_bad_open,       "[test][sync][api] sync bad open       [passed]" },
	{ test_fault_sync_invalid_unlink, "[test][sync][api] sync invalid unlink [passed]" },
	{ test_fault_sync_bad_unlink,     "[test][sync][api] sync bad unlink     [passed]" },
	{ test_fault_sync_double_unlink,  "[test][sync][api] sync double unlink  [passed]" },
	{ test_fault_sync_invalid_close,  "[test][sync][api] sync invalid close  [passed]" },
	{ test_fault_sync_bad_close,      "[test][sync][api] sync bad close      [passed]" },
	{ test_fault_sync_double_close,   "[test][sync][api] sync double close   [passed]" },
	{ test_fault_sync_invalid_signal, "[test][sync][api] sync invalid signal [passed]" },
	{ test_fault_sync_bad_signal,     "[test][sync][api] sync bad signal     [passed]" },
	{ test_fault_sync_invalid_wait,   "[test][sync][api] sync invalid wait   [passed]" },
	{ test_fault_sync_bad_wait,       "[test][sync][api] sync bad wait       [passed]" },
	{ NULL,                            NULL                                            },
};

/**
 * The test_thread_mgmt() function launches testing units on thread manager.
 *
 * @author Pedro Henrique Penna
 */
void test_sync(void)
{
	int nodenum;

	nodenum = processor_node_get_num();

	/* API Tests */
	if (nodenum == processor_node_get_num())
		nanvix_puts("--------------------------------------------------------------------------------");
	for (int i = 0; sync_tests_api[i].test_fn != NULL; i++)
	{
		sync_tests_api[i].test_fn();

		if (nodenum == processor_node_get_num())
			nanvix_puts(sync_tests_api[i].name);
	}

	if (nodenum == processor_node_get_num())
	{
		/* Fault Tests */
		nanvix_puts("--------------------------------------------------------------------------------");
		for (int i = 0; sync_tests_fault[i].test_fn != NULL; i++)
		{
			sync_tests_fault[i].test_fn();
			nanvix_puts(sync_tests_fault[i].name);
		}
	}
}

#endif /* __TARGET_SYNC */

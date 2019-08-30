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

#if __TARGET_HAS_MAILBOX

/**
 * @brief Test's parameters
 */
#define NR_NODES       2
#define NR_NODES_MAX   PROCESSOR_NOC_NODES_NUM
#define MASTER_NODENUM 0
#define SLAVE_NODENUM  1

/*============================================================================*
 * API Test: Create Unlink                                                    *
 *============================================================================*/

/**
 * @brief API Test: Mailbox Create Unlink
 */
static void test_api_mailbox_create_unlink(void)
{
	int mbxid;
	int local;

	local = processor_node_get_num();

	test_assert((mbxid = kmailbox_create(local)) >= 0);
	test_assert(kmailbox_unlink(mbxid) == 0);
}

/*============================================================================*
 * API Test: Create Unlink                                                    *
 *============================================================================*/

/**
 * @brief API Test: Mailbox Create Unlink
 */
static void test_api_mailbox_open_close(void)
{
	int mbxid;
	int remote;

	remote = (processor_node_get_num() == MASTER_NODENUM) ?
			 SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((mbxid = kmailbox_open(remote)) >= 0);
	test_assert(kmailbox_close(mbxid) == 0);
}

/*============================================================================*
 * API Test: Read Write 2 CC                                                  *
 *============================================================================*/

/**
 * @brief API Test: Read Write 2 CC
 */
static void test_api_mailbox_read_write(void)
{
	int local;
	int remote;
	int mbx_in;
	int mbx_out;
	char message[MAILBOX_MSG_SIZE];

	local  = processor_node_get_num();
	remote = local == MASTER_NODENUM ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((mbx_in = kmailbox_create(local)) >= 0);
	test_assert((mbx_out = kmailbox_open(remote)) >= 0);

	if (local != MASTER_NODENUM)
	{
		for (unsigned i = 0; i < NITERATIONS; i++)
		{
			kmemset(message, 0, MAILBOX_MSG_SIZE);

			test_assert(kmailbox_aread(mbx_in, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);
			test_assert(kmailbox_wait(mbx_in) == 0);

			for (unsigned j = 0; j < MAILBOX_MSG_SIZE; ++j)
				test_assert(message[j] == 1);

			kmemset(message, 2, MAILBOX_MSG_SIZE);

			test_assert(kmailbox_awrite(mbx_out, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);
			test_assert(kmailbox_wait(mbx_out) == 0);
		}
	}
	else
	{
		for (unsigned i = 0; i < NITERATIONS; i++)
		{
			kmemset(message, 1, MAILBOX_MSG_SIZE);

			test_assert(kmailbox_awrite(mbx_out, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);
			test_assert(kmailbox_wait(mbx_out) == 0);

			kmemset(message, 0, MAILBOX_MSG_SIZE);

			test_assert(kmailbox_aread(mbx_in, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);
			test_assert(kmailbox_wait(mbx_in) == 0);

			for (unsigned j = 0; j < MAILBOX_MSG_SIZE; ++j)
				test_assert(message[j] == 2);
		}
	}

	test_assert(kmailbox_close(mbx_out) == 0);
	test_assert(kmailbox_unlink(mbx_in) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Create                                                 *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Create
 */
static void test_fault_mailbox_invalid_create(void)
{
	int nodenum;

	nodenum = (processor_node_get_num() + 4) % PROCESSOR_NOC_NODES_NUM;

	test_assert(kmailbox_create(-1) < 0);
	test_assert(kmailbox_create(nodenum) < 0);
	test_assert(kmailbox_create(PROCESSOR_NOC_NODES_NUM) < 0);
}

/*============================================================================*
 * Fault Test: Double Create                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Double Create
 */
static void test_fault_mailbox_double_create(void)
{
	int local;
	int mbxid;

	local = processor_node_get_num();

	test_assert((mbxid = kmailbox_create(local)) >=  0);
	test_assert(kmailbox_create(local) < 0);
	test_assert(kmailbox_unlink(mbxid) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Unlink                                                 *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Unlink
 */
static void test_fault_mailbox_invalid_unlink(void)
{
	test_assert(kmailbox_unlink(-1) < 0);
	test_assert(kmailbox_unlink(MAILBOX_CREATE_MAX) < 0);
	test_assert(kmailbox_unlink(1000000) < 0);
}

/*============================================================================*
 * Fault Test: Double Unlink                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Double Unlink
 */
static void test_fault_mailbox_double_unlink(void)
{
	int local;
	int mbxid;

	local = processor_node_get_num();

	test_assert((mbxid = kmailbox_create(local)) >=  0);
	test_assert(kmailbox_unlink(mbxid) == 0);
	test_assert(kmailbox_unlink(mbxid) < 0);
}

/*============================================================================*
 * Fault Test: Invalid Open                                                   *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Open
 */
static void test_fault_mailbox_invalid_open(void)
{
	test_assert(kmailbox_open(-1) < 0);
	test_assert(kmailbox_open(PROCESSOR_NOC_NODES_NUM) < 0);
}

/*============================================================================*
 * Fault Test: Invalid Close                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Close
 */
static void test_fault_mailbox_invalid_close(void)
{
	test_assert(kmailbox_close(-1) < 0);
	test_assert(kmailbox_close(MAILBOX_OPEN_MAX) < 0);
	test_assert(kmailbox_close(1000000) < 0);
}

/*============================================================================*
 * Fault Test: Bad Close                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Bad Close
 */
static void test_fault_mailbox_bad_close(void)
{
	int local;
	int mbxid;

	local = processor_node_get_num();

	test_assert((mbxid = kmailbox_create(local)) >=  0);
	test_assert(kmailbox_close(mbxid) < 0);
	test_assert(kmailbox_unlink(mbxid) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Read                                                   *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Read
 */
static void test_fault_mailbox_invalid_read(void)
{
	char buffer[MAILBOX_MSG_SIZE];

	test_assert(kmailbox_aread(-1, buffer, MAILBOX_MSG_SIZE) < 0);
	test_assert(kmailbox_aread(MAILBOX_CREATE_MAX, buffer, MAILBOX_MSG_SIZE) < 0);
	test_assert(kmailbox_aread(1000000, buffer, MAILBOX_MSG_SIZE) < 0);
}

/*============================================================================*
 * Fault Test: Invalid Read Size                                              *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Read Size
 */
static void test_fault_mailbox_invalid_read_size(void)
{
	int mbxid;
	int local;
	char buffer[MAILBOX_MSG_SIZE];

	local = processor_node_get_num();

	test_assert((mbxid = kmailbox_create(local)) >=  0);
	test_assert(kmailbox_aread(mbxid, buffer, -1) < 0);
	test_assert(kmailbox_aread(mbxid, buffer, 0) < 0);
	test_assert(kmailbox_aread(mbxid, buffer, MAILBOX_MSG_SIZE - 1) < 0);
	test_assert(kmailbox_aread(mbxid, buffer, MAILBOX_MSG_SIZE + 1) < 0);
	test_assert(kmailbox_unlink(mbxid) == 0);
}

/*============================================================================*
 * Fault Test: Null Read                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Null Read
 */
static void test_fault_mailbox_null_read(void)
{
	int mbxid;
	int local;

	local = processor_node_get_num();

	test_assert((mbxid = kmailbox_create(local)) >=  0);
	test_assert(kmailbox_aread(mbxid, NULL, MAILBOX_MSG_SIZE) < 0);
	test_assert(kmailbox_unlink(mbxid) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Write                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Write
 */
static void test_fault_mailbox_invalid_write(void)
{
	char buffer[MAILBOX_MSG_SIZE];

	test_assert(kmailbox_awrite(-1, buffer, MAILBOX_MSG_SIZE) < 0);
	test_assert(kmailbox_awrite(MAILBOX_OPEN_MAX, buffer, MAILBOX_MSG_SIZE) < 0);
	test_assert(kmailbox_awrite(1000000, buffer, MAILBOX_MSG_SIZE) < 0);
}

/*============================================================================*
 * Fault Test: Bad Write                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Bad Write
 */
static void test_fault_mailbox_bad_write(void)
{
	int mbxid;
	int local;
	char buffer[MAILBOX_MSG_SIZE];

	local = processor_node_get_num();

	test_assert((mbxid = kmailbox_create(local)) >=  0);
	test_assert(kmailbox_awrite(mbxid, buffer, MAILBOX_MSG_SIZE) < 0);
	test_assert(kmailbox_unlink(mbxid) == 0);
}

/*============================================================================*
 * Fault Test: Bad Wait                                                       *
 *============================================================================*/

/**
 * @brief Fault Test: Bad Write
 */
static void test_fault_mailbox_bad_wait(void)
{
	test_assert(kmailbox_wait(-1) < 0);
#ifndef __unix64__
	test_assert(kmailbox_wait(MAILBOX_CREATE_MAX) < 0);
	test_assert(kmailbox_wait(MAILBOX_OPEN_MAX) < 0);
#endif
	test_assert(kmailbox_wait(1000000) < 0);
}

/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * @brief Unit tests.
 */
static struct test mailbox_tests_api[] = {
	{ test_api_mailbox_create_unlink, "[test][mailbox][api] mailbox create unlink [passed]" },
	{ test_api_mailbox_open_close,    "[test][mailbox][api] mailbox open close    [passed]" },
	{ test_api_mailbox_read_write,    "[test][mailbox][api] mailbox read write    [passed]" },
	{ NULL,                            NULL                                                 },
};

/**
 * @brief Unit tests.
 */
static struct test mailbox_tests_fault[] = {
	{ test_fault_mailbox_invalid_create,    "[test][mailbox][fault] mailbox invalid create    [passed]" },
	{ test_fault_mailbox_double_create,     "[test][mailbox][fault] mailbox double create     [passed]" },
	{ test_fault_mailbox_invalid_unlink,    "[test][mailbox][fault] mailbox invalid unlink    [passed]" },
	{ test_fault_mailbox_double_unlink,     "[test][mailbox][fault] mailbox double unlink     [passed]" },
	{ test_fault_mailbox_invalid_open,      "[test][mailbox][fault] mailbox invalid open      [passed]" },
	{ test_fault_mailbox_invalid_close,     "[test][mailbox][fault] mailbox invalid close     [passed]" },
	{ test_fault_mailbox_bad_close,         "[test][mailbox][fault] mailbox bad close         [passed]" },
	{ test_fault_mailbox_invalid_read,      "[test][mailbox][fault] mailbox invalid read      [passed]" },
	{ test_fault_mailbox_invalid_read_size, "[test][mailbox][fault] mailbox invalid read size [passed]" },
	{ test_fault_mailbox_null_read,         "[test][mailbox][fault] mailbox null read         [passed]" },
	{ test_fault_mailbox_invalid_write,     "[test][mailbox][fault] mailbox invalid write     [passed]" },
	{ test_fault_mailbox_bad_write,         "[test][mailbox][fault] mailbox bad write         [passed]" },
	{ test_fault_mailbox_bad_wait,          "[test][mailbox][fault] mailbox bad wait          [passed]" },
	{ NULL,                                  NULL                                                       },
};

/**
 * The test_thread_mgmt() function launches testing units on thread manager.
 *
 * @author Pedro Henrique Penna
 */
void test_mailbox(void)
{
	int nodenum;

	nodenum = processor_node_get_num();

	/* API Tests */
	if (nodenum == processor_node_get_num())
		nanvix_puts("--------------------------------------------------------------------------------");
	for (unsigned i = 0; mailbox_tests_api[i].test_fn != NULL; i++)
	{
		mailbox_tests_api[i].test_fn();
		if (nodenum == processor_node_get_num())
			nanvix_puts(mailbox_tests_api[i].name);
	}

	if (nodenum == processor_node_get_num())
	{
		/* Fault Tests */
		nanvix_puts("--------------------------------------------------------------------------------");
		for (unsigned i = 0; mailbox_tests_fault[i].test_fn != NULL; i++)
		{
			mailbox_tests_fault[i].test_fn();
			nanvix_puts(mailbox_tests_fault[i].name);
		}
	}
}

#endif /* __TARGET_HAS_MAILBOX */

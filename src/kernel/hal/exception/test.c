/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Dummy exception handler
 */
static void dummy_handler(const struct exception *excp,
                          const struct context *ctx)
{
    UNUSED(excp);
    UNUSED(ctx);
}

/**
 * @brief Set and Unset and Exception Handler
 */
static void test_exception_set_unset_handler(void)
{
    KASSERT(exception_register(EXCEPTION_PAGE_FAULT, dummy_handler) == 0);
    KASSERT(exception_unregister(EXCEPTION_PAGE_FAULT) == 0);
}

/**
 * Fault Injection Test: Set a Handler for an Invalid Exception
 */
static void test_exception_register_inval(void)
{
    KASSERT(exception_register(-1, dummy_handler) == -1);
    KASSERT(exception_register(EXCEPTIONS_NUM + 1, dummy_handler) == -1);
}

/**
 * Fault Injection Test: Unset a Handler for an Invalid Exception
 */
static void test_exception_unregister_inval(void)
{
    KASSERT(exception_unregister(-1) == -1);
    KASSERT(exception_unregister(EXCEPTIONS_NUM + 1) == -1);
}

/**
 * Fault Injection Test: Unset a Handler for a Bad Exception
 */
static void test_exception_unregister_bad(void)
{
    KASSERT(exception_unregister(EXCEPTION_PAGE_FAULT) == -1);
}

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Unit test.
 */
struct test {
    void (*test_fn)(void); /**< Test function. */
    const char *name;      /**< Test name.     */
};

/**
 * @brief Tests.
 */
static struct test exception_tests[] = {
    {test_exception_set_unset_handler, "set and unset exception handler"},
    {test_exception_register_inval, "set handler for invalid exception  "},
    {test_exception_unregister_inval, "unset handler for invalid exception"},
    {test_exception_unregister_bad, "unset handler for bad exception    "},
    {NULL, NULL},
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Runs self-tests on the exceptions module.
 */
void test_exception(void)
{
    for (int i = 0; exception_tests[i].test_fn != NULL; i++) {
        kprintf(MODULE_NAME " TEST: %s", exception_tests[i].name);
        exception_tests[i].test_fn();
    }
}

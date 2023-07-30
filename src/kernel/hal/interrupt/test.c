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
 * @brief Counter of handler calls.
 */
static volatile int ncalls = 0;

/**
 * @brief Dummy interrupt handler.
 */
static void dummy_handler(void)
{
    ncalls++;
}

/**
 * @brief Attempts to register and unregister an interrupt handler.
 */
static void test_interrupt_register_unregister(void)
{
    KASSERT(interrupt_register(INTERRUPT_TIMER, dummy_handler) == 0);
    KASSERT(interrupt_unregister(INTERRUPT_TIMER) == 0);
}

/**
 * @brief Attempts to enable and disable interrupts.
 */
static void test_interrupt_enable_disable(void)
{
    const int ntrials = 10000000;

    ncalls = 0;

    KASSERT(interrupt_register(INTERRUPT_TIMER, dummy_handler) == 0);

    interrupts_enable();

    while (ncalls == 0) {
        noop();
    }

    interrupts_disable();

    KASSERT(interrupt_unregister(INTERRUPT_TIMER) == 0);

    // Ensure that the handler is not
    // longer called.
    ncalls = 0;
    for (int i = 0; i < ntrials; i++) {
        noop();
        KASSERT(ncalls == 0);
    }
}

/**
 * @brief Attempts to register a handler for an invalid interrupt.
 */
static void test_interrupt_register_handler_inval(void)
{
    KASSERT(interrupt_register(-1, dummy_handler) == -1);
    KASSERT(interrupt_register(INTERRUPTS_NUM + 1, dummy_handler) == -1);
}

/**
 * @brief Attempts to unregister a handler for an invalid interrupt.
 */
static void test_interrupt_unregister_handler_inval(void)
{
    KASSERT(interrupt_unregister(-1) == -1);
    KASSERT(interrupt_unregister(INTERRUPTS_NUM + 1) == -1);
}

/**
 * @brief Attempts to register a handler for an bad interrupt.
 */
static void test_interrupt_register_handler_bad(void)
{
    KASSERT(interrupt_register(INTERRUPT_TIMER, dummy_handler) == 0);
    KASSERT(interrupt_register(INTERRUPT_TIMER, dummy_handler) == -1);
    KASSERT(interrupt_unregister(INTERRUPT_TIMER) == 0);
}

/**
 * @brief Attempts to unregister a handler for an bad interrupt.
 */
static void test_interrupt_unregister_handler_bad(void)
{
    KASSERT(interrupt_unregister(INTERRUPT_TIMER) == -1);
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
 * @brief Test.
 */
static struct test interrupt_tests[] = {
    {test_interrupt_register_unregister, "register and unregister a handler"},
    {test_interrupt_enable_disable, "enable and disable interrupts"},
    {test_interrupt_register_handler_inval,
     "register handler for invalid interrupt"},
    {test_interrupt_unregister_handler_inval,
     "unregister handler for invalid interrupt"},
    {test_interrupt_register_handler_bad, "register handler for bad interrupt"},
    {test_interrupt_unregister_handler_bad,
     "unregister handler for bad interrupt"},
    {NULL, NULL},
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Runs self-tests on the interrupts module.
 */
void test_interrupts(void)
{
    for (int i = 0; interrupt_tests[i].test_fn != NULL; i++) {
        kprintf(MODULE_NAME " TEST: %s", interrupt_tests[i].name);
        interrupt_tests[i].test_fn();
    }
}

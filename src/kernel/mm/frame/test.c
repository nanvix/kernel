/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <stddef.h>

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Unit test.
 */
struct test {
    void (*test_fn)(void); /** Test function. */
    const char *name;      /** Test name.     */
};

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Attempts to allocate more page frames than available.
 */
static void test_frame_allocation_overflow(void)
{
    const frame_t start = USER_BASE_PHYS / PAGE_SIZE;
    const frame_t end = USER_END_PHYS / PAGE_SIZE;

    /* Allocate all user page frames. */
    for (frame_t i = start; i < end; i++) {
        KASSERT(frame_alloc(i) == 0);
    }

    /* Fail to allocate an extra page frame. */
    KASSERT(frame_alloc(end) != 0);

    /* Release all page frames. */
    for (frame_t i = start; i < end; i++) {
        KASSERT(frame_free((i)) == 0);
    }
}

/**
 * @brief Attempts to allocate and release all available page frames.
 */
static void test_frame_allocation_stress(void)
{
    const frame_t start = USER_BASE_PHYS / PAGE_SIZE;
    const frame_t end = USER_END_PHYS / PAGE_SIZE;

    /* Allocate all page frames. */
    for (frame_t i = start; i < end; i++) {
        KASSERT(frame_alloc(i) == 0);
    }

    /* Release all page frames. */
    for (frame_t i = start; i < end; i++) {
        KASSERT(frame_free(i) == 0);
    }
}

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Page Frame unit tests.
 */
static struct test frame_tests[] = {
    {test_frame_allocation_stress, "frame allocation"},
    {test_frame_allocation_overflow, "frame allocation overflow"},
    {NULL, NULL},
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function runs self-tests on the Page Frame Allocator.
 */
void test_frame(void)
{
    for (int i = 0; frame_tests[i].test_fn != NULL; i++) {
        kprintf(MODULE_NAME " TEST: %s", frame_tests[i].name);
        frame_tests[i].test_fn();
    }
}

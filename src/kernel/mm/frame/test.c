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
 * @brief Attempts to allocate a page frame.
 */
static void test_frame_allocation(void)
{
    frame_t frame;

    KASSERT((frame = frame_alloc_any()) != FRAME_NULL);
    KASSERT(frame < ((MEMORY_SIZE) >> PAGE_SHIFT));
    KASSERT(frame_free(frame) == 0);
}

/**
 * @brief Attempts to release an invalid page frame.
 */
static void test_frame_invalid_free(void)
{
    KASSERT(frame_free((MEMORY_SIZE / PAGE_SIZE) + 1) == -1);
    KASSERT(frame_free((USER_BASE_PHYS) >> PAGE_SHIFT) == -1);
}

/**
 * @brief Attempts to release a page frame that was not allocated.
 */
static void test_frame_bad_free(void)
{
    KASSERT(frame_free((USER_BASE_PHYS / PAGE_SIZE)) == -1);
    KASSERT(frame_free((NUM_FRAMES - 1)) == -1);
}

/**
 * @brief Attempts to release a page frame multiple times.
 */
static void test_frame_double_free(void)
{
    frame_t frame;

    KASSERT((frame = frame_alloc_any()) != FRAME_NULL);
    KASSERT(frame_free(frame) == 0);
    KASSERT(frame_free(frame) == -1);
}

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
    {test_frame_allocation, "frame allocation"},
    {test_frame_invalid_free, "invalid frame release"},
    {test_frame_bad_free, "bad frame release"},
    {test_frame_double_free, "double frame release"},
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

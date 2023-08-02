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
 * @brief Attempts to perform a page frame address translation.
 */
static void test_frame_translation(void)
{
    KASSERT(frame_num_to_id(USER_BASE_PHYS >> PAGE_SHIFT) == 0);
    KASSERT(frame_id_to_num(0) == (USER_BASE_PHYS >> PAGE_SHIFT));
}

/**
 * @brief Attempts to allocate a page frame.
 */
static void test_frame_allocation(void)
{
    frame_t frame;

    KASSERT((frame = frame_alloc()) != FRAME_NULL);
    KASSERT(frame >= (USER_BASE_PHYS >> PAGE_SHIFT));
    KASSERT(frame < ((USER_BASE_PHYS + UMEM_SIZE) >> PAGE_SHIFT));
    KASSERT(frame_free(frame) == 0);
}

/**
 * @brief attempts to perform an invalid page frame address translation.
 */
static void test_frame_invalid_translation(void)
{
    KASSERT(frame_num_to_id(KERNEL_BASE_VIRT >> PAGE_SHIFT) == -1);
    KASSERT(frame_num_to_id((USER_BASE_VIRT - PAGE_SIZE) >> PAGE_SHIFT) == -1);
    KASSERT(frame_id_to_num(-1) == FRAME_NULL);
    KASSERT(frame_id_to_num((UMEM_SIZE / PAGE_SIZE) + 1) == FRAME_NULL);
}

/**
 * @brief Attempts to release an invalid page frame.
 */
static void test_frame_invalid_free(void)
{
    KASSERT(frame_free(0) == -1);
    KASSERT(frame_free((USER_BASE_VIRT + UMEM_SIZE) >> PAGE_SHIFT) == -1);
}

/**
 * @brief Attempts to release a page frame that was not allocated.
 */
static void test_frame_bad_free(void)
{
    KASSERT(frame_free(frame_id_to_num(0)) == -1);
    KASSERT(frame_free(frame_id_to_num(NUM_UFRAMES - 1)) == -1);
}

/**
 * @brief Attempts to release a page frame multiple times.
 */
static void test_frame_double_free(void)
{
    frame_t frame;

    KASSERT((frame = frame_alloc()) != FRAME_NULL);
    KASSERT(frame_free(frame) == 0);
    KASSERT(frame_free(frame) == -1);
}

/**
 * @brief Attempts to perform a page frame address translation several times.
 */
static void test_frame_translation_stress(void)
{
    /* Release all page frames. */
    for (frame_t i = 0; i < NUM_UFRAMES; i++) {
        frame_t frame;

        KASSERT((frame = frame_id_to_num(i)) != FRAME_NULL);

        KASSERT(frame >= (USER_BASE_PHYS >> PAGE_SHIFT));
        KASSERT(frame < ((USER_BASE_PHYS + UMEM_SIZE) >> PAGE_SHIFT));
    }
}

/**
 * @brief Attempts to allocate more page frames than available.
 */
static void test_frame_allocation_overflow(void)
{
    /* Allocate all page frames. */
    for (frame_t i = 0; i < NUM_UFRAMES; i++) {
        KASSERT(frame_alloc() != FRAME_NULL);
    }

    /* Fail to allocate an extra page frame. */
    KASSERT(frame_alloc() == FRAME_NULL);

    /* Release all page frames. */
    for (frame_t i = 0; i < NUM_UFRAMES; i++) {
        KASSERT(frame_free(frame_id_to_num(i)) == 0);
    }
}

/**
 * @brief Attempts to allocate and release all available page frames.
 */
static void test_frame_allocation_stress(void)
{
    /* Allocate all page frames. */
    for (frame_t i = 0; i < NUM_UFRAMES; i++) {
        KASSERT(frame_alloc() != FRAME_NULL);
    }

    /* Release all page frames. */
    for (frame_t i = 0; i < NUM_UFRAMES; i++) {
        KASSERT(frame_free(frame_id_to_num(i)) == 0);
    }
}

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Page Frame unit tests.
 */
static struct test frame_tests[] = {
    {test_frame_translation, "frame address translation"},
    {test_frame_allocation, "frame allocation"},
    {test_frame_invalid_translation, "invalid frame address translation"},
    {test_frame_invalid_free, "invalid frame release"},
    {test_frame_bad_free, "bad frame release"},
    {test_frame_double_free, "double frame release"},
    {test_frame_translation_stress, "frame address translation"},
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

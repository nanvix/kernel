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
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Length for bitmap of page frames.
 */
#define FRAMES_LENGTH (NUM_FRAMES / BITMAP_WORD_LENGTH)

/**
 * @brief Size for bitmap of page frames (in bytes)
 */
#define FRAMES_SIZE (FRAMES_LENGTH * sizeof(bitmap_t))

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Bitmap of page frames.
 */
static bitmap_t frames[FRAMES_LENGTH];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Asserts if a frame is valid.
 *
 * @returns If @p frame is valid, non zero is returned. Otherwise,
 * zero is returned instead.
 */
static int frame_is_valid(frame_t frame)
{
    return (frame < NUM_FRAMES);
}

/**
 * @brief Attempts to allocate a specific page frame.
 *
 * @param frame Number of the target page frame.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
static int frame_alloc(frame_t frame)
{
    // Check wether or not target page frame is valid.
    if (!frame_is_valid(frame)) {
        kprintf(MODULE_NAME " ERROR: invalid frame (frame=%x)", frame);
        return (-1);
    }

    const bitmap_t bit = frame;

    // Check wether or not the target page frame is available.
    if (bitmap_check_bit(frames, bit) != 0) {
        kprintf(MODULE_NAME " ERROR: busy frame (frame=%x)", frame);
        return (-1);
    }

    // Allocate request frame.
    bitmap_set(frames, bit);

    return (0);
}

/**
 * @brief Books all page frames within a range.
 *
 * @param base Base address.
 * @param end  End address.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
static int frame_book_range(paddr_t base, paddr_t end)
{
    int ret = 0;

    // Attempt to allocate all page frames within the taget range.
    for (paddr_t addr = base; addr < end; addr += PAGE_SIZE) {
        ret |= frame_alloc(addr / PAGE_SIZE);
    }

    return (ret);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * This function attempts to allocates a page frame using a first-free policy.
 */
frame_t frame_alloc_any(void)
{
    bitmap_t bit;

    /* Search for a free frame. */
    bit = bitmap_first_free(frames, FRAMES_SIZE);

    // Check wether we succeeded to allocate a frame.
    if (bit == BITMAP_FULL) {
        kprintf(MODULE_NAME " ERROR: overflow");
        return (FRAME_NULL);
    }

    bitmap_set(frames, bit);

    return (bit);
}

/**
 * This function releases a page frame that was previously allocated.
 */
int frame_free(frame_t frame)
{
    if (!frame_is_valid(frame))
        return (-1);

    bitmap_t bit = frame;

    // Check wether or not the target page frame was previously allocated.
    if (bitmap_check_bit(frames, bit) == 0) {
        kprintf(MODULE_NAME " ERROR: double free (frame=%x)", frame);
        return (-1);
    }

    bitmap_clear(frames, bit);

    return (0);
}

/*============================================================================*
 * frame_init()                                                               *
 *============================================================================*/

/**
 * The frame_init() function initializes internal structures of the
 * page frame allocator. Additionally, if the kernel is compiled
 * without the @p NDEBUG build option, unit tests on the page frame
 * allocator are launched once its initialization is completed.
 */
void frame_init(void)
{
    kprintf(MODULE_NAME " INFO: initializing the page frame allocator");

    __memset(frames, 0, FRAMES_SIZE);

    // Book all page frames that lie in the kernel address range.
    kprintf(MODULE_NAME " INFO: booking kernel address range");
    if (frame_book_range(KERNEL_BASE_PHYS, KERNEL_END_PHYS) != 0) {
        kpanic("failed to book kernel page frames");
    }

    // Book all page frames that lie in the kernel pool address range.
    kprintf(MODULE_NAME " INFO: booking kpool address range");
    if (frame_book_range(KPOOL_BASE_PHYS, KPOOL_END_PHYS) != 0) {
        kpanic("failed to book kpool page frames");
    }

    test_frame();
}

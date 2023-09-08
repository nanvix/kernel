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

/**
 * @brief Length for bitmap of page frames.
 */
#define FRAMES_LENGTH (NUM_UFRAMES / BITMAP_WORD_LENGTH)

/**
 * @brief Size for bitmap of page frames (in bytes)
 */
#define FRAMES_SIZE (FRAMES_LENGTH * sizeof(bitmap_t))

/**
 * @brief Bitmap of page frames.
 */
static bitmap_t frames[FRAMES_LENGTH];

/**
 * This function asserts whether or not frame @p ID is valid.
 */
int frame_is_valid_id(frame_t id)
{
    return (id < NUM_UFRAMES);
}

/**
 * This function converts frame @p id to a page frame number.
 */
frame_t frame_id_to_num(frame_t id)
{
    /* Invalid ID. */
    if (!frame_is_valid_id(id))
        return (FRAME_NULL);

    return ((USER_BASE_PHYS >> PAGE_SHIFT) + id);
}

/**
 * This function function asserts whether or not the frame number @p frame is
 * valid.
 */
int frame_is_valid_num(frame_t frame)
{
    return ((frame >= (USER_BASE_PHYS >> PAGE_SHIFT)) &&
            (frame < ((USER_BASE_PHYS >> PAGE_SHIFT) + NUM_UFRAMES)));
}

/**
 * This function Converts a page frame number to an ID of a page frame.
 */
int frame_num_to_id(frame_t frame)
{
    /* Invalid frame. */
    if (!frame_is_valid_num(frame)) {
        return (-1);
    }

    return (frame - (USER_BASE_PHYS >> PAGE_SHIFT));
}

/*============================================================================*
 * frame_is_allocated()                                                       *
 *============================================================================*/

/**
 * The frame_is_allocated() function asserts if the target page frame
 * @p frame is allocated.
 */
int frame_is_allocated(frame_t frame)
{
    bitmap_t bit;

    /* Invalid page frame. */
    if (!frame_is_valid_num(frame))
        return (0);

    bit = frame_num_to_id(frame);

    return (bitmap_check_bit(frames, bit));
}

/*============================================================================*
 * frame_alloc()                                                              *
 *============================================================================*/

/**
 * The frame_alloc() function searches sequentially, for a free frame
 * to be allocated. The first free frame is allocated (if any) and its
 * number is returned.
 *
 * @retval FRAME_NULL Cannot allocate another frame.
 */
frame_t frame_alloc(void)
{
    bitmap_t bit;

    /* Search for a free frame. */
    bit = bitmap_first_free(frames, FRAMES_SIZE);

    if (bit == BITMAP_FULL) {
        kprintf("[kernel][mm] page frame overflow");

        return (FRAME_NULL);
    }

    bitmap_set(frames, bit);

    return (frame_id_to_num(bit));
}

/*============================================================================*
 * frame_free()                                                               *
 *============================================================================*/

/**
 * The frame_free() function frees a previously allocated page frame
 * whose number equals to @p frame.
 */
int frame_free(frame_t frame)
{
    bitmap_t bit;

    if (!frame_is_valid_num(frame))
        return (-1);

    bit = frame_num_to_id(frame);

    if (bitmap_check_bit(frames, bit) == 0) {
        kprintf("[kernel][mm] double free on page frame %x", frame);
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

    test_frame();
}

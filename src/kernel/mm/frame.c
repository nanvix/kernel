/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

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

/*============================================================================*
 * frame_is_allocated()                                                       *
 *============================================================================*/

/**
 * The frame_is_allocated() function asserts if the target page frame
 * @p frame is allocated.
 *
 * @author Pedro Henrique Penna
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
 *
 * @author Pedro Henrique Penna
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
 *
 * @author Pedro Henrique Penna
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
 *
 * @author Pedro Henrique Penna
 */
void frame_init(void)
{
    kprintf("[kernel][mm] initializing the page frame allocator");

#ifndef __NANVIX_FAST_BOOT
    __memset(frames, 0, FRAMES_SIZE);
#endif

#ifdef __ENABLE_SELF_TESTS
    kprintf("[kernel][mm] running tests on the page frame allocator");
    frame_test_driver();
#endif
}

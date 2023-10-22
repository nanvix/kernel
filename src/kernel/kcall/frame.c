/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Attempts to allocate a page frame.
 */
frame_t kcall_fralloc(void)
{
    // TODO: Check if the calling process has enough permissions.
    KASSERT_SIZE_LE(sizeof(frame_t), sizeof(word_t));

    return (frame_alloc_any());
}

/**
 * @details Frees a page frame.
 */
int kcall_frfree(frame_t frame)
{
    // TODO: Check if the calling process has enough permissions.

    return (frame_free(frame));
}
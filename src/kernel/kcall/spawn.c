/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/pm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Spawns a new process.
 */
pid_t kcall_spawn(void *image)
{
    // Check if image is valid.
    if (image == NULL) {
        return (-1);
    }

    // TODO: check if image is pinned.

    kprintf("[kernel] spawning process %p", image);

    // Spawn server.
    return (process_create(image));
}

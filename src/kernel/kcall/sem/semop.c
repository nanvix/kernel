/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/pm/process.h>
#include <nanvix/kernel/pm/semaphore.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

#define SEMAPHORE_UP 0
#define SEMAPHORE_DOWN 1
#define SEMAPHORE_TRYLOCK 2

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Do operations in a semaphore.
 */
int kcall_semop(int id, int op)
{
    switch (op) {
        case SEMAPHORE_UP:
            return semaphore_up(id);
        case SEMAPHORE_DOWN:
            return semaphore_down(id);
        case SEMAPHORE_TRYLOCK:
            return semaphore_trylock(id);
        default:
            return (-ENOENT);
    }

    return (-EBADMSG);
}

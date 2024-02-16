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
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Get a semaphore.
 */
int kcall_semget(unsigned key)
{
    int semid = -1;
    int ret = semaphore_create(key);

    // Try create a semaphore.
    switch (ret) {
        case 0:
            return (0);
        case -1:
            return (-1);
        default:
            semid = ret;
            break;
    }

    // Try get semaphore.
    ret = semaphore_get(semid);
    switch (ret) {
        case 0:
            return (0);
        default:
            return (-1);
    }

    return (-1);
}

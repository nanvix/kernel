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
    int semid = -EINVAL;

    // Try create a semaphore.
    int ret = semaphore_create(key);
    if (ret < 0) {
        switch (ret) {
            case -EEXIST:
                return (semaphore_getid(key));
            case -ENOBUFS:
                return (-ENOBUFS);
            default:
                return (-EBADMSG);
        }
    }

    // Try get semaphore and return.
    semid = ret;
    return (semaphore_get(semid));
}

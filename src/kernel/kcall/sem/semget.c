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

/**
 * @brief Semaphore key Exist
 */
#define SEMAPHORE_KEYEXIST -2

/**
 * @brief Semaphore Buffer is Full
 */
#define SEMAPHORE_ENOBUFS -1

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
        case SEMAPHORE_KEYEXIST:
            // Return semaphore id if success in get semaphore or return error
            semaphore_getid(key) >= 0 ? (ret = semaphore_getid(key))
                                      : (ret = -ENOBUFS);
            return (ret);
        case SEMAPHORE_ENOBUFS:
            return (-ENOBUFS);
        default:
            semid = ret;
            break;
    }

    // Try get semaphore.
    ret = semaphore_get(semid);
    if (ret != -1) {
        return (ret);
    }

    return (-ENOENT);
}
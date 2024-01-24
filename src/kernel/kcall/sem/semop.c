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
 * @details Do operations in a semaphore.
 */
int kcall_semop(int id, int op)
{
    // TODO: https://github.com/nanvix/microkernel/issues/392

    // Return to test syscall.
    return (id + op);
}

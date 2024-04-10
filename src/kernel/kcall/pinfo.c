/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/pm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Gets various information about the calling process.
 */
int kcall_pinfo(struct process_info *buf)
{
    // TODO: check if storage location is valid.
    if (buf == NULL) {
        return (-EINVAL);
    }

    const struct process *p = process_get_curr();

    // Copy relevant information.
    buf->pid = p->pid;
    buf->tid = p->tid;
    buf->vmem = p->vmem;

    return (0);
}

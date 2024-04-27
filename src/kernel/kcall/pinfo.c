/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/kcall.h>
#include <nanvix/kernel/log.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Gets various information about a process.
 */
int kcall_pinfo(pid_t pid, struct process_info *buf)
{
    const struct process *p =
        (pid == PID_SELF) ? process_get_curr() : process_get(pid);

    // Check for invalid process.
    if (p == NULL) {
        error("no such process %d", pid);
        return (-ENOENT);
    }

    // Check for invalid storage location.
    if (buf == NULL) {
        error("invalid storage location %x", buf);
        return (-EINVAL);
    }

    // Check for bad storage location.
    if (!mm_check_area(VADDR(buf), sizeof(struct process_info), UMEM_AREA)) {
        error("bad storage location %x", buf);
        return (-EFAULT);
    }

    trace("pinfo(): pid=%d, buf=%x", pid, buf);

    // Copy relevant information.
    buf->pid = p->pid;
    buf->tid = p->tid;
    buf->vmem = p->vmem;

    return (0);
}

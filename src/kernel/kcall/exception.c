/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/excp.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/log.h>
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Controls which action to take when an exception happens.
 */
int kcall_excpctrl(int excpnum, int action)
{
    trace("excpnum=%d, action=%x", excpnum, action);

    // NOTE: excpnum is checked in excp_control().
    // NOTE: action is checked in excp_control().

    return (excp_control(excpnum, action));
}

/**
 * @details Resumes the execution of faulting process.
 */
int kcall_excpresume(int excpnum)
{
    trace("excpnum=%d", excpnum);

    // Check for invalid exception number.
    if (!WITHIN(excpnum, 0, EXCEPTIONS_NUM)) {
        error("invalid exception number %d", excpnum);
        return (-EINVAL);
    }

    return (excp_resume(excpnum));
}

/**
 * @details Waits for an exception to happen.
 */
int kcall_excpwait(struct excpinfo *info)
{
    trace("info=%x", info);

    // Check for invalid storage location.
    if (info == NULL) {
        error("invalid storage location %x", info);
        return (-EINVAL);
    }

    // Check for bad storage location.
    if (!mm_check_area(VADDR(info), sizeof(struct excpinfo), UMEM_AREA)) {
        error("invalid storage location %x", info);
        return (-EFAULT);
    }

    return (excp_wait(info));
}

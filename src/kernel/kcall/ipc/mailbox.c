/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/cc.h>
#include <nanvix/errno.h>
#include <nanvix/kernel/ipc/mailbox.h>
#include <nanvix/kernel/lib.h>

// #include <nanvix/kernel/ipc/mailbox.h>
// #include <nanvix/cc.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Gets mailbox tag.
 */
int kcall_mailbox_tag(int mbxid)
{
    UNUSED(mbxid);
    // TODO: https://github.com/nanvix/microkernel/issues/400
    // Return to test syscall.
    return (ENOTSUP);
}

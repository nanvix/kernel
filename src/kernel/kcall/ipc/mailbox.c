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

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Creates a mailbox.
 */
int kcall_mailbox_create(const int owner, const int tag)
{
    return do_mailbox_create(owner, tag);
}

/**
 * @details Opens an existing mailbox.
 */
int kcall_mailbox_open(const int owner, const int tag)
{
    return do_mailbox_open(owner, tag);
}

/**
 * @details Removes a mailbox.
 */
int kcall_mailbox_unlink(const int ombxid)
{
    return do_mailbox_unlink(ombxid);
}

/**
 * @details Closes a mailbox.
 */
int kcall_mailbox_close(const int ombxid)
{
    return do_mailbox_close(ombxid);
}

/**
 * @details Writes a message to a mailbox.
 */
int kcall_mailbox_write(const int ombxid, const void *buffer, const size_t sz)
{
    return do_mailbox_write(ombxid, buffer, sz);
}

/**
 * @details Reads a message from a mailbox.
 */
int kcall_mailbox_read(const int ombxid, void *buffer, const size_t sz)
{
    return do_mailbox_read(ombxid, buffer, sz);
}

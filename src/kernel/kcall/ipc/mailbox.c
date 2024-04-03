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
 * @brief Creates a mailbox.
 */
int kcall_do_mailbox_create(const int owner, const int tag)
{
    return do_mailbox_create(owner, tag);
}

/**
 * @brief Opens an existing mailbox.
 */
int kcall_do_mailbox_open(const int owner, const int tag)
{
    return do_mailbox_open(owner, tag);
}

/**
 * @brief Removes a mailbox.
 */
int kcall_do_mailbox_unlink(const int ombxid)
{
    return do_mailbox_unlink(ombxid);
}

/**
 * @brief Closes a mailbox.
 */
int kcall_do_mailbox_close(const int ombxid)
{
    return do_mailbox_close(ombxid);
}

/**
 * @brief Writes a message to a mailbox.
 */
int kcall_do_mailbox_write(const int ombxid, const void *buffer,
                           const size_t sz)
{
    return do_mailbox_write(ombxid, buffer, sz);
}

/**
 * @brief Reads a message from a mailbox.
 */
int kcall_do_mailbox_read(const int ombxid, void *buffer, const size_t sz)
{
    return do_mailbox_read(ombxid, buffer, sz);
}
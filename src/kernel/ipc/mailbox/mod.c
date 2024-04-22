/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                   *
 *============================================================================*/

#include "mod.h"
#include <nanvix/errno.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <stddef.h>

/*============================================================================*
 * Friend-Functions                                                           *
 *============================================================================*/

/**
 * @brief Pushes a message into a mailbox.
 *
 * @param mbxid ID of the target mailbox.
 * @param msg Message to be pushed.
 * @param sz Size of the message.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mailbox_push(const int mbxid, const void *msg, const size_t sz);

/**
 * @brief Pop a message from a mailbox.
 *
 * @param mbxid ID of the target mailbox.
 * @param msg Storage location for the message.
 * @param sz Size of the message.

 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mailbox_pop(const int mbxid, void *msg, const size_t sz);

/**
 * @brief Lookups a mailbox ID on the table of open mailboxes.
 *
 * @param ombxid ID of the open mailbox.
 *
 * @return Upon successful completion the ID of the target mailbox is returned.
 * Upon failure, a negative error code is returned instead.
 */
extern int omailboxes_lookup(const int ombxid);

/**
 * @brief Gets an open mailbox based on its owner and tag.
 *
 * @param owner Owner of the mailbox.
 * @param tag Tag of the mailbox.
 * @param create Create the mailbox if it does not exist?
 *
 * @return Upon successful completion, the ID of the allocated open mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
extern int omailboxes_get(const pid_t owner, const int tag, const bool create);

/**
 * @brief Releases a mailbox.
 *
 * @param ombxid ID of open mailbox.
 * @param unlink Unlink mailbox?
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int omailboxes_release(const int ombxid, const bool unlink);

/**
 * @brief Initializes the table of open mailboxes.
 */
extern void omailbox_init(void);

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Creates a mailbox.
 *
 * @param owner PID of the processing owning the target mailbox.
 * @param tag Tag of the target mailbox.
 *
 * @returns Upon successful completion, the ID of the target open mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
int do_mailbox_create(const pid_t owner, const int tag)
{
    return (omailboxes_get(owner, tag, true));
}

/**
 * @brief Opens an existing mailbox.
 *
 * @param owner PID of the processing owning the target mailbox.
 * @param tag Tag of the target mailbox.
 *
 * @returns Upon successful completion, the ID of the target mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
int do_mailbox_open(const pid_t owner, const int tag)
{
    return (omailboxes_get(owner, tag, false));
}

/**
 * @brief Removes a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int do_mailbox_unlink(const int ombxid)
{
    return (omailboxes_release(ombxid, true));
}

/**
 * @brief Closes a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int do_mailbox_close(const int ombxid)
{
    return (omailboxes_release(ombxid, false));
}

/**
 * @brief Writes a message to a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 * @param buffer Buffer where the data should be read from.
 * @param sz Number of bytes to write.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int do_mailbox_write(const int ombxid, const void *buffer, const size_t sz)
{
    // Lookup target mailbox.
    const int mbxid = omailboxes_lookup(ombxid);
    if (mbxid < 0) {
        return (mbxid);
    }

    // Invalid buffer.
    if (buffer == NULL) {
        log(ERROR, "invalid buffer (buffer=%p)", buffer);
        return (-EINVAL);
    }

    // Invalid write size.
    if ((sz == 0) || (sz > MAILBOX_MESSAGE_SIZE)) {
        log(ERROR, "invalid message size (sz=%d)", sz);
        return (-EINVAL);
    }

    // Bad buffer location.
    if (!mm_check_area(VADDR(buffer), sz, UMEM_AREA)) {
        log(ERROR, "invalid buffer location (buffer=%p)", buffer);
        return (-EFAULT);
    }

    // TODO: pin user memory.

    return (mailbox_push(mbxid, buffer, sz));
}

/**
 * @brief Reads a message from a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 * @param buffer Buffer where the data should be written to.
 * @param sz Number of bytes to read.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int do_mailbox_read(const int ombxid, void *buffer, const size_t sz)
{
    // Lookup target mailbox.
    const int mbxid = omailboxes_lookup(ombxid);
    if (mbxid < 0) {
        return (mbxid);
    }

    // Invalid buffer.
    if (buffer == NULL) {
        log(ERROR, "invalid buffer (buffer=%p)", buffer);
        return (-EINVAL);
    }

    // Invalid write size.
    if ((sz == 0) || (sz > MAILBOX_MESSAGE_SIZE)) {
        log(ERROR, "invalid message size (sz=%d)", sz);
        return (-EINVAL);
    }

    // Bad buffer location.
    if (!mm_check_area(VADDR(buffer), sz, UMEM_AREA)) {
        log(ERROR, "invalid buffer location (buffer=%p)", buffer);
        return (-EFAULT);
    }

    // TODO: pin user memory.

    return (mailbox_pop(mbxid, buffer, sz));
}

/**
 * @brief Initializes the mailbox module.
 */
void mailbox_init(void)
{
    static bool initialized = false;

    // Nothing to do.
    if (initialized) {
        log(WARN, "trying to initialize module again?");
        return;
    }

    log(INFO, "initializing module");

    // Initialize the table of open mailboxes.
    omailbox_init();
}

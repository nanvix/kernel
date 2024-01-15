/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                   *
 *============================================================================*/

#include "mod.h"
#include <nanvix/errno.h>
#include <nanvix/kernel/log.h>
#include <nanvix/kernel/pm.h>
#include <stdbool.h>
#include <stddef.h>

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief A message.
 */
struct message {
    size_t size;                           /** Size   */
    uint8_t payload[MAILBOX_MESSAGE_SIZE]; /* Payload */
};

/**
 * @brief A mailbox.
 */
struct mailbox {
    pid_t owner;                           /** Owner           */
    int refcount;                          /** Reference Count */
    int tag;                               /** Tag             */
    int head;                              /** Head            */
    int tail;                              /** Tail            */
    struct condvar readers;                /** Readers         */
    struct message messages[MAILBOX_SIZE]; /** Messages        */
};

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Writes a message.
 *
 * @param msg Target message.
 * @param buf Source buffer.
 * @param size Size of source buffer.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
static int message_write(struct message *msg, const void *buf, const size_t sz)
{
    // Sanity check parameters.
    KASSERT(msg != NULL);
    KASSERT(buf != NULL);
    KASSERT((sz > 0) && (sz <= MAILBOX_MESSAGE_SIZE));

    msg->size = sz;
    __memcpy(msg->payload, buf, sz);

    return (0);
}

/**
 * @brief Reads a message.
 *
 * @param msg Target message.
 * @param buf Storage buffer.
 * @param sz Size of storage buffer.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
static int message_read(const struct message *msg, void *buf, const size_t sz)
{
    // Sanity check parameters.
    KASSERT(msg != NULL);
    KASSERT(buf != NULL);
    KASSERT(sz >= msg->size);

    __memcpy(buf, msg->payload, msg->size);

    return (0);
}

/**
 * @brief Retrieves a mailbox based on its id.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @return Upon successful completion, a pointer to the target mailbox is
 * returned. Upon failure, a NULL pointer is returned instead.
 */
static struct mailbox *mailboxes(int mbxid)
{
    static struct mailbox mailboxes[MAILBOX_MAX];

    // Check if mailbox ID is valid.
    if (!WITHIN(mbxid, 0, MAILBOX_MAX)) {
        log(ERROR, "invalid mailbox ID (mbxid=%d)", mbxid);
        return (NULL);
    }

    return (&mailboxes[mbxid]);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * Checks wether a mailbox is assigned.
 */
int mailbox_is_assigned(const int mbxid)
{
    const struct mailbox *mbx = mailboxes(mbxid);
    if (mbx == NULL) {
        return (0);
    }

    return (mbx->refcount > 0);
}

/**
 * Gets the owner of a mailbox.
 */
pid_t mailbox_owner(const int mbxid)
{
    // Ensure that the target mailbox is assigned.
    if (!mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is not assigned (mbxid=%d)", mbxid);
        return (-EBADF);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is assigned.
    const struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    return (mbx->owner);
}

/**
 * Gets the tag of a mailbox.
 */
int mailbox_tag(const int mbxid)
{
    // Ensure that the target mailbox is assigned.
    if (!mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is not assigned (mbxid=%d)", mbxid);
        return (-EBADF);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is assigned.
    struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    return (mbx->tag);
}

/**
 * Initializes a mailbox with the default values.
 */
int mailbox_default(const int mbxid)
{
    // Ensure that the target mailbox is NOT assigned.
    if (mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is assigned (mbxid=%d", mbxid);
        return (-EBADF);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is NOT assigned.
    struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    // Initialize all fields of the mailbox.
    mbx->owner = 0;
    mbx->refcount = 0;
    mbx->tag = 0;
    mbx->head = 0;
    mbx->tail = 0;
    cond_init(&mbx->readers);
    __memset(mbx->messages->payload, 0, sizeof(mbx->messages->payload));

    return (0);
}

/**
 * Assigns a mailbox.
 */
int mailbox_assign(const int mbxid, const pid_t owner, const int tag)
{
    // Ensure that the target mailbox is NOT assigned.
    if (mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is assigned (mbxid=%d)", mbxid);
        return (-EBADF);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is NOT assigned.
    struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    // Initialize relevant fiends of a mailbox.
    // Other fields should have their default values.
    KASSERT(mbx->owner == 0);
    mbx->owner = owner;
    mbx->refcount = 1;
    mbx->tag = tag;
    KASSERT(mbx->head == 0);
    KASSERT(mbx->tail == 0);
    // TODO: assert default value of condvar.
    __memset(mbx->messages->payload, 0, sizeof(mbx->messages->payload));

    return (0);
}

/**
 * Links a mailbox.
 */
int mailbox_link(const int mbxid)
{
    // Ensure that the target mailbox is assigned.
    if (!mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is not assigned (mbxid=%d)", mbxid);
        return (-EBADF);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is assigned.
    struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    // Increment reference count.
    mbx->refcount++;

    return (0);
}

/**
 * Unlinks a mailbox.
 */
int mailbox_unlink(const int mbxid)
{
    // Ensure that the target mailbox is assigned.
    if (!mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is not assigned (mbxid=%d)", mbxid);
        return (-EBADF);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is assigned.
    struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    // Decrement reference count. If it reaches zero, reset the mailbox.
    if (--mbx->refcount == 0) {
        KASSERT(mailbox_default(mbxid) == 0);
    }

    return (0);
}

/**
 * Pushes a message into a mailbox.
 */
int mailbox_push(const int mbxid, const void *msg, const size_t sz)
{
    // Ensure that the target mailbox is assigned.
    if (!mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is not assigned");
        return (-EBADF);
    }

    // Ensure that the message is valid.
    if (msg == NULL) {
        log(ERROR, "invalid message (msg=%p)", msg);
        return (-EINVAL);
    }

    // Ensure that the size of the message is valid.
    if (sz == 0) {
        log(ERROR, "message cannot have zero-length size (sz=%d)", sz);
        return (-EINVAL);
    }

    // Ensure that the message is not too big.
    if (sz > MAILBOX_MESSAGE_SIZE) {
        log(ERROR, "message is too big (sz=%d)", sz);
        return (-E2BIG);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is assigned.
    struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    // Ensure that the mailbox is not full.
    if (mbx->head == (mbx->tail + 1) % MAILBOX_SIZE) {
        log(ERROR, "mailbox is full (mbxid=%d)", mbxid);
        return (-EAGAIN);
    }

    // Write the message to the mailbox.
    KASSERT(message_write(&mbx->messages[mbx->tail], msg, sz) == 0);

    // Update the tail.
    mbx->tail = (mbx->tail + 1) % MAILBOX_SIZE;

    // Wake up blocked readers.
    KASSERT(cond_broadcast(&mbx->readers) == 0);

    return (0);
}

/**
 * Pop a message from a mailbox.
 */
int mailbox_pop(const int mbxid, void *msg, const size_t sz)
{
    // Ensure that the target mailbox is assigned.
    if (!mailbox_is_assigned(mbxid)) {
        log(ERROR, "mailbox is not assigned");
        return (-EBADF);
    }

    // Ensure that the message is valid.
    if (msg == NULL) {
        log(ERROR, "invalid message (msg=%p)", msg);
        return (-EINVAL);
    }

    // Get the target mailbox.
    // The assertion should not fail, because the same check was performed
    // when we checked whether the mailbox is assigned.
    struct mailbox *mbx = mailboxes(mbxid);
    KASSERT(mbx != NULL);

    // Check if the mailbox is empty.
    while (mbx->head == mbx->tail) {
        cond_wait(&mbx->readers);
    }

    const struct message *_msg = &mbx->messages[mbx->head];

    // Ensure that the storage location for the message is big enough.
    if (sz < _msg->size) {
        log(ERROR, "storage location for message is too small (sz=%d)", sz);
        return (-EINVAL);
    }

    // Read the message from the mailbox.
    KASSERT(message_read(_msg, msg, sz) == 0);

    // Update the head.
    mbx->head = (mbx->head + 1) % MAILBOX_SIZE;

    return (0);
}

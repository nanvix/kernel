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
 * Friend-Functions                                                           *
 *============================================================================*/

/**
 * @brief Unlinks a mailbox.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mailbox_unlink(const int mbxid);

/**
 * @brief Gets a mailbox based on its owner and tag.
 *
 * @param owner Owner of the mailbox.
 * @param tag  Tag of the mailbox.
 * @param create Create the mailbox if it does not exist?
 *
 * @return Upon successful completion, the ID of the the target mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
extern int mailbox_get(const pid_t owner, const int tag, const bool create);

/**
 * @brief Initializes the table of mailboxes.
 */
extern void mailboxes_init(void);

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum number of open mailboxes per process.
 */
#define MAILBOX_OPEN_MAX 16

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Open mailboxes.
 */
static struct {
    int mailboxes[MAILBOX_OPEN_MAX]; /** Per-process open mailboxes */
} open[PROCESS_MAX];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Allocates an entry on the table of open mailboxes.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @return Upon successful completion, the ID of the allocated open mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
static int omailboxes_alloc(const int mbxid)
{
    const pid_t mypid = process_get_curr()->pid;

    // Get an empty slot in the table of open mailboxes.
    for (int i = 0; i < MAILBOX_OPEN_MAX; i++) {
        if (open[mypid].mailboxes[i] == -1) {
            open[mypid].mailboxes[i] = mbxid;
            return (i);
        }
    }

    // No entry is available.
    log(ERROR, "cannot open more mailboxes (pid=%d)", mypid);
    return (-ENFILE);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * Lookups a mailbox ID on the table of open mailboxes.
 */
int omailboxes_lookup(const int ombxid)
{
    // Ensure that the ID of the target open mailbox is valid.
    if (!WITHIN(ombxid, 0, MAILBOX_OPEN_MAX)) {
        log(ERROR, "invalid ID of open mailbox (ombxid=%d)", ombxid);
        return (-EINVAL);
    }

    return (open[process_get_curr()->pid].mailboxes[ombxid]);
}

/**
 * Gets an open mailbox based on its owner and tag.
 */
int omailboxes_get(const pid_t owner, const int tag, const bool create)
{
    const pid_t mypid = process_get_curr()->pid;

    // Check if the calling process has an open mailbox with the same tag.
    for (int i = 0; i < MAILBOX_OPEN_MAX; i++) {
        // Skip invalid entries.
        if (open[mypid].mailboxes[i] < 0) {
            continue;
        }

        // Check if owners mismatch.
        if (mailbox_owner(open[mypid].mailboxes[i]) != owner) {
            continue;
        }

        // Check if tags mismatch.
        if (mailbox_tag(open[mypid].mailboxes[i]) != tag) {
            continue;
        }

        // Found.
        return (i);
    }

    // Target mail box not found. Create it, if requested.
    if (create) {
        // Retrieve existing mailbox.
        const int mbxid = mailbox_get(owner, tag, create);
        if (mbxid < 0) {
            return (mbxid);
        }

        // Add the mailbox to the table of open mailboxes. If we fail, rollback.
        const int ombxid = omailboxes_alloc(mbxid);
        if (ombxid < 0) {
            // The assertion should not fail, because we have previously
            // checked all conditions that would cause it to fail.
            KASSERT(mailbox_unlink(mbxid) == 0);
        }

        return (ombxid);
    }

    // Target mailbox not found.
    log(ERROR, "mailbox not found (owner=%d, tag=%d)", owner, tag);
    return (-ENOENT);
}

/**
 * @brief Releases a mailbox.
 *
 * @param ombxid ID of open mailbox.
 * @param unlink Unlink mailbox?
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int omailboxes_release(const int ombxid, const bool unlink)
{
    const int mbxid = omailboxes_lookup(ombxid);
    // Lookup target mailbox.
    if (mbxid < 0) {
        return (-ENOENT);
    }

    const pid_t mypid = process_get_curr()->pid;

    // Unlink mailbox if requested.
    if (unlink) {
        // Check if the calling process is the owner of the target mailbox.
        if (mailbox_owner(mbxid) != mypid) {
            log(ERROR, "permission denied (ombxid=%d, pid=%d)", ombxid, mypid);
            return (-EPERM);
        }
    }

    // Remove mailbox from the per-process table of open mailboxes.
    open[mypid].mailboxes[ombxid] = -1;

    // FIXME: enable the owner of the mailbox to release it without unlinking?

    return (mailbox_unlink(mbxid));
}

/**
 * Initializes the table of open mailboxes.
 */
void omailbox_init(void)
{
    static bool initialized = false;

    // Nothing to do.
    if (initialized) {
        log(WARN, "trying to initialize component again?");
        return;
    }

    log(INFO, "initializing component");

    mailboxes_init();

    // Initialize the table of open mailboxes.
    for (int i = 0; i < PROCESS_MAX; i++) {
        for (int j = 0; j < MAILBOX_OPEN_MAX; j++) {
            open[i].mailboxes[j] = -1;
        }
    }
}

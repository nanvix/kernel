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
 * @brief Checks wether the target mailbox is assigned.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @return Non-zero if the target mailbox is assigned, and zero otherwise.
 */
extern int mailbox_is_assigned(const int mbxid);

/**
 * @brief Assigns a mailbox.
 *
 * @param mbxid ID of the target mailbox.
 * @param owner Owner of the mailbox.
 * @param tag Tag of the mailbox.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mailbox_assign(const int mbxid, const pid_t owner, const int tag);

/**
 * @brief Links a mailbox.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mailbox_link(const int mbxid);

/**
 * @brief Initializes a mailbox with the default values.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @return This function always return zero.
 */
extern int mailbox_default(const int mbxid);

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Allocates a mailbox.
 *
 * @param owner Owner of the mailbox.
 * @param tag Tag of the mailbox.

 * @return Upon successful completion, the ID of the the allocated mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
static int mailbox_alloc(const pid_t owner, const int tag)
{
    // Search for a mailbox that is not assigned.
    for (int mbxid = 0; mbxid < MAILBOX_MAX; mbxid++) {
        // Skip assigned mailboxes.
        if (mailbox_is_assigned(mbxid)) {
            continue;
        }

        // Assign mailbox.
        // The assertion should not fail, because we have previously
        // checked all conditions that would cause it to fail.
        KASSERT(mailbox_assign(mbxid, owner, tag) == 0);

        return (mbxid);
    }

    // No unassigned mailboxes are available.
    log(ERROR, "no unassigned mailboxes are available");
    return (-EMFILE);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * Gets a mailbox based on its owner and tag.
 */
int mailbox_get(const pid_t owner, const int tag, const bool create)
{
    // Get target mailbox.
    for (int mbxid = 0; mbxid < MAILBOX_MAX; mbxid++) {
        // Skip invalid unassigned mailboxes.
        if (!mailbox_is_assigned(mbxid)) {
            continue;
        }

        // Check if owners mismatch.
        if (mailbox_owner(mbxid) != owner) {
            continue;
        }

        // Check if tags mismatch.
        if (mailbox_tag(mbxid) != tag) {
            continue;
        }

        // Link mailbox.
        // The assertion should not fail, because we have previously
        // checked all conditions that would cause it to fail.
        KASSERT(mailbox_link(mbxid) == 0);
        return (mbxid);
    }

    // Target mailbox not found. Create it, if requested.
    if (create) {
        return (mailbox_alloc(owner, tag));
    }

    // Target mailbox not found.
    log(ERROR, "mailbox not found (owner=%d, tag=%d)", owner, tag);
    return (-ENOENT);
}

/**
 * Initializes the table of mailboxes.
 */
void mailboxes_init(void)
{
    static bool initialized = false;

    // Nothing to do.
    if (initialized) {
        log(WARN, "trying to initialize component again?");
        return;
    }

    log(INFO, "initializing component");

    // Initialize all mailboxes with the default value.
    for (int i = 0; i < MAILBOX_MAX; i++) {
        mailbox_default(i);
    }
}

/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                   *
 *============================================================================*/

#include "mod.h"
#include <nanvix/errno.h>
#include <nanvix/kernel/limits.h>
#include <nanvix/kernel/log.h>
#include <nanvix/libcore.h>
#include <nanvix/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*============================================================================*
 * Structure                                                                  *
 *============================================================================*/

/**
 * @brief Identity.
 */
struct identity {
    uid_t uid;  /** Real user ID.            */
    gid_t gid;  /** Real user group ID.      */
    uid_t euid; /** Effective user ID.       */
    gid_t egid; /** Effective user group ID. */
    uid_t suid; /** Saved user ID.           */
    gid_t sgid; /** Saved user group ID.     */
};

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Table of identities.
 */
static struct {
    bool used[PROCESS_MAX];                  /** Used entry? */
    struct identity identities[PROCESS_MAX]; /** Identities  */
} table;

/**
 * @brief Root identity.
 */
static const struct identity *ROOT = NULL;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Gets the offset of an identity in the table of identities.
 *
 * @param id Target identity.
 *
 * @return The function returns the offset of the identity pointed to by @p id
 * in the table of identities. If the target identity is invalid, a negative
 * error code is returned instead.
 */
static int identity_offset(const struct identity *id)
{
    const uintptr_t base = (uintptr_t)table.identities;
    const uintptr_t end = base + sizeof(table.identities);
    const uintptr_t ptr = (uintptr_t)id;

    // Check if target identity is within the table of identities.
    if ((ptr < base) || (ptr >= end)) {
        error("identity out of bounds");
        return (-EINVAL);
    }

    // Check if target identity is properly aligned.
    if (((ptr - base) % sizeof(struct identity)) != 0) {
        error("identity misaligned");
        return (-EINVAL);
    }

    // Return offset.
    return ((ptr - base) / sizeof(struct identity));
}

/**
 * @brief Checks if an identity is valid.
 *
 * @param id Target identity.
 *
 * @return The function returns @p true if the identity pointed to by @p id is
 * valid, and @p false otherwise.
 */
static bool identity_is_valid(const struct identity *id)
{
    // Check if target identity is NULL.
    if (id == NULL) {
        error("identity is NULL");
        return (false);
    }

    const int i = identity_offset(id);

    // Check if offset is valid.
    if (i < 0) {
        return (false);
    }

    // Check if identity is used.
    return (table.used[i]);
}

/**
 * @brief Allocates a new identity.
 *
 * @return On successful completion, a pointer to the newly allocated identity
 * is returned. On error, @p NULL is returned instead.
 */
static struct identity *identity_alloc(void)
{
    // Search for an unused entry in the table of identities.
    for (int i = 0; i < PROCESS_MAX; i++) {
        // Found it.
        if (!table.used[i]) {
            // Allocate entry and return corresponding identity.
            table.used[i] = true;
            return (&table.identities[i]);
        }
    }

    // Table overflow, log an error and return NULL.
    error("identity table overflow");
    return (NULL);
}

/**
 * @brief Frees an identity.
 *
 * @param id Target identity.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
static int identity_free(struct identity *id)
{
    const int i = identity_offset(id);

    // Check if offset is valid.
    if (i < 0) {
        return (-EINVAL);
    }

    // Check if identity is used.
    if (!table.used[i]) {
        error("identity is not used");
        return (-EINVAL);
    }

    // Free identity.
    table.used[i] = false;
    __memset(id, 0, sizeof(struct identity));

    return (0);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Gets the root identity.
 */
const struct identity *identity_root(void)
{
    return (ROOT);
}

/**
 * @details Allocates a new identity and initializes it.
 */
struct identity *identity_new(const struct identity *baseid)
{
    struct identity *id = NULL;

    // Check if base identity is valid.
    if (!identity_is_valid(baseid)) {
        return (NULL);
    }

    // Allocate identity.
    if ((id = identity_alloc()) == NULL) {
        return (NULL);
    }

    // Initialize identity.
    __memcpy(id, baseid, sizeof(struct identity));

    return (id);
}

/**
 * @details Releases a previously allocated identity.
 */
int identity_drop(struct identity *id)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    // Check if identity is root.
    if (id == ROOT) {
        error("cannot drop root identity");
        return (-EINVAL);
    }

    // Free identity.
    return (identity_free(id));
}

/**
 * @details Checks if an identity matches the one of a superuser.
 */
bool identity_is_superuser(const struct identity *id)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    // Check if either the user ID or the
    // effective user ID is matches the user ID of root.
    return ((id->uid == ROOT->uid) || (id->euid == ROOT->uid));
}

/**
 * @details Gets the real user ID stored in an identity.
 */
uid_t identity_getuid(const struct identity *id)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    return (id->uid);
}

/**
 * @details Gets the effective user ID stored in an identity.
 */
uid_t identity_geteuid(const struct identity *id)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    return (id->euid);
}

/**
 * @details Gets the user group ID stored in an identity.
 */
gid_t identity_getgid(const struct identity *id)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    return (id->gid);
}

/**
 * @details Gets the effective user group ID stored in an identity.
 */
gid_t identity_getegid(const struct identity *id)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    return (id->egid);
}

/**
 * @details Sets the real user ID stored in an identity.
 */
int identity_setuid(struct identity *id, uid_t uid)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    // Check if superuser is changing user ID.
    if (identity_is_superuser(id)) {
        // Change real user ID, effective user ID, and saved user ID.
        id->uid = uid;
        id->euid = uid;
        id->suid = uid;
        return (0);
    }

    // Check if user is changing its own user ID.
    if ((id->uid == uid) || (id->suid == uid)) {
        // Change effective user ID only.
        id->euid = uid;
        return (0);
    }

    error("permission denied");
    return (-EPERM);
}

/**
 * @details Sets the effective user ID stored in an identity.
 */
int identity_seteuid(struct identity *id, uid_t euid)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    // Check if superuser is changing effective user ID.
    if (identity_is_superuser(id)) {
        // Change effective user ID only.
        id->euid = euid;
        return (0);
    }

    // Check if user is changing its own effective user ID.
    if (id->euid == euid) {
        // Change effective user ID only.
        id->euid = euid;
        return (0);
    }

    error("permission denied");
    return (-EPERM);
}

/**
 * @details Sets the user group ID stored in an identity.
 */
int identity_setgid(struct identity *id, gid_t gid)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    // Check if superuser is changing group ID.
    if (identity_is_superuser(id)) {
        // Change real group ID, effective group ID, and saved group ID.
        id->gid = gid;
        id->egid = gid;
        id->sgid = gid;
        return (0);
    }

    // Check if user is changing its own group ID.
    if ((id->gid == gid) || (id->sgid == gid)) {
        // Change effective group ID only.
        id->egid = gid;
        return (0);
    }

    error("permission denied");
    return (-EPERM);
}

/**
 * @details Sets the effective user group ID stored in an identity.
 */
int identity_setegid(struct identity *id, gid_t egid)
{
    // Check if identity is valid.
    if (!identity_is_valid(id)) {
        return (-EINVAL);
    }

    // Check if superuser is changing effective group ID.
    if (identity_is_superuser(id)) {
        // Change effective group ID only.
        id->egid = egid;
        return (0);
    }

    // Check if user is changing its own effective group ID.
    if (id->egid == egid) {
        // Change effective group ID only.
        id->egid = egid;
        return (0);
    }

    error("permission denied");
    return (-EPERM);
}

/**
 * @details Initializes the identity management facility.
 */
int iam_init(void)
{
    static bool initialized = false;

    if (initialized) {
        warn("identity facility already initialized");
        return (-EBUSY);
    }

    // Initialize table of identities.
    for (int i = 0; i < PROCESS_MAX; i++) {
        table.used[i] = false;
        __memset(&table.identities[i], 0, sizeof(struct identity));
    }

    // Allocate root identity.
    struct identity *root = identity_alloc();
    if (root == NULL) {
        // We could not allocate the root identity. This is unlikely to happen.
        // If it does happen, something bad is going on. Unfortunately, we
        // cannot do much about it. We just log an error and return.
        error("failed to allocate root identity");
        return (-ENOMEM);
    }

    // Initialize root identity.
    root->uid = 0;
    root->gid = 0;
    root->euid = 0;
    root->egid = 0;
    root->suid = 0;
    root->sgid = 0;
    ROOT = root;

    // Run unit tests.
    iam_test();

    return (0);
}

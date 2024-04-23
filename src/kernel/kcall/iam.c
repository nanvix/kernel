/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/iam.h>
#include <nanvix/kernel/pm.h>
#include <nanvix/types.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Gets the real user ID of the calling process.
 */
uid_t kcall_getuid(void)
{
    const struct process *proc = process_get_curr();
    const struct identity *id = proc->id;
    return (identity_getuid(id));
}

/**
 * @details Gets the effective user ID of the calling process.
 */
uid_t kcall_geteuid(void)
{
    const struct process *proc = process_get_curr();
    const struct identity *id = proc->id;
    return (identity_geteuid(id));
}

/**
 * @details Gets the user group ID of the calling process.
 */
gid_t kcall_getgid(void)
{
    const struct process *proc = process_get_curr();
    const struct identity *id = proc->id;
    return (identity_getgid(id));
}

/**
 * @details Gets the effective user group ID of the calling process.
 */
gid_t kcall_getegid(void)
{
    const struct process *proc = process_get_curr();
    const struct identity *id = proc->id;
    return (identity_getegid(id));
}

/**
 * @details Sets the real user ID of the calling process.
 */
int kcall_setuid(uid_t uid)
{
    const struct process *proc = process_get_curr();
    struct identity *id = proc->id;
    return (identity_setuid(id, uid));
}

/**
 * @details Sets the effective user ID of the calling process.
 */
int kcall_seteuid(uid_t euid)
{
    const struct process *proc = process_get_curr();
    struct identity *id = proc->id;
    return (identity_seteuid(id, euid));
}

/**
 * @details Sets the user group ID of the calling process.
 */
int kcall_setgid(gid_t gid)
{
    const struct process *proc = process_get_curr();
    struct identity *id = proc->id;
    return (identity_setgid(id, gid));
}

/**
 * @details Sets the effective user group ID of the calling process.
 */
int kcall_setegid(gid_t egid)
{
    const struct process *proc = process_get_curr();
    struct identity *id = proc->id;
    return (identity_setegid(id, egid));
}

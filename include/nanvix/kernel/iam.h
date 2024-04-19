/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_IAM_H_
#define NANVIX_KERNEL_IAM_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/types.h>
#include <stdbool.h>

/*============================================================================*
 * Types                                                                      *
 *============================================================================*/

/**
 * @brief Identity.
 */
struct identity;

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Gets the root identity.
 *
 * @return The root identity is returned.
 */
extern const struct identity *identity_root(void);

/**
 * @brief Allocates a new identity and initializes it.
 *
 * @param baseid Base identity used for initialization.
 *
 * @return On successful completion, a pointer to the newly allocated identity
 * is returned. On error, @p NULL is returned instead.
 */
extern struct identity *identity_new(const struct identity *baseid);

/**
 * @brief Releases a previously allocated identity.
 *
 * @param id Target identity.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int identity_drop(struct identity *id);

/**
 * @brief Checks if an identity matches the one of a superuser.
 *
 * @param id Target identity.
 *
 * @return The function returns @p true if the target identity matches the one
 * of a superuser, and @p false otherwise.
 */
extern bool identity_is_superuser(const struct identity *id);

/**
 * @brief Gets the real user ID stored in an identity.
 *
 * @param id Target identity.
 *
 * @return The real user ID stored in the identity pointed to by @p id is
 * returned.
 */
extern uid_t identity_getuid(const struct identity *id);

/**
 * @brief Gets the effective user ID stored in an identity.
 *
 * @param id Target identity.
 *
 * @return The effective user ID stored in the identity pointed to by @p id is
 * returned.
 */
extern uid_t identity_geteuid(const struct identity *id);

/**
 * @brief Gets the user group ID stored in an identity.
 *
 * @param id Target identity.
 *
 * @return The user group ID stored in the identity pointed to by @p id is
 * returned.
 */
extern gid_t identity_getgid(const struct identity *id);

/**
 * @brief Gets the effective user group ID stored in an identity.
 *
 * @param id Target identity.
 *
 * @return The effective user group ID stored in the identity pointed to by @p
 * id is returned.
 */
extern gid_t identity_getegid(const struct identity *id);

/**
 * @brief Sets the real user ID stored in an identity.
 *
 * @param id Target identity.
 * @param uid User ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int identity_setuid(struct identity *id, uid_t uid);

/**
 * @brief Sets the effective user ID stored in an identity.
 *
 * @param id Target identity.
 * @param euid Effective user ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int identity_seteuid(struct identity *id, uid_t euid);

/**
 * @brief Sets the user group ID stored in an identity.
 *
 * @param id Target identity.
 * @param gid user group ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 */
extern int identity_setgid(struct identity *id, gid_t gid);

/**
 * @brief Sets the effective user group ID stored in an identity.
 *
 * @param id Target identity.
 * @param egid Effective user group ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int identity_setegid(struct identity *id, gid_t egid);

/**
 * @brief Initializes the identity management facility.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int iam_init(void);

/*============================================================================*/

#endif /* NANVIX_KERNEL_IAM_H_ */

/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/iam.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/limits.h>
#include <nanvix/kernel/log.h>
#include <stddef.h>

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Unit test.
 */
struct test {
    void (*test_fn)(void); /** Test function. */
    const char *name;      /** Test name.     */
};

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Attempt to retrieve the root identity.
 */
static void test_identity_root(void)
{
    const struct identity *root = NULL;

    root = identity_root();

    KASSERT(root != NULL);
}

/**
 * @brief Attempt to verify that the root identity is a superuser.
 */
static void test_identity_is_superuser(void)
{
    const struct identity *root = identity_root();

    KASSERT(identity_is_superuser(root) == true);
}

/**
 * @brief Attempt to drop root identity.
 */
static void test_identity_drop_root(void)
{
    struct identity *root = (struct identity *)identity_root();

    KASSERT(identity_drop(root) == -EINVAL);
}

/**
 * @brief Attempt to allocate and drop an identity.
 */
static void test_identity_alloc_drop(void)
{
    struct identity *id = NULL;
    const struct identity *root = identity_root();

    id = identity_new(root);
    KASSERT(id != NULL);

    KASSERT(identity_drop(id) == 0);
}

/**
 * @brief Attempt to allocate an identity with an invalid base identity.
 */
static void test_identity_alloc_invalid_base(void)
{
    struct identity *id = NULL;

    id = identity_new(NULL);
    KASSERT(id == NULL);
}

/**
 * @brief Attempt to allocate all possible identities.
 */
static void test_identity_alloc_all(void)
{
    struct identity *ids[PROCESS_MAX];
    const struct identity *root = identity_root();

    // Allocate all possible identities.
    for (int i = 0; i < PROCESS_MAX - 1; i++) {
        ids[i] = identity_new(root);
        KASSERT(ids[i] != NULL);
    }

    // Fail to allocate one more identity.
    ids[PROCESS_MAX - 1] = identity_new(root);
    KASSERT(ids[PROCESS_MAX - 1] == NULL);

    // Drop all allocated identities.
    for (int i = 0; i < PROCESS_MAX - 1; i++) {
        KASSERT(identity_drop(ids[i]) == 0);
    }

    // Succeed to allocate one more identity and drop it.
    ids[PROCESS_MAX - 1] = identity_new(root);
    KASSERT(ids[PROCESS_MAX - 1] != NULL);
    KASSERT(identity_drop(ids[PROCESS_MAX - 1]) == 0);
}

/**
 * @brief Attempt to get the real user ID stored in an identity.
 */
static void test_identity_getuid(void)
{
    const struct identity *root = identity_root();

    KASSERT(identity_getuid(root) == 0);
}

/**
 * @brief Attempt to get the effective user ID stored in an identity.
 */
static void test_identity_geteuid(void)
{
    const struct identity *root = identity_root();

    KASSERT(identity_geteuid(root) == 0);
}

/**
 * @brief Attempt to get the real group ID stored in an identity.
 */
static void test_identity_getgid(void)
{
    const struct identity *root = identity_root();

    KASSERT(identity_getgid(root) == 0);
}

/**
 * @brief Attempt to get the effective group ID stored in an identity.
 */
static void test_identity_getegid(void)
{
    const struct identity *root = identity_root();

    KASSERT(identity_getegid(root) == 0);
}

/**
 * @brief Attempt to set the real user ID stored in an identity.
 */
static void test_identity_setuid(void)
{
    struct identity *id = NULL;
    const struct identity *root = identity_root();

    id = identity_new(root);
    KASSERT(id != NULL);

    KASSERT(identity_setuid(id, 1) == 0);
    KASSERT(identity_getuid(id) == 1);
    KASSERT(identity_setuid(id, 2) == -EPERM);

    KASSERT(identity_drop(id) == 0);
}

/**
 * @brief Attempt to set the effective user ID stored in an identity.
 */
static void test_identity_seteuid(void)
{
    struct identity *id = NULL;
    const struct identity *root = identity_root();

    id = identity_new(root);
    KASSERT(id != NULL);

    KASSERT(identity_seteuid(id, 1) == 0);
    KASSERT(identity_geteuid(id) == 1);
    KASSERT(identity_seteuid(id, 2) == 0);

    KASSERT(identity_drop(id) == 0);
}

/**
 * @brief Attempt to set the real group ID stored in an identity.
 */
static void test_identity_setgid(void)
{
    struct identity *id = NULL;
    const struct identity *root = identity_root();

    id = identity_new(root);
    KASSERT(id != NULL);

    KASSERT(identity_setgid(id, 1) == 0);
    KASSERT(identity_getgid(id) == 1);
    KASSERT(identity_setgid(id, 2) == 0);

    KASSERT(identity_drop(id) == 0);
}

/**
 * @brief Attempt to set the effective group ID stored in an identity.
 */
static void test_identity_setegid(void)
{
    struct identity *id = NULL;
    const struct identity *root = identity_root();

    id = identity_new(root);
    KASSERT(id != NULL);

    KASSERT(identity_setegid(id, 1) == 0);
    KASSERT(identity_getegid(id) == 1);
    KASSERT(identity_setegid(id, 2) == 0);

    KASSERT(identity_drop(id) == 0);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Runs unit tests on the identity management facility.
 */
void iam_test(void)
{
    struct test tests[] = {
        {test_identity_root, "retrieve the root identity"},
        {test_identity_is_superuser,
         "verify that the root identity is a superuser"},
        {test_identity_drop_root, "drop the root identity"},
        {test_identity_alloc_drop, "allocate and drop an identity"},
        {test_identity_alloc_invalid_base,
         "allocate an identity with an invalid base identity"},
        {test_identity_alloc_all, "allocate all possible identities"},
        {test_identity_getuid, "get the real user ID stored in an identity"},
        {test_identity_geteuid,
         "get the effective user ID stored in an identity"},
        {test_identity_getgid,
         "get the real user group ID stored in an identity"},
        {test_identity_getegid,
         "get the effective user group ID stored in an identity"},
        {test_identity_setuid, "set the real user ID stored in an identity"},
        {test_identity_seteuid,
         "set the effective user ID stored in an identity"},
        {test_identity_setgid,
         "set the real user group ID stored in an identity"},
        {test_identity_setegid,
         "set the effective user group ID stored in an identity"},
        {NULL, NULL},
    };

    for (int i = 0; tests[i].test_fn != NULL; i++) {
        debug("%s", tests[i].name);
        tests[i].test_fn();
    }
}

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <stddef.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Defined Number of Pages for Stress Tests
 */
#define NUM_UPAGES_TEST 64

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
 * @brief API Test: User Page Allocation
 *
 * @author Pedro Henrique Penna
 */
static void test_api_upage_allocation(void)
{
    KASSERT(upage_alloc(root_pgdir, USER_BASE_VIRT) == 0);
    KASSERT(upage_free(root_pgdir, USER_BASE_VIRT) == 0);
}

/**
 * @brief API Test: User Page Write
 *
 * @author Pedro Henrique Penna
 */
static void test_api_upage_write(void)
{
    unsigned *upg;
    const unsigned magic = 0xdeadbeef;

    upg = (unsigned *)(USER_BASE_VIRT);

    /* Allocate.*/
    KASSERT(upage_alloc(root_pgdir, VADDR(upg)) == 0);

    /* Write. */
    for (size_t i = 0; i < PAGE_SIZE / sizeof(unsigned); i++)
        upg[i] = magic;
    for (size_t i = 0; i < PAGE_SIZE / sizeof(unsigned); i++)
        KASSERT(upg[i] == magic);

    /* Unmap. */
    KASSERT(upage_free(root_pgdir, VADDR(upg)) == 0);
}

/**
 * @brief Fault Injection Test: Invalid User Page Allocation
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_upage_invalid_allocation(void)
{
    KASSERT(upage_alloc(NULL, USER_BASE_VIRT) == -1);
    KASSERT(upage_alloc(root_pgdir, KERNEL_BASE_VIRT) == -1);
    KASSERT(upage_alloc(root_pgdir, USER_BASE_VIRT - PAGE_SIZE) == -1);
    KASSERT(upage_alloc(root_pgdir, USER_BASE_VIRT + PAGE_SIZE - 1) == -1);
}

/**
 * @brief Fault Injection Test: Double User Allocation Test
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_upage_double_allocation(void)
{
    KASSERT(upage_alloc(root_pgdir, USER_BASE_VIRT) == 0);
    KASSERT(upage_alloc(root_pgdir, USER_BASE_VIRT) == -1);
    KASSERT(upage_free(root_pgdir, USER_BASE_VIRT) == 0);
}

/**
 * @brief Fault Injection Test: Invalid User Page Release
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_upage_invalid_free(void)
{
    KASSERT(upage_free(NULL, USER_BASE_VIRT) == -1);
    KASSERT(upage_free(root_pgdir, KERNEL_BASE_VIRT) == -1);
    KASSERT(upage_free(root_pgdir, USER_BASE_VIRT - PAGE_SIZE) == -1);
    KASSERT(upage_free(root_pgdir, USER_BASE_VIRT + PAGE_SIZE - 1) == -1);
}

/**
 * @brief Fault Injection Test: Bad User Page Release
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_upage_bad_free(void)
{
    KASSERT(upage_free(root_pgdir, USER_BASE_VIRT) == -1);
}

/**
 * @brief Fault Injection Test: Double User Page Release
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_upage_double_free(void)
{
    KASSERT(upage_alloc(root_pgdir, USER_BASE_VIRT) == 0);
    KASSERT(upage_free(root_pgdir, USER_BASE_VIRT) == 0);
    KASSERT(upage_free(root_pgdir, USER_BASE_VIRT) == -1);
}

/**
 * @brief Stress Test: User Page Allocation
 *
 * @author Pedro Henrique Penna
 */
static void test_stress_upage_allocation(void)
{
    /* Allocate pages. */
    for (unsigned i = 0; i < NUM_UPAGES_TEST; i++)
        KASSERT(upage_alloc(root_pgdir, USER_BASE_VIRT + i * PAGE_SIZE) == 0);

    /* Release pages. */
    for (unsigned i = 0; i < NUM_UPAGES_TEST; i++)
        KASSERT(upage_free(root_pgdir, USER_BASE_VIRT + i * PAGE_SIZE) == 0);
}

/**
 * @brief Stress Test: User Page Write
 *
 * @author Pedro Henrique Penna
 */
static void test_stress_upage_write(void)
{
    unsigned *upg;
    const unsigned magic = 0xdeadbeef;

    /* Allocate pages. */
    for (unsigned i = 0; i < NUM_UPAGES_TEST; i++) {
        upg = (void *)(USER_BASE_VIRT + i * PAGE_SIZE);

        KASSERT(upage_alloc(root_pgdir, VADDR(upg)) == 0);

        /* Write to kernel page. */
        for (unsigned j = 0; j < PAGE_SIZE / sizeof(unsigned); j++)
            upg[j] = magic;
    }

    /* Release pages. */
    for (unsigned i = 0; i < NUM_UPAGES_TEST; i++) {
        upg = (void *)(USER_BASE_VIRT + i * PAGE_SIZE);

        /* Sanity check. */
        for (unsigned j = 0; j < PAGE_SIZE / sizeof(unsigned); j++)
            KASSERT(upg[j] == magic);

        KASSERT(upage_free(root_pgdir, VADDR(upg)) == 0);
    }
}

/**
 * @brief User Page Pool unit tests.
 */
static struct test upool_tests[] = {
    {test_api_upage_allocation, "user page allocation"},
    {test_api_upage_write, "user page write              "},
    {test_fault_upage_invalid_allocation, "user page invalid allocation"},
    {test_fault_upage_double_allocation, "user page double allocation"},
    {test_fault_upage_invalid_free, "user page invalid free"},
    {test_fault_upage_bad_free, "user page bad free"},
    {test_fault_upage_double_free, "user page double free"},
    {test_stress_upage_allocation, "user page allocation"},
    {test_stress_upage_write, "user page write"},
    {NULL, NULL},
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function runs self-tests on the User Page Pool.
 */
void test_upool(void)
{
    for (int i = 0; upool_tests[i].test_fn != NULL; i++) {
        kprintf(MODULE_NAME " TEST: %s", upool_tests[i].name);
        upool_tests[i].test_fn();
    }
}

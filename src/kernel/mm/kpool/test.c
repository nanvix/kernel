/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
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
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief API Test: Kernel Page Address Translation
 *
 * @author Pedro Henrique Penna
 */
static void test_api_kpage_translation(void)
{
    KASSERT(kpool_id_to_addr(0) == KPOOL_BASE_VIRT);
    KASSERT(kpool_addr_to_id(KPOOL_BASE_VIRT) == 0);
    KASSERT(kpool_addr_to_frame(KPOOL_BASE_VIRT) ==
            (KPOOL_BASE_PHYS >> PAGE_SHIFT));
    KASSERT(kpool_frame_to_addr(KPOOL_BASE_PHYS >> PAGE_SHIFT) ==
            KPOOL_BASE_VIRT);
}

/**
 * @brief API Test: Kernel Page Allocation
 *
 * @author Pedro Henrique Penna
 */
static void test_api_kpage_allocation(void)
{
    void *kpg;

    KASSERT((kpg = kpage_get(0)) != NULL);

    KASSERT(VADDR(kpg) >= KPOOL_BASE_VIRT);
    KASSERT(VADDR(kpg) < (KPOOL_BASE_VIRT + KPOOL_SIZE));

    KASSERT(kpage_put(kpg) == 0);
}

/**
 * @brief API Test: Kernel Page Write
 *
 * @author Pedro Henrique Penna
 */
static void test_api_kpage_write(void)
{
    unsigned *kpg;
    const unsigned magic = 0xdeadbeef;

    /* Allocate. */
    KASSERT((kpg = kpage_get(0)) != NULL);

    /* Write. */
    for (size_t i = 0; i < PAGE_SIZE / sizeof(unsigned); i++)
        kpg[i] = magic;
    for (size_t i = 0; i < PAGE_SIZE / sizeof(unsigned); i++)
        KASSERT(kpg[i] == magic);

    /* Release. */
    KASSERT(kpage_put(kpg) == 0);
}

/**
 * @brief API Test: Kernel Page Clean Allocation
 *
 * @author Pedro Henrique Penna
 */
static void test_api_kpage_clean_allocation(void)
{
    unsigned *kpg;
    const unsigned magic = 0xdeadbeef;

    /* Make a dirty kernel. */
    KASSERT((kpg = kpage_get(0)) != NULL);
    for (unsigned i = 0; i < PAGE_SIZE / sizeof(unsigned); i++)
        kpg[i] = magic;
    KASSERT(kpage_put(kpg) == 0);

    /* Allocate. */
    KASSERT((kpg = kpage_get(1)) != NULL);

    /* Sanity check. */
    for (unsigned i = 0; i < PAGE_SIZE / sizeof(unsigned); i++)
        KASSERT(kpg[i] == 0);

    /* Release. */
    KASSERT(kpage_put(kpg) == 0);
}

/**
 * @brief Fault Injection Test: Invalid Kernel Page Release
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_kpage_invalid_free(void)
{
    KASSERT(kpage_put((void *)(KPOOL_BASE_VIRT - PAGE_SIZE)) == -1);
    KASSERT(kpage_put((void *)(KPOOL_BASE_VIRT + KPOOL_SIZE)) == -1);
}

/**
 * @brief Fault Injection Test: Bad Kernel Page Release
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_kpage_bad_free(void)
{
    KASSERT(kpage_put((void *)KPOOL_BASE_VIRT) == -1);
}

/**
 * @brief Fault Injection Test: Kernel Page Double Free
 *
 * @author Pedro Henrique Penna
 */
static void test_fault_kpage_double_free(void)
{
    void *kpg;

    KASSERT((kpg = kpage_get(0)) != NULL);
    KASSERT(kpage_put(kpg) == 0);
    KASSERT(kpage_put(kpg) == -1);
}

/**
 * @brief Stress test: Kernel Page Address Translation
 *
 * @author Pedro Henrique Penna
 */
static void test_stress_kpage_translation(void)
{
    /* Allocate all kernel pages. */
    for (unsigned i = 0; i < NUM_KPAGES; i++) {
        vaddr_t vaddr = kpool_id_to_addr(i);

        KASSERT(vaddr >= KPOOL_BASE_VIRT);
        KASSERT(vaddr < (KPOOL_BASE_VIRT + KPOOL_SIZE));
    }
}

/**
 * @brief Stress Test: Kernel Page Allocation Overflow
 *
 * @author Pedro Henrique Penna
 */
static void test_stress_kpage_allocation_overflow(void)
{
    unsigned *kpg;

    /* Allocate all kernel pages. */
    for (unsigned i = 0; i < NUM_KPAGES; i++)
        KASSERT((kpg = kpage_get(0)) != NULL);

    /* Fail to allocate one more page. */
    KASSERT((kpg = kpage_get(0)) == NULL);

    /* Free all kernel pages. */
    for (unsigned i = 0; i < NUM_KPAGES; i++)
        KASSERT(kpage_put((void *)(KPOOL_BASE_VIRT + i * PAGE_SIZE)) == 0);
}

/**
 * @brief Stress test: Kernel Page Allocation
 */
static void test_stress_kpage_allocation(void)
{
    unsigned *kpg;

    /* Allocate all kernel pages. */
    for (unsigned i = 0; i < NUM_KPAGES; i++)
        KASSERT((kpg = kpage_get(0)) != NULL);

    /* Free all kernel pages. */
    for (unsigned i = 0; i < NUM_KPAGES; i++)
        KASSERT(kpage_put((void *)(KPOOL_BASE_VIRT + i * PAGE_SIZE)) == 0);
}

/**
 * @brief Stress Test: Kernel Page Write
 *
 * @author Pedro Henrique Penna
 */
static void test_stress_kpage_write(void)
{
    unsigned *kpg;
    const unsigned magic = 0xdeadbeef;

    /* Allocate all kernel pages. */
    for (unsigned i = 0; i < NUM_KPAGES; i++) {
        KASSERT((kpg = kpage_get(1)) != NULL);

        /* Write to kernel page. */
        for (size_t j = 0; j < PAGE_SIZE / sizeof(unsigned); j++)
            kpg[j] = magic;
    }

    /* Free all kernel pages. */
    for (unsigned i = 0; i < NUM_KPAGES; i++) {
        kpg = (void *)(KPOOL_BASE_VIRT + i * PAGE_SIZE);

        /* Sanity check. */
        for (size_t j = 0; j < PAGE_SIZE / sizeof(unsigned); j++)
            KASSERT(kpg[j] == magic);

        KASSERT(kpage_put(kpg) == 0);
    }
}

/**
 * @brief Kernel Pool unit tests.
 *
 * Unit tests for the Kernel Page Pool subsystem.
 *
 * @author Pedro Henrique Penna.
 */
static struct {
    void (*test_fn)(void); /**< Test function.     */
    const char *type;      /**< Name of test type. */
    const char *name;      /**< Test Name.         */
} kpool_tests[] = {
    {test_api_kpage_translation, "api", "kernel page address translation"},
    {test_api_kpage_allocation, "api", "kernel page allocation         "},
    {test_api_kpage_write, "api", "kernel page write              "},
    {test_api_kpage_clean_allocation, "api", "kernel page clean allocation   "},
    {test_fault_kpage_invalid_free, "fault", "kernel page invalid release    "},
    {test_fault_kpage_bad_free, "fault", "kernel page bad release        "},
    {test_fault_kpage_double_free, "fault", "kernel page double release     "},
    {test_stress_kpage_translation,
     "stress",
     "kernel page address translation"},
    {test_stress_kpage_allocation, "stress", "kernel page allocation         "},
    {test_stress_kpage_allocation_overflow,
     "stress",
     "kernel page allocation overflow"},
    {test_stress_kpage_write, "stress", "kernel page write              "},
    {NULL, NULL, NULL},
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function runs self-tests on the Kernel Page Pool.
 */
void test_kpool(void)
{
    for (int i = 0; kpool_tests[i].test_fn != NULL; i++) {
        kprintf(MODULE_NAME " TEST: %s", kpool_tests[i].name);
        kpool_tests[i].test_fn();
    }
}

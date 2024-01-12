/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/libcore.h>
#include <stddef.h>

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Attempts to clear a PTE.
 */
static void mmu_pte_clear(void)
{
    struct pte pte;

    __memset(&pte, 1, sizeof(struct pte));
    pte_clear(&pte);
    KASSERT((pte_frame_get(&pte) == 0) && (pte_is_present(&pte) == 0));
}

/**
 * @brief Attempts to clear a PDE.
 */
static void mmu_pde_clear(void)
{
    struct pde pde;

    __memset(&pde, 1, sizeof(struct pde));
    pde_clear(&pde);
    KASSERT((pde_frame_get(&pde) == 0) && (pde_is_present(&pde) == 0));
}

/**
 * @brief Attempts to set a Frame of a PTE.
 */
static void mmu_pte_frame_set(void)
{
    struct pte pte;
    frame_t frame = (0x1UL << (VADDR_BIT - PAGE_SHIFT)) - 1;

    __memset(&pte, 1, sizeof(struct pte));
    pte_clear(&pte);
    pte_frame_set(&pte, frame);

    KASSERT(pte_frame_get(&pte) == frame);
}

/**
 * @brief Attempts to set a frame of a PDE.
 */
static void mmu_pde_frame_set(void)
{
    struct pde pde;
    frame_t frame = (0X1UL << (VADDR_BIT - PAGE_SHIFT)) - 1;

    __memset(&pde, 1, sizeof(struct pde));
    pde_clear(&pde);
    pde_frame_set(&pde, frame);

    KASSERT(pde_frame_get(&pde) == frame);
}

/**
 * @brief Attempts to set present bit of a PTE.
 */
static void mmu_pte_present_set(void)
{
    struct pte pte;

    __memset(&pte, 1, sizeof(struct pte));
    pte_clear(&pte);
    pte_present_set(&pte, 1);
    KASSERT(pte_is_present(&pte));
    pte_present_set(&pte, 0);
    KASSERT(!pte_is_present(&pte));
}

/**
 * @brief Attempts to set present bit of a PDE.
 */
static void mmu_pde_present_set(void)
{
    struct pde pde;

    __memset(&pde, 1, sizeof(struct pde));
    pde_clear(&pde);
    pde_present_set(&pde, 1);
    KASSERT(pde_is_present(&pde));
    pde_present_set(&pde, 0);
    KASSERT(!pde_is_present(&pde));
}

/**
 * @brief Attempts to set user bit of a PTE.
 */
static void mmu_pte_user_set(void)
{
    struct pte pte;

    __memset(&pte, 1, sizeof(struct pte));
    pte_clear(&pte);
    pte_user_set(&pte, 1);
    KASSERT(pte_is_user(&pte));
    pte_user_set(&pte, 0);
    KASSERT(!pte_is_user(&pte));
}

/**
 * @brief Attempts to set user bit of a PDE
 */
static void mmu_pde_user_set(void)
{
    struct pde pde;

    __memset(&pde, 1, sizeof(struct pde));
    pde_clear(&pde);
    pde_user_set(&pde, 1);
    KASSERT(pde_is_user(&pde));
    pde_user_set(&pde, 0);
    KASSERT(!pde_is_user(&pde));
}

/**
 * @brief Attempts to set write bit of a PTE
 */
static void mmu_pte_write_set(void)
{
    struct pte pte;

    __memset(&pte, 1, sizeof(struct pte));
    pte_clear(&pte);
    pte_write_set(&pte, 1);
    KASSERT(pte_is_write(&pte));
    pte_write_set(&pte, 0);
    KASSERT(!pte_is_write(&pte));
}

/**
 * @brief Attempts to set write bit of a PDE.
 */
static void mmu_pde_write_set(void)
{
    struct pde pde;

    __memset(&pde, 1, sizeof(struct pde));
    pde_clear(&pde);
    pde_write_set(&pde, 1);
    KASSERT(pde_is_write(&pde));
    pde_write_set(&pde, 0);
    KASSERT(!pde_is_write(&pde));
}

/**
 * @brief Attempts to clear invalid PTE.
 */
static void mmu_pte_clear_inval(void)
{
    KASSERT(pte_clear(NULL) == -1);
}

/**
 * @brief Attempts to clear invalid PDE.
 */
static void mmu_pde_clear_inval(void)
{
    KASSERT(pde_clear(NULL) == -1);
}

/**
 * @brief Attempts to set a frame in an invalid PTE.
 */
static void mmu_pte_frame_set_inval(void)
{
    frame_t frame = (0X1UL << (VADDR_BIT - PAGE_SHIFT)) - 1;

    KASSERT(pte_frame_set(NULL, frame) == -1);
}

/**
 * @brief Attempts to set a frame in an invalid PDE.
 */
static void mmu_pde_frame_set_inval(void)
{
    frame_t frame = (0X1UL << (VADDR_BIT - PAGE_SHIFT)) - 1;

    KASSERT(pde_frame_set(NULL, frame) == -1);
}

/**
 * @brief Attempts to set invalid frame in a PTE.
 */
static void mmu_pte_frame_set_inval2(void)
{
    struct pte pte;

    KASSERT(pte_frame_set(&pte, -1) == -1);
}

/**
 * @brief Attempts to set invalid frame in a PDE.
 */
static void mmu_pde_frame_set_inval2(void)
{
    struct pde pde;

    KASSERT(pde_frame_set(&pde, -1) == -1);
}

/**
 * @brief Attempts to set present bit in an invalid PTE.
 */
static void mmu_pte_present_set_inval(void)
{
    KASSERT(pte_present_set(NULL, 0) == -1);
}

/**
 * @brief Attempts to set present bit in an invalid PDE.
 */
static void mmu_pde_present_set_inval(void)
{
    KASSERT(pde_present_set(NULL, 0) == -1);
}

/**
 * @brief Attempts to set user bit in an invalid PTE.
 */
static void mmu_pte_user_set_inval(void)
{
    KASSERT(pte_user_set(NULL, 0) == -1);
}

/**
 * @brief Attempts to set user bit in an invalid PDE.
 */
static void mmu_pde_user_set_inval(void)
{
    KASSERT(pde_user_set(NULL, 0) == -1);
}

/**
 * @brief Attempts to set write bit in an invalid PTE.
 */
static void mmu_pte_write_set_inval(void)
{
    KASSERT(pte_write_set(NULL, 0) == -1);
}

/**
 * @brief Attempts to set write bit in an invalid PDE.
 */
static void mmu_pde_write_set_inval(void)
{
    KASSERT(pde_write_set(NULL, 0) == -1);
}

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Unit test.
 */
struct test {
    void (*test_fn)(void); /**< Test function. */
    const char *name;      /**< Test name.     */
};

/**
 * @brief Unit tests.
 */
static struct test mmu_tests[] = {
    {mmu_pte_clear, "clear pte"},
    {mmu_pde_clear, "clear pde"},
    {mmu_pte_frame_set, "pte frame set"},
    {mmu_pde_frame_set, "pde frame set"},
    {mmu_pte_present_set, "pte present set"},
    {mmu_pde_present_set, "pde present set"},
    {mmu_pte_user_set, "pte user set"},
    {mmu_pde_user_set, "pde user set"},
    {mmu_pte_write_set, "pte write set"},
    {mmu_pde_write_set, "pde write set"},
    {mmu_pte_clear_inval, "clear invalid pte"},
    {mmu_pde_clear_inval, "clear invalid pde"},
    {mmu_pte_frame_set_inval, "set frame in invalid pte"},
    {mmu_pde_frame_set_inval, "set frame in invalid pde"},
    {mmu_pte_frame_set_inval2, "set invalid frame in pte"},
    {mmu_pde_frame_set_inval2, "set invalid frame in pde"},
    {mmu_pte_present_set_inval, "set present bit in invalid pte"},
    {mmu_pde_present_set_inval, "set present bit in invalid pde"},
    {mmu_pte_user_set_inval, "set user bit in invalid pte"},
    {mmu_pde_user_set_inval, "set user bit in invalid pde"},
    {mmu_pte_write_set_inval, "set write bit in invalid pte"},
    {mmu_pde_write_set_inval, "set write bit in invalid pde"},
    {NULL, NULL},
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function function launches testing units on the MMU Interface
 * of the HAL.
 */
void test_mmu(void)
{
    for (int i = 0; mmu_tests[i].test_fn != NULL; i++) {
        kprintf(MODULE_NAME " TEST: %s", mmu_tests[i].name);
        mmu_tests[i].test_fn();
    }
}

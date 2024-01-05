/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/cc.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/libcore.h>

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Task state segment (TSS).
 */
struct tss tss;

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Kernel stack.
 */
extern byte_t kstack[PAGE_SIZE];

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Loads the Task State Segment (TSS).
 */
void tss_load(unsigned tss_selector)
{
    kprintf("[hal][cpu] loading tss...");

    asm volatile("movl %0, %%eax;\
			ltr %%ax;"
                 :
                 : "r"(tss_selector)
                 : "eax");
}

/**
 * @details Initializes the Task State Segment (TSS).
 */
const struct tss *tss_init(unsigned ss0)
{
    kprintf("[hal][cpu] initializing tss...");

    // Ensure that size of structures match what we expect.
    KASSERT_SIZE(sizeof(struct tss), TSS_SIZE);

    // Blank TSS.
    __memset(&tss, 0, TSS_SIZE);

    // Initialize the TSS.
    tss.ss0 = ss0;
    tss.esp0 = (word_t)kstack + PAGE_SIZE;

    return (&tss);
}

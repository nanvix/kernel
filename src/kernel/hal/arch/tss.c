/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
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
static struct tss tss;

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
const struct tss *tss_init(unsigned ss_selector)
{
    kprintf("[hal][cpu] initializing tss...");

    // Ensure that size of structures match what we expect.
    KASSERT_SIZE(sizeof(struct tss), TSS_SIZE);

    // Blank TSS.
    __memset(&tss, 0, TSS_SIZE);

    // Initialize the TSS.
    // FIXME: Provide definitions for the following magic constants.
    tss.ss0 = ss_selector;

    return (&tss);
}

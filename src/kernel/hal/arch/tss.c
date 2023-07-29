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
 * Variables                                                                  *
 *============================================================================*/

/**
 * @brief Task state segment.
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
 * @todo Provide a detailed description for this function (TODO).
 */
const struct tss *tss_init(unsigned ss_selector)
{
    /* Size-error checking. */
    KASSERT_SIZE(sizeof(struct tss), TSS_SIZE);

    kprintf("[hal][cpu] initializing tss...");

    /* Blank TSS. */
    __memset(&tss, 0, TSS_SIZE);

    /* Fill up TSS. */
    tss.ss0 = ss_selector;
    tss.iomap = (TSS_SIZE - 1) << 16;

    return (&tss);
}

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

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Initializes the memory management system.
 */
const struct vmem *mm_init(void)
{
    kprintf(MODULE_NAME " INFO: initializing the memory system");
    frame_init();
    const void *root_pgdir = memory_init();
    kpool_init();
    const struct vmem *root_vmem = vmem_init(root_pgdir);
    upool_init();

    return (root_vmem);
}

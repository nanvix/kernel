/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Public Variables                                                           *
 *============================================================================*/

const struct phys_memory_region phys_memory_layout[2] = {
    {
        .pbase = KERNEL_BASE_PHYS,
        .pend = KERNEL_END_PHYS,
        .size = KMEM_SIZE,
        .writable = true,
        .executable = true,
        .desc = "kernel",
    },
    {
        .pbase = KPOOL_BASE_PHYS,
        .pend = KPOOL_END_PHYS,
        .size = KPOOL_SIZE,
        .writable = true,
        .executable = false,
        .desc = "kpool",
    },
};

/**
 * @brief Memory layout.
 */
struct virt_memory_region mem_layout[VMEM_REGION] = {
    {
        .phys = phys_memory_layout[0],
        .vbase = KERNEL_BASE_VIRT,
        .vend = KERNEL_END_VIRT,
        .root_pgtab_num = 0,
    },
    {
        .phys = phys_memory_layout[1],
        .vbase = KPOOL_BASE_VIRT,
        .vend = KPOOL_END_VIRT,
        .root_pgtab_num = 1,
    },
};

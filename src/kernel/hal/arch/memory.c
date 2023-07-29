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

/**
 * @brief Memory layout.
 */
struct memory_region mem_layout[MEM_REGIONS] = {
    {.pbase = KERNEL_BASE_PHYS,
     .vbase = KERNEL_BASE_VIRT,
     .pend = KERNEL_END_PHYS,
     .vend = KERNEL_END_VIRT,
     .size = KMEM_SIZE,
     .writable = true,
     .executable = true,
     .root_pgtab_num = 0,
     .desc = "kernel"},
    {.pbase = KPOOL_BASE_PHYS,
     .vbase = KPOOL_BASE_VIRT,
     .pend = KPOOL_END_PHYS,
     .vend = KPOOL_END_VIRT,
     .size = KPOOL_SIZE,
     .writable = true,
     .executable = false,
     .root_pgtab_num = 1,
     .desc = "kpool"},
};

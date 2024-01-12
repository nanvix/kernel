/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Initializes the Hardware Abstraction Layer (HAL).
 */
void hal_init(void)
{
    cpu_init();
    exceptions_init();
    interrupts_init();
    mmu_init();
}

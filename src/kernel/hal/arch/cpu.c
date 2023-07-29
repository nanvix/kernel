/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Initializes the CPU.
 */
void cpu_init(void)
{
    kprintf("[hal] initializing cpu...");
    gdt_init();
    unsigned kernel_cs = gdt_kernel_cs();
    idt_init(kernel_cs);
    lpic_init(0x20, 0x28);
    timer_init(32);
}

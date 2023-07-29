/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>

/**
 * The mm_init() function initializes the Memory Management (MM)
 * system. It first performs assertions on memory structures handled
 * by the Hardware Abstraction Layer (HAL) and then initializes its
 * internal subsystems: (i) the Page Frame Allocator; (ii) the Kernel
 * Page Pool; and (iii) the User Page Pool.
 *
 * @see frame_init(), kpool_init and upool_init().
 *
 * @author Pedro Henrique Penna
 */
void mm_init(void)
{
    kprintf("[kernel][mm] initializing the memory system");
    frame_init();
    kpool_init();
}

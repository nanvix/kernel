/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm/thread.h>

/**
 * @details Initializes the processor management subsystem.
 */
void pm_init(vmem_t root_vmem)
{
    kprintf("[kernel] initializing processor management subsystem...");
    process_init(root_vmem);
}

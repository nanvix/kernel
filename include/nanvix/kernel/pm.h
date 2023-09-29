/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_PM_H_
#define NANVIX_KERNEL_PM_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/pm/cond.h>
#include <nanvix/kernel/pm/semaphore.h>
#include <nanvix/kernel/pm/thread.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Initializes the processor management subsystem.
 *
 * @param root_pgdir Root page directory.
 */
extern void pm_init(const void *root_pgdir);

#endif /* NANVIX_KERNEL_PM_H_ */

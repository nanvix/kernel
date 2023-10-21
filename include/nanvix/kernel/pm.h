/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_PM_H_
#define NANVIX_KERNEL_PM_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm/cond.h>
#include <nanvix/kernel/pm/semaphore.h>
#include <nanvix/kernel/pm/thread.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Initializes the processor management subsystem.
 *
 * @param root_vmem Root virtual memory space.
 */
extern void pm_init(const struct vmem *root_vmem);

#endif /* NANVIX_KERNEL_PM_H_ */

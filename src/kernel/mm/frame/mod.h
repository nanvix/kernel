/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef KERNEL_MM_FRAME_H_
#define KERNEL_MM_FRAME_H_

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[kernel][mm][frame]"

/**
 * @brief Number of page frames for user use.
 */
#define NUM_FRAMES (MEMORY_SIZE / PAGE_SIZE)

/*============================================================================*/

#endif /* KERNEL_MM_FRAME_H_ */

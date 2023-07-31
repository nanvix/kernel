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

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Runs unit tests on the Page Frame Allocator.
 */
extern void test_frame(void);

/*============================================================================*/

#endif /* KERNEL_MM_FRAME_H_ */

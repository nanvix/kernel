/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef KERNEL_MM_UPOOL_H_
#define KERNEL_MM_UPOOL_H_

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[kernel][mm][upool]"

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Runs unit tests on the User Page Allocator.
 */
extern void test_upool(void);

/*============================================================================*/

#endif /* KERNEL_MM_UPOOL_H_ */

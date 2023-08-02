/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef KERNEL_MM_KPOOL_H_
#define KERNEL_MM_KPOOL_H_

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[kernel][mm][kpool]"

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Runs unit tests on the Kernel Page Allocator.
 */
extern void test_kpool(void);

/*============================================================================*/

#endif /* KERNEL_MM_KPOOL_H_ */

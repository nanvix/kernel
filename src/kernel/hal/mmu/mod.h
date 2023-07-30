/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef HAL_MMU_H_
#define HAL_MMU_H_

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[hal][mmu]"

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Runs self-tests on the MMU module.
 */
extern void test_mmu(void);

#endif /* HAL_MMU_H_ */

/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef HAL_EXCEPTION_H_
#define HAL_EXCEPTION_H_

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[hal][exception]"

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Runs self-tests on the exceptions module.
 */
extern void test_exception(void);

#endif /* HAL_EXCEPTION_H_ */

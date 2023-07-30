/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef HAL_INTERRUPT_H_
#define HAL_INTERRUPT_H_

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[hal][interrupt]"

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Runs self-tests on the interrupts module.
 */
extern void test_interrupts(void);

#endif /* HAL_INTERRUPT_H_ */

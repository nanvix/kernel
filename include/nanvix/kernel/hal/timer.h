/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_TIMER_H_
#define NANVIX_KERNEL_HAL_TIMER_H_

#ifndef _ASM_FILE_

/**
 * @brief Initializes the timer device.
 *
 * @param freq Target frequency for the timer device.
 */
extern void timer_init(unsigned freq);

#endif /* _ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_TIMER_H_ */

/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_H_
#define NANVIX_KERNEL_HAL_ARCH_H_

#if defined(__x86__)
#include <nanvix/kernel/hal/arch/x86.h>
#else
#error "unkown architecture"
#endif

#endif /* NANVIX_KERNEL_HAL_ARCH_H_ */

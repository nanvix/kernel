/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_ARCH_H_
#define ARCH_ARCH_H_

#if defined(__x86__)
#include <arch/x86.h>
#else
#error "unkown architecture"
#endif

#endif /* ARCH_ARCH_H_ */

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_KARGS_H_
#define NANVIX_KERNEL_KARGS_H_

/**
 * @addtogroup kernel-kargs Kernel Arguments
 * @ingroup kernel
 *
 * @brief Kernel Arguments
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Kernel arguments.
 */
struct kargs;

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Parses kernel arguments.
 *
 * @param args Kernel arguments.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kargs_parse(struct kargs *args);

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_KMOD_H_ */

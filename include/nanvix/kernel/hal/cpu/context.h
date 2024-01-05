/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_CPU_CONTEXT_H_
#define NANVIX_KERNEL_HAL_CPU_CONTEXT_H_

#include <nanvix/kernel/hal/arch.h>

#ifndef _ASM_FILE_

/**
 * @brief Dumps context information.
 *
 * @param ctx Saved execution context.
 */
extern void context_dump(const struct context *ctx);

/**
 * @brief Initializes an execution context.
 *
 * @param ctx   Storage location for the execution context.
 * @param pgdir Page directory.
 * @param kbp   Kernel stack base pointer.
 * @param ksp   Kernel stack pointer.
 *
 * @returns Upon successful completion, zero is returned. Upon failure,
 * a negative number is returned instead.
 */
extern int context_create(struct context *ctx, const void *pgdir,
                          const void *kbp, const void *ksp);

/**
 * @brief Switches execution context.
 *
 * @param from Storage location for calling context.
 * @param to   Storage location for target context.
 */
extern void __context_switch(struct context *from, struct context *to);

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_CPU_CONTEXT_H_ */

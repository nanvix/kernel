/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_UTHREAD_H_
#define NANVIX_KERNEL_HAL_UTHREAD_H_

#ifndef _ASM_FILE_

/**
 * @brief Forges a user thread stack.
 *
 * @param ustack User stack.
 * @param arg    Argument to the user thread.
 * @param func  User thread function.
 *
 * @returns Upon successful completion, a pointer to the top of the
 * user thread stack is returned. Upon failure, NULL is returned
 * instead.
 */
extern void *uthread_forge_stack(void *ustack, void *arg, void *(*func)());

#endif /* _ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_UTHREAD_H_ */

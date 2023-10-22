/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef KERNEL_KCALL_MOD_H_
#define KERNEL_KCALL_MOD_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/mm.h>
#include <stddef.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Issues a void kernel call that takes no arguments.
 *
 * @return Always returns zero.
 */
extern int kcall_void0(void);

/**
 * @brief Issues a void kernel call that takes one argument.
 *
 * @param arg0 First argument.
 *
 * @return Always returns @p arg0.
 */
extern int kcall_void1(int arg0);

/**
 * @brief Issues a void kernel call that takes two arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 *
 * @return Always returns @p arg0 + @p arg1.
 */
extern int kcall_void2(int arg0, int arg1);

/**
 * @brief Issues a void kernel call that takes three arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 * @param arg2 Third argument.
 *
 * @return Always returns @p arg0 + @p arg1 + @p arg2.
 */
extern int kcall_void3(int arg0, int arg1, int arg2);

/**
 * @brief Issues a void kernel call that takes four arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 * @param arg2 Third argument.
 * @param arg3 Fourth argument.
 *
 * @return Always returns @p arg0 + @p arg1 + @p arg2 + @p arg3.
 */
extern int kcall_void4(int arg0, int arg1, int arg2, int arg3);

/**
 * @brief Issues a void kernel call that takes five arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 * @param arg2 Third argument.
 * @param arg3 Fourth argument.
 * @param arg4 Fifth argument.
 *
 * @return Always returns @p arg0 + @p arg1 + @p arg2 + @p arg3 + @p arg4.
 */
extern int kcall_void5(int arg0, int arg1, int arg2, int arg3, int arg4);

/**
 * @brief Shutdowns the system
 */
extern noreturn void kcall_shutdown(void);

/**
 * @brief Writes a buffer to a file descriptor.
 *
 * @param fd  Target file descriptor.
 * @param buf Target buffer.
 * @param n   Number of bytes to write.
 *
 * @returns The number of bytes written is returned.
 */
extern size_t kcall_write(int fd, const char *buf, size_t n);

/**
 * @brief Attempts to allocate a page frame.
 *
 * @return On success, the number of the allocated page frame is returned.
 * On failure, @p FRAME_NULL is returned instead.
 */
extern frame_t kcall_fralloc(void);

/**
 * @brief Frees a page frame.
 *
 * @param frame Number of the target page frame.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
int kcall_frfree(frame_t frame);

/*============================================================================*/

#endif /* KERNEL_KCALL_MOD_H_ */

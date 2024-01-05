/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_STDOUT_H_
#define NANVIX_KERNEL_HAL_STDOUT_H_

#ifndef _ASM_FILE_

#include <stddef.h>

/**
 * @brief Initializes the standard output device.
 */
extern void stdout_init(void);

/**
 * @brief Writes to the standard output device.
 *
 * @param buf Target buffer.
 * @param n   Number of bytes to write.
 */
extern void stdout_write(const char *buf, size_t n);

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_STDOUT_H_ */

/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef DEV_UART_H_
#define DEV_UART_H_

#ifndef _ASM_FILE_
#include <stddef.h>
#include <stdint.h>
#endif /* _ASM_FILE_ */

/**
 * @brief Divisors for Common Baud Rates
 */
/**@{*/
#define UART_BAUD_115200 1 /** 115200 Baud */
#define UART_BAUD_57600 2  /** 57600 Baud  */
#define UART_BAUD_38400 3  /** 38400 Baud  */
/**@}*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes the UART device.
 *
 * @param addr Base address for UART device.
 * @param baud Baud rate.
 *
 * @return On successful completion, zero is returned. On failure -1 returned
 * instead.
 */
extern int uart_init(uintptr_t addr, uint16_t baud);

/**
 * @brief Writes a buffer on the UART device.
 *
 * @param buf Target buffer.
 * @param len Length of the buffer.
 */
extern void uart_write(const char *buf, size_t len);

#endif /* _ASM_FILE_ */

#endif /* DEV_UART_H_ */

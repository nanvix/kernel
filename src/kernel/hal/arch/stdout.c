/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <nanvix/kernel/hal.h>
#include <stddef.h>

/**
 * @details Initializes the standard output device.
 */
void stdout_init(void)
{
    uart_init(UART_PORT_0, UART_BAUD_38400);
}

/**
 * @details Writes to the standard output device @p n bytes from the buffer
 * pointed to by @p buf.
 */
void stdout_write(const char *buf, size_t n)
{
    uart_write(buf, n);
}

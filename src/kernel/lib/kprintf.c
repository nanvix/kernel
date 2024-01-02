/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <nanvix/kernel/lib.h>
#include <nanvix/libcore.h>
#include <stdarg.h>

/**
 * @brief Buffer size (in bytes).
 */
#define KBUFFER_SIZE 128

/**
 * @details Writes a formatted string on the kernels's output device.
 */
void kprintf(const char *fmt, ...)
{
    size_t len;                    /* String length.                  */
    va_list args;                  /* Variable arguments list.        */
    char buffer[KBUFFER_SIZE + 2]; /* Temporary buffer (+2 for \n\0). */

    /* Convert to raw string. */
    va_start(args, fmt);

    /* Give it 1 extra byte because the size includes '\0'. */
    len = __vsnprintf(buffer, KBUFFER_SIZE + 1, fmt, args);
    /* Substitute the ending with \n\0. */
    buffer[++len] = '\n';
    buffer[++len] = '\0';

    va_end(args);

    kputs(buffer);
}

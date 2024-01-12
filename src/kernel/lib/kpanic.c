/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <nanvix/cc.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/libcore.h>
#include <stdarg.h>
#include <stdnoreturn.h>

/**
 * @brief Buffer size (in bytes).
 */
#define KBUFFER_SIZE 128

/**
 * @details The kpanic() function writes the formatted message pointed to by @p
 * fmt to the standard output device and panics the kernel. In panic mode, all
 * interrupts are disabled in the underlying core, and execution loops
 * indefinitely.
 */
noreturn void kpanic(const char *fmt, ...)
{
    size_t len = 0;                /* String length.                  */
    va_list args;                  /* Variable arguments list.        */
    char buffer[KBUFFER_SIZE + 2]; /* Temporary buffer (+2 for \n\0). */
    const char *panic_str = "PANIC: ";
    const size_t panic_str_len = __strlen(panic_str);

    __strncpy(buffer, panic_str, panic_str_len);

    /* Convert to raw string. */
    va_start(args, fmt);
    len = __vsnprintf(
        buffer + panic_str_len, KBUFFER_SIZE - panic_str_len + 1, fmt, args);
    len += panic_str_len;
    buffer[++len] = '\n';
    buffer[++len] = '\0';
    va_end(args);

    kputs(buffer);

    /* We don't want to be troubled. */
    __khalt_fn();

    /* Stay here, forever. */
    UNREACHABLE();
}

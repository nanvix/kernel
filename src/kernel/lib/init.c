/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stddef.h>

/**
 * @details Stdout write function.
 */
void (*__kwrite_fn)(const char *, size_t) = NULL;

/**
 * @brief Target halt function.
 */
void (*__khalt_fn)(void) = NULL;

/**
 * @details Initializes the kernel libary
 */
void klib_init(void (*write_fn)(const char *, size_t), void (*halt_fn)(void))
{
    __kwrite_fn = write_fn;
    __khalt_fn = halt_fn;
}

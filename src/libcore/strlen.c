/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stddef.h>

/**
 * @details The __strlen() function computes the length of the string pointed to
 * by @p s.
 */
size_t __strlen(const char *str)
{
    const char *p = str;

    /* Count the number of characters. */
    while (*p++ != '\0') {
        /* No operation.*/;
    }

    return (p - str - 1);
}

/**
 * @details This is an internal function for the __strlen() function.
 */
size_t strlen(const char *str)
{
    return (__strlen(str));
}

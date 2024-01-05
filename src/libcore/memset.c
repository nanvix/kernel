/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stddef.h>

/**
 * @details The __memset() function copies the value of @p c (converted to an
 * unsigned char) into each of the first @p n characters of the object pointed
 * to by @p s.
 */
void *__memset(void *s, int c, size_t n)
{
    unsigned char *p = s;

    while (n-- > 0) {
        *p++ = c;
    }

    return (s);
}

/**
 * @details This is an internal function for the __memset() function.
 */
void *memset(void *s, int c, size_t n)
{
    return (__memset(s, c, n));
}

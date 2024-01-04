/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stddef.h>

/**
 * @details The memcmp() function compares the first @p n bytes of the memory
 * areas @p s1 and @p s2. It returns an integer less than, equal to, or greater
 * than zero if @p s1 is found, respectively, to be less than, to match, or be
 * greater than @p s2.
 */
int __memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;

    while (n-- > 0) {
        if (*p1++ != *p2++)
            return (*--p1 - *--p2);
    }

    return (0);
}

/**
 * @details This is an internal function for the memcmp() function.
 */
int memcmp(const void *s1, const void *s2, size_t n)
{
    return (__memcmp(s1, s2, n));
}

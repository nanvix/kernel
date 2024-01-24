/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stddef.h>

/**
 * @details The __strncmp() function compares not more than @p n
 * characters (characters that follow a null character are not
 * compared) from the array pointed to by @p s1 to the array pointed to
 * by @p s2. If copying takes place between objects that overlap, the
 * behavior is undefined.
 */
int __strncmp(const char *s1, const char *s2, size_t n)
{
    // Compare strings.
    while (n > 0) {
        // Strings differ.
        if (*s1 != *s2) {
            return ((*(unsigned char *)s1 - *(unsigned char *)s2));

        }

        /* End of string. */
        else if (*s1 == '\0') {
            return (0);
        }

        s1++;
        s2++;
        n--;
    }

    return (0);
}

/**
 * @details This function is an internal version for __strncmp().
 */
int strncmp(const char *s1, const char *s2, size_t n)
{
    return (__strncmp(s1, s2, n));
}

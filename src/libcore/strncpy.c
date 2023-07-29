/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stddef.h>

/**
 * @details The __strncpy() function copies not more than @p n characters
 * (characters that follow a null character are not copied) from the
 * array pointed to by @p s2 to the array pointed to by @p s1) If
 * copying takes place between objects that overlap, the behavior is
 * undefined.
 */
char *__strncpy(char *s1, const char *s2, size_t n)
{
    char *p1 = s1;       /* Indexes s1. */
    const char *p2 = s2; /* Indexes s2.  */

    /* Copy string. */
    while (n > 0) {
        if (*p2 == '\0') {
            break;
        }

        *p1++ = *p2++;
        n--;
    }

    /* Fill with null bytes. */
    while (n > 0) {
        *p1++ = '\0';
        n--;
    }

    return (s1);
}

/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stddef.h>

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(X, Y)                                                        \
    (((long)X & (sizeof(long) - 1)) | ((long)Y & (sizeof(long) - 1)))

/* How many bytes are copied each iteration of the 4X unrolled loop.  */
#define BIGBLOCKSIZE (sizeof(long) << 2)

/* How many bytes are copied each iteration of the word copy loop.  */
#define LITTLEBLOCKSIZE (sizeof(long))

/* Threshold for punting to the byte copier.  */
#define TOO_SMALL(LEN) ((LEN) < BIGBLOCKSIZE)

/**
 * @details The __memcpy() function copies @p n characters from the object
 * pointed to by @p s2 into the object pointed to by @p s1. If copying
 * takes place between objects that overlap, the behavior is
 * undefined.
 */
void *__memcpy(void *s1, const void *s2, size_t n)
{
    char *dst = s1;
    const char *src = s2;
    long *aligned_dst;
    const long *aligned_src;

    /* If the size is small, or either SRC or DST is unaligned,
       then punt into the byte copy loop.	This should be rare. */
    if (!TOO_SMALL(n) && !UNALIGNED(src, dst)) {
        aligned_dst = (long *)dst;
        aligned_src = (long *)src;

        /* Copy 4X long words at a time if possible. */
        while (n >= BIGBLOCKSIZE) {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            n -= BIGBLOCKSIZE;
        }

        /* Copy one long word at a time if possible. */
        while (n >= LITTLEBLOCKSIZE) {
            *aligned_dst++ = *aligned_src++;
            n -= LITTLEBLOCKSIZE;
        }

        /* Pick up any residual with a byte copier. */
        dst = (char *)aligned_dst;
        src = (char *)aligned_src;
    }

    while (n--)
        *dst++ = *src++;

    return s1;
}

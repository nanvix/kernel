/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <nanvix/kernel/lib.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Returns the number of bits that are set in a bitmap.
 *
 * @details Counts the number of bits that are set in a bitmap using a
 *		  bit-hacking algorithm from Stanford.
 *
 * @param bitmap Bitmap to be searched.
 * @param size   Size (in bytes) of the bitmap.
 *
 * @returns The number of bits that are set in the bitmap.
 */
bitmap_t bitmap_nset(bitmap_t *bitmap, size_t size)
{
    bitmap_t count; /* Number of bits set. */
    bitmap_t *idx;  /* Loop index.         */
    bitmap_t *end;  /* End of bitmap.      */
    bitmap_t chunk; /* Working chunk.      */

    /**
     * @brief Assert bitmap size.
     */
    ((void)sizeof(
        char[(((sizeof(bitmap_t)) == (BITMAP_WORD_LENGTH / 8)) ? 1 : -1)]));

    /* Count the number of bits set. */
    count = 0;
    end = (bitmap + (size >> 2));
    for (idx = bitmap; idx < end; idx++) {
        chunk = *idx;

        /*
         * Fast way for counting number of bits set in a bit map.
         * I have no idea how does it work. I just got it from here:
         * https://graphics.stanford.edu/~seander/bithacks.html
         */
        chunk = chunk - ((chunk >> 1) & 0x55555555);
        chunk = (chunk & 0x33333333) + ((chunk >> 2) & 0x33333333);
        count += (((chunk + (chunk >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    }

    return (count);
}

/**
 * @brief Returns the number of bits that are cleared in a bitmap.
 *
 * @details Counts the number of bits that are cleared in a bitmap using a
 *		  bit-hacking algorithm from Stanford.
 *
 * @param bitmap Bitmap to be searched.
 * @param size   Size (in bytes) of the bitmap.
 *
 * @returns The number of bits that are cleared in the bitmap.
 */
bitmap_t bitmap_nclear(bitmap_t *bitmap, size_t size)
{
    return ((size << 3) - bitmap_nset(bitmap, size));
}

/**
 * @brief Searches for the first free bit in a bitmap.
 *
 * @details Searches for the first free bit in a bitmap. In order to speedup
 * computation, bits are checked in chunks of 4 bytes.
 *
 * @param bitmap Bitmap to be searched.
 * @param start  Start index.
 * @param size   Size (in bytes) of the bitmap.
 *
 * @returns If a free bit is found, the number of that bit is returned. However,
 * if no free bit is found #BITMAP_FULL is returned instead.
 */
bitmap_t bitmap_first_free(bitmap_t *bitmap, bitmap_t start, size_t size)
{
    bitmap_t *max;          /* Bitmap bondary. */
    register bitmap_t off;  /* Bit offset.     */
    register bitmap_t *idx; /* Bit index.      */

    idx = bitmap + (start >> BITMAP_WORD_SHIFT);
    max = (bitmap + (size >> 2));

    /* Find bit index. */
    while (idx < max) {
        /* Index found. */
        if (*idx != 0xffffffff) {
            off = 0;

            /* Find offset. */
            while (*idx & (0x1 << off))
                off++;

            return (((idx - bitmap) << BITMAP_WORD_SHIFT) + off);
        }

        idx++;
    }

    return (BITMAP_FULL);
}

/**
 * @brief Checks what is the value of the nth bit.
 *
 * @details Return the value of the bit on the nth position.
 *
 * @param bitmap Bitmap to be checked.
 * @param idx Index of the bitmap to be checked.
 *
 * @returns The value of the bit in the idx position.
 */
bitmap_t bitmap_check_bit(bitmap_t *bitmap, bitmap_t idx)
{
    return (bitmap[IDX(idx)] & (1 << OFF(idx)));
}

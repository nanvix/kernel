/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_LIB_H_
#define NANVIX_KERNEL_LIB_H_

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Bitmap                                                                     *
 *============================================================================*/

/**
 * @brief Shift of a bitmap word.
 */
#define BITMAP_WORD_SHIFT 5

/**
 * @brief Length of a bitmap word.
 */
#define BITMAP_WORD_LENGTH (1 << BITMAP_WORD_SHIFT)

/**
 * @brief Bitmap word.
 */
typedef uint32_t bitmap_t;

/**
 * @brief Full bitmap.
 */
#define BITMAP_FULL 0xffffffff

/**
 * @name Bitmap Operators
 */
#define IDX(a) ((a) >> BITMAP_WORD_SHIFT) /** Returns the index of the bit. */
#define OFF(a) ((a)&0x1F)                 /** Returns the offset of the bit. */

/**
 * @brief Sets a bit in a bitmap.
 *
 * @param bitmap Bitmap where the bit should be set.
 * @param pos	Position of the bit that shall be set.
 */
#define bitmap_set(bitmap, pos)                                                \
    (((bitmap_t *)(bitmap))[IDX(pos)] |= (0x1 << OFF(pos)))

/**
 * @brief Clears a bit in a bitmap.
 *
 * @param bitmap Bitmap where the bit should be cleared.
 * @param pos	Position of the bit that shall be cleared.
 */
#define bitmap_clear(bitmap, pos)                                              \
    (((bitmap_t *)(bitmap))[IDX(pos)] &= ~(0x1 << OFF(pos)))

extern bitmap_t bitmap_nset(bitmap_t *, size_t);
extern bitmap_t bitmap_nclear(bitmap_t *, size_t);
extern bitmap_t bitmap_first_free(bitmap_t *, size_t);
extern bitmap_t bitmap_check_bit(bitmap_t *, bitmap_t);

/*============================================================================*
 * Logging and Debugging                                                      *
 *============================================================================*/

/**
 * @brief Panics the kernel.
 *
 * @param fmt Panic message.
 */
extern noreturn void kpanic(const char *fmt, ...);

/**
 * @brief Writes a formatted string on the kernels's output device.
 *
 * @param fmt Formatted string.
 */
extern void kprintf(const char *, ...);

/**
 * @brief Prints a string on the standard output device.
 *
 * @param str Target string.
 */
extern void kputs(const char *str);

/*============================================================================*
 * Misc                                                                       *
 *============================================================================*/

/**
 * @brief Asserts if 'a' and 'b' agree on size.
 *
 * @param a Probing size.
 * @param b Control size.
 *
 * @returns Upon success, compilation proceeds as normal. Upon
 * failure, a compilation error is generated.
 */
#define KASSERT_SIZE(a, b) ((void)sizeof(char[(((a) == (b)) ? 1 : -1)]))

/**
 * @brief Returns the length of an array.
 *
 * @param x Target array.
 *
 * @returns The length of the array @p x.
 */
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

/**
 * @brief Aligns a value to a boundary.
 *
 * @param x Target value.
 * @param a Boundary.
 *
 * @returns A aligned value.
 */
#define ALIGN(x, a) ((x) & ~((a)-1))

/**
 * @brief Truncates a value on a boundary.
 *
 * @param x Target value
 * @param a Boundary.
 *
 * @returns A truncated value.
 */
#define TRUNCATE(x, a) (((x) + ((a)-1)) & ~((a)-1))

/**
 * @brief HAL assert.
 *
 * The kassert() function asserts if @p expr evaluates to non-zero. If
 * not, it panics the kernel with the @p msg.
 *
 * @see kpanic()
 */
static inline void _kassert(int expr, const char *msg, const char *filename,
                            int linenum)
{
    if (!expr) {
        kpanic("%s (%s : %d)", (msg != NULL) ? msg : "", filename, linenum);
    }
}

/**
 * @see _kassert()
 */
#ifdef NDEBUG
#define kassert(x, msg)                                                        \
    {                                                                          \
        UNUSED(x);                                                             \
        UNUSED(msg);                                                           \
    }
#else
#define kassert(x, msg) _kassert(x, msg, __FILE__, __LINE__)
#endif

/**
 * @brief Wrapper for kassert().
 *
 * The KASSERT() is indeed a wrapper macro for kassert() where we
 * do not want to print an error message. It is useful to assert
 * parameters of kernel routines.
 */
#define KASSERT(x) kassert(x, "kassert() failed")

/*============================================================================*
 * Initialization                                                             *
 *============================================================================*/

/**
 * @brief Initializes the kernel libary
 *
 * @param write_fn Stdout write function.
 * @param halt_fn  Target halt function.
 */
extern void klib_init(void (*write_fn)(const char *, size_t),
                      void (*halt_fn)(void));

/**
 * @brief Stdout write function.
 */
extern void (*__kwrite_fn)(const char *, size_t);

/**
 * @brief Target halt function.
 */
extern void (*__khalt_fn)(void);

#endif /* NANVIX_KERNEL_LIB_H_ */

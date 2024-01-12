/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_LIBCORE_H_
#define NANVIX_LIBCORE_H_
#ifndef _ASM_FILE_

#include <stdarg.h>
#include <stddef.h>

/*============================================================================*
 * Math                                                                       *
 *============================================================================*/

/**
 * @brief Divides two integers.
 *
 * @param a First operand.
 * @param b Second operand.
 *
 * @returns The result of @p a divided by @p b.
 */
extern int __div(int a, int b);

/*============================================================================*
 * Memory Manipulation                                                        *
 *============================================================================*/

/**
 * @brief Compares bytes in memory.
 *
 * @param s1 Pointer to object 1.
 * @param s2 Pointer to object 2.
 * @param n  Number of bytes to compare.
 *
 * @returns An integer greater than, equal to or less than 0, if the object
 * pointed to by @p s1 is greater than, equal to or less than the object pointed
 * to by @p s2 respectively.
 */
extern int __memcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief Copy bytes in memory.
 *
 * @param s1 Target memory area.
 * @param s2 Source memory area.
 * @param n  Number of bytes to be copied.
 *
 * @returns A pointer to the target memory area.
 */
extern void *__memcpy(void *s1, const void *s2, size_t n);

/**
 * @brief Sets bytes in memory.
 *
 * @param s Pointer to target memory area.
 * @param c Character to use.
 * @param n Number of bytes to be set.
 *
 * @returns A pointer to the target memory area.
 */
extern void *__memset(void *s, int c, size_t n);

/*============================================================================*
 * Memory Manipulation                                                        *
 *============================================================================*/

/**
 * @brief Returns the length of a string.
 *
 * @param str String to be evaluated.
 *
 * @returns The length of the string.
 */
extern size_t __strlen(const char *str);

/**
 * @brief Copies part of a string.
 *
 * @param s1 Target string.
 * @param s2 Source string.
 * @param n  Number of characters to be copied.
 *
 * @returns A pointer to the target string.
 */
extern char *__strncpy(char *s1, const char *s2, size_t n);

/*============================================================================*
 * String Formatting                                                          *
 *============================================================================*/

/**
 * @brief Writes at most size bytes of formatted data to str.
 *
 * @param str	Output string.
 * @param size	Write at most size bytes (including null byte).
 * @param fmt	Formatted string.
 * @param args	Variable arguments list.
 *
 * @returns Length of output string (excluding terminating null byte). If size
 * is 0 or fmt string is NULL or empty, the function returns -1.
 */
extern int __vsnprintf(char *str, size_t size, const char *fmt, va_list args);

#endif /* !_ASM_FILE_ */
#endif /* NANVIX_LIBCORE_H_ */

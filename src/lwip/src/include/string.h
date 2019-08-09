/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ULIBC_STRING_H_
#define ULIBC_STRING_H_

	#include <stddef.h>

/*============================================================================*
 * Copying Functions                                                          *
 *============================================================================*/

	/**
	 * @brief Copies bytes in memory.
	 *
	 * @param s1 Pointer to target object.
	 * @param s2 Pointer to source object.
	 * @param n  Number of bytes to copy.
	 *
	 * @returns @p s1 is returned.
	 *
	 * @note If copying takes place between objects that overlap, the behavior is
	 *       undefined.
	 */
	extern void *memcpy(void *restrict s1, const void *restrict s2, size_t n);

	/**
	 * @brief Copies bytes in memory with overlapping areas.
	 *
	 * @param s1 Pointer to target object.
	 * @param s2 Pointer to source object.
	 * @param n  Number of bytes to copy.
	 *
	 * @returns @p s1 is returned.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern void *memmove(void *s1, const void *s2, size_t n);

	/**
	 * @brief Copies a string.
	 *
	 * @param s1 Pointer to target string.
	 * @param s2 Pointer to source string.
	 *
	 * @returns s1 is returned.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strcpy(char *restrict s1, const char *restrict s2);

	/**
	 * @brief Copies part of a string.
	 *
	 * @param s1 Pointer to target string.
	 * @param s2 Pointer to source string.
	 * @param n  Number of characters to copy.
	 *
	 * @param @p s1 is returned.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strncpy(char *restrict s1, const char *restrict s2, size_t n);

/*============================================================================*
 * Concatenation Functions                                                    *
 *============================================================================*/

	/**
	 * @brief Concatenates two strings.
	 *
	 * @param s1 Pointer to target string.
	 * @param s2 Pointer to source string.
	 *
	 * @returns @p s1 is returned.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strcat(char *restrict s1, const char *restrict s2);

	/**
	 * @brief Concatenates part of two strings.
	 *
	 * @param s1 Pointer to target string.
	 * @param s2 Pointer to source string.
	 * @param n  Number of characters to consider.
	 *
	 * @returns @p s1 is returned.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strncat(char *restrict s1, const char *restrict s2, size_t n);

/*============================================================================*
 * Comparison Functions                                                       *
 *============================================================================*/

	/**
	 * @brief Compares bytes in memory.
	 *
	 * @param s1 Pointer to object 1.
	 * @param s2 Pointer to object 2.
	 * @param n  Number of bytes to compare.
	 *
	 * @returns An integer greater than, equal to or less than 0, if the object
	 *          pointed to by @p s1 is greater than, equal to or less than the
	 *          object pointed to by @p s2 respectively.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern int memcmp(const void *s1, const void *s2, size_t n);

	/**
	 * @brief Compares two strings.
	 *
	 * @param s1 Pointer to first string.
	 * @param s2 Pointer to second string.
	 *
	 * @returns An integer greater than, equal to or less than 0, if the string
	 *          pointed to by @p s1 is greater than, equal to or less than the
	 *          string pointed to by @p s2 respectively.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern int strcmp(const char *s1, const char *s2);

	/**
	 * @brief String comparison using collating information.
	 *
	 * @param s1 Pointer to first string.
	 * @param s2 Pointer to second string.
	 *
	 * @returns An integer greater than, equal to, or less than 0, according to
	 *          whether the string pointed to by @p s1 is greater than, equal to,
	 *          or less than the string pointed to by @p s2 when both are
	 *          interpreted as appropriate to the current locale.
	 *
	 * @note On error errno may be set, but no return value is reserved to indicate
	 *       an error.
	 *
	 * @todo Perform comparison using collating information.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern int strcoll(const char *s1, const char *s2);

	/**
	 * @brief Compares part of two strings.
	 *
	 * @param s1 Pointer to first string.
	 * @param s2 Pointer to second string.
	 * @param n  Number of characters to consider.
	 *
	 * @returns An integer greater than, equal to or less than 0, if the possibly
	 *          null-terminated array pointed to by @p s1 is greater than, equal to
	 *          or less than the possibly null-terminated array pointed to by @p s2
	 *          respectively.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern int strncmp(const char *restrict s1, const char *restrict s2, size_t n);

	/**
	 * @brief Transforms a string into another.
	 *
	 * @param s1 Pointer to target string.
	 * @param s2 Pointer to source string.
	 * @param n  Maximum bytes to consider.
	 *
	 * @returns The length of the transformed string (not including the terminating
	 *          null byte) is returned. If the value returned is @p n or more, the
	 *          contents of the array pointed to by @p s1 are indeterminate.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern size_t strxfrm(char *s1, const char *s2, size_t n);

/*============================================================================*
 * Search Functions                                                           *
 *============================================================================*/

	/**
	 * @brief Finds a byte in memory.
	 *
	 * @param s Where to search from.
	 * @param c Byte to be located.
	 * @param n Maximum number of bytes to consider.
	 *
	 * @returns A pointer to the located byte, or a null pointer if the byte does
	 *          not occur in the object.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern void *memchr(const void *s, int c, size_t n);

	/**
	 * @brief Finds a byte in string.
	 *
	 * @param s String to search.
	 * @param c Character to search.
	 *
	 * @returns A pointer to the byte, or a null pointer if the byte was not found.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strchr(const char *s, int c);

	/**
	 * @brief Gets the length of a complementary substring.
	 *
	 * @param s1 Pointer to string to search.
	 * @param s2 Pointer to string to be checked against.
	 *
	 * @returns The length of the computed segment of the string pointed to by
				@p s1.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern size_t strcspn(const char *s1, const char *s2);

	/**
	 * @brief Scans a string for a byte.
	 *
	 * @param s1 Pointer to string to scan.
	 * @param s2 Pointer to string of bytes to scan for.
	 *
	 * @returns A pointer to the byte or a null pointer if no byte from @p s2
	 *          occurs in @p s1.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strpbrk(const char *s1, const char *s2);

	/**
	 * @brief String scanning operation.
	 *
	 * @param s Pointer to string to search.
	 * @param c Character to search for.
	 *
	 * @returns A pointer to the byte, or a null pointer if @p c does not occur in
	 *          the string.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strrchr(const char *s, int c);

	/**
	 * @brief Gets the length of a substring.
	 *
	 * @param Pointer to string.
	 * @param Pointer to substring.
	 *
	 * @returns The length of @p s1 is returned.
	 */
	extern size_t strspn(const char *s1, const char *s2);

	/**
	 * @brief Finds a substring.
	 *
	 * @param s1 Pointer to string to search.
	 * @param s2 Pointer to substring to search for.
	 *
	 * @returns A pointer to the located string or a null pointer if the string is
	 *          not found. If @p s2 points to a string with zero length, the
	 *          function returns @p s1.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strstr(const char *s1, const char *s2);

	/**
	 * @brief Splits string into tokens.
	 *
	 * @param s1 Pointer to string to split.
	 * @param s2 Pointer to token string.
	 *
	 * @returns A pointer to the first byte of a token. Otherwise, if there is no
	 *          token a null pointer is returned.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strtok(char *s1, const char *s2);

/*============================================================================*
 * Miscellaneous Functions                                                    *
 *============================================================================*/

	/**
	 * @brief Sets bytes in memory.
	 *
	 * @param s Pointer to target object.
	 * @param c Character to copy.
	 * @param n Number of bytes to set.
	 *
	 * @returns @p s is returned.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern void *memset(void *s, int c, size_t n);

	/**
	 * @brief Gets error message string.
	 *
	 * @returns A pointer to the generated message string.
	 *
	 * @todo Use collating information.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern char *strerror(int errnum);

	/**
	 * @brief Gets string length.
	 *
	 * @param str Target string.
	 *
	 * @returns The length of @p str.
	 *
	 * @version IEEE Std 1003.1, 2013 Edition
	 */
	extern size_t strlen(const char *str);

#endif /* ULIBC_STRING_H_ */
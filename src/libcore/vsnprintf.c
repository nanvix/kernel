/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <stdarg.h>
#include <stddef.h>

/**
 * @brief Converts an integer to a string.
 *
 * @param str  Output string.
 * @param num  Number to be converted.
 * @param base Base to use.
 *
 * @returns The length of the output string.
 */
static int itoa(char *str, unsigned num, int base)
{
    char *b = str;
    char *p, *p1, *p2;
    unsigned divisor;

    divisor = 10;

    if (base == 'x') {
        *b++ = '0';
        *b++ = 'x';
        divisor = 16;
    }

    p = b;

    /* Convert number. */
    do {
        unsigned remainder;

        remainder = num % divisor;

        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;

    } while (num /= divisor);

    /* Fill up with zeros. */
    if (divisor == 16) {
        while ((p - b) < 8)
            *p++ = '0';
    }

    /* Reverse BUF. */
    p1 = b;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }

    return (p - str);
}

/**
 * @brief Count number of digits a number has.
 *
 * @param num Number to have digits counted.
 *
 * @returns Number of digits of num.
 */
static unsigned count_digits(unsigned num)
{
    unsigned digits = 0;
    while (num != 0) {
        num /= 10;
        ++digits;
    }

    return digits;
}

/**
 * @details Writes at most size bytes (including the terminating null byte
 * ('\0')) of formatted data to str. If the the result is larger than size, the
 * output string will be truncated. If a number needs to be truncated, not even
 * the part that fits in the output will be written, so no number will be in the
 * output.
 */
int __vsnprintf(char *str, size_t size, const char *fmt, va_list args)
{
    char *base = str;

    if (size == 0 || fmt == NULL) {
        return (-1);
    }

    /* Format string. */
    while (*fmt != '\0' && (size_t)(str - base) < (size - 1)) {
        /* No conversion needed. */
        if (*fmt != '%') {
            *str++ = *fmt;
        } else {
            switch (*(++fmt)) {
                /* Character. */
                case 'c': {
                    *str++ = (char)va_arg(args, int);
                } break;
                /* Number. */
                case 'd': {
                    unsigned num = va_arg(args, unsigned);
                    if ((size_t)(count_digits(num) + (str - base)) <=
                        (size - 1)) {
                        str += itoa(str, num, *fmt);
                    }
                } break;
                case 'x': {
                    /* Hex numbers are currently being converted
                     * using always 0x + 8 digits. */
                    if ((size_t)(10 + (str - base)) <= (size - 1))
                        str += itoa(str, va_arg(args, unsigned), *fmt);
                } break;
                /* String. */
                case 's': {
                    const char *s = va_arg(args, const char *);
                    while (*s != '\0' && (size_t)(str - base) < (size - 1)) {
                        *str++ = *s++;
                    }
                } break;
                /* Ignore. */
                default:
                    break;
            }
        }

        ++fmt;
    }

    *str = '\0';

    return ((str - base) - 1);
}

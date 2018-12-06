/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#include <nanvix/const.h>
#include <stdarg.h>

/**
 * @brief Converts an integer to a string.
 * 
 * @param str  Output string.
 * @param num  Number to be converted.
 * @param base Base to use.
 * 
 * @returns The length of the output string.
 */
PRIVATE int itoa(char *str, unsigned num, int base)
{
	char *b = str;
	char *p, *p1, *p2;
	unsigned divisor;

	if (base == 'd')
		divisor = 10;

	else
	{
		*b++ = '0'; *b++ = 'x';
		divisor = 16;
	}

	p = b;

	/* Convert number. */
	do
	{
		int remainder = num % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : 
		                          remainder + 'a' - 10;
	} while (num /= divisor);

	/* Fill up with zeros. */
	if (divisor == 16)
		while ((p - b) < 8)
			*p++ = '0';

	/* Reverse BUF. */
	p1 = b; p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1++ = *p2;
		*p2-- = tmp;
	}

	return(p - str);
}

/**
 * @brief Writes formatted data from variable argument list to a string. 
 * 
 * @param str  Output string.
 * @param fmt  Formated string.
 * @param args Variable arguments list.
 * 
 * @returns The length of the output string.
 */
PUBLIC int kvsprintf(char *str, const char *fmt, va_list args)
{
	char *base = str;
	char *s;
	
    /* Format string. */
    while (*fmt != '\0')
    {
        /* No conversion needed. */
        if (*fmt != '%')
            *str++ = *fmt;
        
        /* Parse format. */
        else
            switch (*(++fmt))
            {
				/* Character. */
                case 'c':
					*str++ = (char)va_arg(args, int);
					break;
				
				/* Number. */
				case 'd':
				case 'x':
					str += itoa(str, va_arg(args, unsigned int), *fmt);
					break;
				
				/* String. */
                case 's':
					s = va_arg(args, char*);
                    while (*s != '\0')
						*str++ = *s++;
					break;
                
                /* Ignore. */
                default:
                    break;
            }
        fmt++;
    }

    return (str - base);
}

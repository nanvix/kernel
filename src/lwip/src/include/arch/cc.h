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

#ifndef CC_H_
#define CC_H_

#include <nanvix/hal/hal.h>

#define LWIP_NO_INTTYPES_H 1

/* nanvix doesn't support all the print format that lwip use... to get proper
debug messages nanvix should support them. */
#define X8_F  "x"
#define U16_F "d"
#define S16_F "d"
#define X16_F "x"
#define U32_F "d"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "d"

#if defined(__qemu_x86__)
    #define BYTE_ORDER LITTLE_ENDIAN
#elif defined(__qemu_openrisc__)
    #define BYTE_ORDER BIG_ENDIAN
#endif

#define LWIP_PLATFORM_DIAG(x)	do {kprintf x;} while(0)

#define LWIP_PLATFORM_ASSERT(x) do {kprintf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); KASSERT(false) ;} while(0)

#define LWIP_NO_LIMITS_H 1
#define LWIP_NO_CTYPE_H 1

#define INT_MAX 2147483647

#define EWOULDBLOCK EAGAIN /**< Operation would block */

#endif /* CC_H_ */
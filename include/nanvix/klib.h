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

#ifndef NANVIX_KLIB_H_
#define NANVIX_KLIB_H_

	#include <nanvix/const.h>
	#include <stdarg.h>

/**
 * @defgroup klib Kernel Library
 * @ingroup kernel
 */

/*============================================================================*
 *                          Logging and Debugging                             *
 *============================================================================*/

/**
 * @addtogroup klib-debug Log & Debug
 * @ingroup klib
 */
/**@{*/

	/**
	 * @brief Kernel buffer size (in bytes).
	 *
	 * @note Hopefully not kernel string is longer than this.
	 */
	#define KBUFFER_SIZE 64

	/**
	 * @brief Prints a string on the standard output device.
	 *
	 * @param str Target string.
	 */
	EXTERN void kputs(const char *str);

	/* Forward definitions. */
	EXTERN void kprintf(const char *, ...);
	EXTERN int kvsprintf(char *, const char *, va_list);
	EXTERN void kmemdump(const void *, size_t);

/**@}*/

/*============================================================================*
 *                                 String                                     *
 *============================================================================*/

/**
 * @addtogroup klib-string Strings
 * @ingroup klib
 */
/**@{*/

	/* Forward definitions. */
	EXTERN int kstrcmp(const char *, const char *);
	EXTERN char *kstrcpy(char *, const char *);
	EXTERN size_t kstrlen(const char *);
	EXTERN int kstrncmp(const char *, const char *, size_t);
	EXTERN char *kstrncpy(char *, const char *, size_t);

/**@}*/

/*============================================================================*
 *                                 Memory                                     *
 *============================================================================*/

/**
 * @addtogroup klib-memory Memory
 * @ingroup klib
 */
/**@{*/

	/* Forward definitions. */
	EXTERN void *kmemcpy(void *, const void *, size_t);
	EXTERN void *kmemset(void *, int, size_t);

	/**
	 * @brief Asserts if 'a' and 'b' agrees on size.
	 *
	 * @param a Probing size.
	 * @param b Control size.
	 *
	 * @returns Upon success, compilation proceeds as normal. Upon
	 * failure, a compilation error is generated.
	 */
	#define KASSERT_SIZE(a, b) \
		((void) sizeof(char[(((a) == (b)) ? 1 : -1)]))

/**@}*/

/*============================================================================*
 *                              Miscellaneous                                 *
 *============================================================================*/

/**
 * @addtogroup klib-misc Miscellaneous
 * @ingroup klib
 */
/**@{*/

	/**
	 * @brief Kernel assert.
	 */
	#define KASSERT(x) if(x) { noop() };

	/**
	 * @brief Declares something to be unused.
	 * 
	 * @param x Thing.
	 */
	#define UNUSED(x) ((void) (x))
	
	/**
	 * @brief No operation.
	 */
	#define noop()

/**@}*/

#endif /* NANVIX_KLIB_H_ */

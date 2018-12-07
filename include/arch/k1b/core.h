/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef ARCH_K1B_CORE_H_
#define ARCH_K1B_CORE_H_

/**
 * @addtogroup k1b-core Core
 * @ingroup k1b
 *
 * @brief Interface for dealing with processor cores.
 */
/**@{*/

	#include <nanvix/const.h>

	#ifdef __k1io__
		#define K1B_NUM_CORES 4
	#else
		#define K1B_NUM_CORES 16
	#endif

	/* Forward definitions. */
	EXTERN void core_wakeup(int, void (*)(void));
	EXTERN void core_start(void);
	EXTERN void core_halt(void);
	EXTERN void shutdown(int);

/**@}*/

#endif /* ARCH_K1B_CORE_H_ */

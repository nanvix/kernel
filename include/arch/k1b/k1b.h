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

#ifndef ARCH_K1B_K1B_H_
#define ARCH_K1B_K1B_H_

	#ifndef TARGET_KALRAY_MPPA256_H_
	#error "include <target/kalray/mppa256.h> instead"
	#endif

	/**
	 * @defgroup k1b Bostan Architecture
	 * @ingroup mppa256
	 */
	#include <arch/k1b/cache.h>
	#include <arch/k1b/clock.h>
	#include <arch/k1b/core.h>
	#include <arch/k1b/cpu.h>
	#include <arch/k1b/int.h>
	#include <arch/k1b/io.h>
	#include <arch/k1b/pic.h>

#endif /* ARCH_K1B_K1B_H_ */

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

#include <nanvix/kernel/mm.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/**
 * The mm_init() function initializes the Memory Management (MM)
 * system. It first performs assertions on memory structures handled
 * by the Hardware Abstraction Layer (HAL) and then initializes its
 * internal subsystems: (i) the Page Frame Allocator; (ii) the Kernel
 * Page Pool; and (iii) the User Page Pool.
 *
 * @see frame_init(), kpool_init and upool_init().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void mm_init(void)
{
	kprintf("[mm] initializing the memory system");
	frame_init();
	kpool_init();
	upool_init();
}


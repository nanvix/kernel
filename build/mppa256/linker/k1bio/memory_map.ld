/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

/**
 * Copyright (C) 2013-2014 Kalray SA.
 *
 * All rights reserved.
 */

MEMORY
{
    /* Definitions of available memory regions */
    internal_mem           : ORIGIN = 0x00000000, LENGTH = 2M
    upper_internal_memory  : ORIGIN = 0x00200000, LENGTH = 2M

    /*
     * 2G since 64G is not directly accessible for linker
     * a RTEMS bug prevent to use 2G, but 2G-1 does work, see #5591
     */
    ddr                    : ORIGIN = 0x80000000, LENGTH = 2147483647
    flash                  : ORIGIN = 0x08000000, LENGTH = 128M
}

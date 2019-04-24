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

/**
 * Copyright (C) 2013-2014 Kalray SA.
 *
 * All rights reserved.
 */

/* user accessible constants and memory layout */
INCLUDE "memory_map.ld"

DEBUGGER_HAVE_MOS = 1;

INTERNAL_RAM_BASE = DEFINED(INTERNAL_RAM_BASE) ? INTERNAL_RAM_BASE : ORIGIN(internal_mem);
INTERNAL_RAM_SIZE = DEFINED(INTERNAL_RAM_SIZE) ? INTERNAL_RAM_SIZE : LENGTH(internal_mem) + LENGTH(upper_internal_memory);
INTERNAL_RAM_SIZE = ALIGN(INTERNAL_RAM_SIZE,2); /* When upper_internal_memory is not defined ld returns -1 */

S_RAM_BASE = INTERNAL_RAM_BASE;
S_RAM_SIZE = INTERNAL_RAM_SIZE;

/*
 * Page Size
 */
PAGE_SIZE = 4096;

/*
 * ROM
 */
ROM_MEM_SIZE = 4K;
ROM_MEM_END  = INTERNAL_RAM_BASE + INTERNAL_RAM_SIZE ;
ROM_MEM_START = INTERNAL_RAM_BASE + INTERNAL_RAM_SIZE - ROM_MEM_SIZE ;

/*
 * Debug
 */
DEBUG_SIZE = 32;
DEBUG_START = ROM_MEM_END - DEBUG_SIZE;

/*
 * Boot Address
 */
K1_BOOT_ADDRESS = 0x0;

K1_MONITOR_SCOREBOARD = 0;

/*
 * We want a virtual memory entry at mem_base + mem_size of size
 * mem_base + mem_size. On the K1 TLB entries have to begin at a
 * multiple of their size, which has to be a multiple of 2. This
 * little trick rounds INTERNAL_RAM_SIZE up to the nearest power of 2,
 * or is a nop if INTERNAL_RAM_SIZE is already aligned
 */

K1_KERNEL_DATA_ADDR_ALIGN = INTERNAL_RAM_SIZE - 1;
K1_KERNEL_DATA_ADDR_ALIGN |= K1_KERNEL_DATA_ADDR_ALIGN >> 1;
K1_KERNEL_DATA_ADDR_ALIGN |= K1_KERNEL_DATA_ADDR_ALIGN >> 2;
K1_KERNEL_DATA_ADDR_ALIGN |= K1_KERNEL_DATA_ADDR_ALIGN >> 4;
K1_KERNEL_DATA_ADDR_ALIGN |= K1_KERNEL_DATA_ADDR_ALIGN >> 8;
K1_KERNEL_DATA_ADDR_ALIGN |= K1_KERNEL_DATA_ADDR_ALIGN >> 16;
K1_KERNEL_DATA_ADDR_ALIGN += 1;

/*
 * kernel data is @ 256K, but loaded continuously to text in
 * kernel_mem
 */
K1_KERNEL_DATA_ADDR = ALIGN(INTERNAL_RAM_BASE + INTERNAL_RAM_SIZE, K1_KERNEL_DATA_ADDR_ALIGN );

/*
 * Flash
 */
FlashBase = 0x8000000;
FlashSize = 0x8000000;

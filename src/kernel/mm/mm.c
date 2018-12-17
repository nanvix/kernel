/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
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
#include <nanvix/klib.h>
#include <nanvix/mm.h>

/*
 * User stack should be in the user virtual address space.
 */
#if (USTACK_ADDR < UBASE_VIRT) && (USTACK_ADDR > (UBASE_VIRT + UMEM_SIZE))
	#error "user stack outside user address space"
#endif

/*
 * Kernel page pool cannot not overlap with kernel.
 */
#if ((KPOOL_VIRT >= KBASE_VIRT) && (KPOOL_VIRT < (KBASE_VIRT + KMEM_SIZE)))
	#error "kernel page pool overlaps with kernel"
#elif (((KPOOL_VIRT + KPOOL_SIZE) >= KBASE_VIRT) && ((KPOOL_VIRT + KPOOL_SIZE) < (KBASE_VIRT + KMEM_SIZE)))
	#error "kernel page pool overlaps with kernel"
#endif

/*
 * User address space cannot overlap with kernel.
 */
#if ((UBASE_VIRT >= KBASE_VIRT) && (UBASE_VIRT < (KBASE_VIRT + KMEM_SIZE)))
	#error "user address space overlaps with kernel"
#elif (((UBASE_VIRT + UMEM_SIZE) >= KBASE_VIRT) && ((UBASE_VIRT + UMEM_SIZE) < (KBASE_VIRT + KMEM_SIZE)))
	#error "user address space overlaps with kernel"
#endif

/*
 * User address space cannot overlap with kernel page pool.
 */
#if ((UBASE_VIRT >= KPOOL_VIRT) && (UBASE_VIRT < (KPOOL_VIRT + KPOOL_SIZE)))
	#error "user address space overlaps with kernel page pool"
#elif (((UBASE_VIRT + UMEM_SIZE) >= KPOOL_VIRT) && ((UBASE_VIRT + UMEM_SIZE) < (KPOOL_VIRT + KPOOL_SIZE)))
	#error "user address space overlaps with kernel page pool"
#endif

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


/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2017 Clement Rouquier     <clementrouquier@gmail.com>
 *              2018-2018 Davidson Francis     <davidsondfgl@gmail.com>
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

/* Must come first. */
#define _ASM_FILE_

#include <grub/mboot.h>
#include <arch/core/i386/core.h>
#include <arch/core/i386/mmu.h>

/**
 * @brief Multiboot flags.
 */
#define MBOOT_FLAGS (MBOOT_PAGE_ALIGN | MBOOT_MEMORY_INFO)

/* Exported symbols. */
.globl start
.globl idle_pgdir

/*============================================================================*
 *                              bootstrap section                             *
 *============================================================================*/

.section .bootstrap

/*----------------------------------------------------------------------------*
 * multiboot_header                                                           *
 *----------------------------------------------------------------------------*/

/**
 * @brief Multiboot header.
 */
.align 4
mboot_header:
    .long  MBOOT_MAGIC                  /* Magic number.              */
    .long  MBOOT_FLAGS                  /* Flags.                     */
    .long  -(MBOOT_MAGIC + MBOOT_FLAGS) /* Checksum.                  */
    .long  mboot_header                 /* Pointer to this structure. */

/*----------------------------------------------------------------------------*
 * start()                                                                    *
 *----------------------------------------------------------------------------*/

/*
 * Kernel entry point.
 */
start:

	/* Build kernel page tables. */
	start.kernel.init:
		movl $kernel_pgtab, %edi
		movl $0x00000000 + 7, %eax
		start.kernel.init.loop:
			stosl
			addl $I386_PAGE_SIZE, %eax
			cmpl $kernel_pgtab + I386_PAGE_SIZE, %edi
			jl start.kernel.init.loop

	/* Build kernel pool tables. */
	start.kpool.init:
		movl $kpool_pgtab, %edi
		movl $0x01000000 + 7, %eax
		start.kpool.init.loop:
			stosl
			addl $I386_PAGE_SIZE, %eax
			cmpl $kpool_pgtab + I386_PAGE_SIZE, %edi
			jl start.kpool.init.loop

	/*
	 * Build initial page directory.
	 *   - Kernel code + data at 0x00000000
	 *   - Kernel code + data at 0xc0000000
	 *   - Kernel page pool at 0xc1000000
	 */
	start.kpgdir.init:
		movl $kernel_pgtab + 3, idle_pgdir + I386_PTE_SIZE*0
		movl $kernel_pgtab + 3, idle_pgdir + I386_PTE_SIZE*768
		movl $kpool_pgtab + 3,  idle_pgdir + I386_PTE_SIZE*772

	/* Enable paging. */
	start.paging.enable:
		movl $idle_pgdir, %eax
		movl %eax, %cr3
		movl %cr0, %eax
		orl $0x80000000, %eax
		movl %eax, %cr0

	/* Setup stack. */
	start.kstack.setup:
		movl $core0.kstack + I386_PAGE_SIZE - DWORD_SIZE, %ebp
		movl $core0.kstack + I386_PAGE_SIZE - DWORD_SIZE, %esp

	call i386_core_setup

	call kmain

	/* Halt execution. */
	start.halt:
		hlt
		jmp start.halt

/*----------------------------------------------------------------------------*
 * kernel_pgtab                                                               *
 *----------------------------------------------------------------------------*/

/**
 * @brief Page table for kernel code and data.
 */
.align I386_PAGE_SIZE
kernel_pgtab:
	.fill I386_PAGE_SIZE/I386_PTE_SIZE, I386_PTE_SIZE, 0

/*----------------------------------------------------------------------------*
 * kpool_pgtab                                                                *
 *----------------------------------------------------------------------------*/

/**
 * @brief Page table for kernel page pool.
 */
.align I386_PAGE_SIZE
kpool_pgtab:
	.fill I386_PAGE_SIZE/I386_PTE_SIZE, I386_PTE_SIZE, 0

/*----------------------------------------------------------------------------*
 * idle_pgdir                                                                 *
 *----------------------------------------------------------------------------*/

/**
 * @brief Page directory of idle process.
 */
.align I386_PAGE_SIZE
idle_pgdir:
	.fill I386_PAGE_SIZE/I386_PTE_SIZE, I386_PTE_SIZE, 0

/*----------------------------------------------------------------------------*
 * core0.kstack                                                               *
 *----------------------------------------------------------------------------*/

/**
 * @brief Kernel stack for core 0.
 */
.align I386_PAGE_SIZE
core0.kstack:
	.fill I386_PAGE_SIZE/I386_PTE_SIZE, I386_PTE_SIZE, 0

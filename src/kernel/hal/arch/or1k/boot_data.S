/*
 * Copyright(C) 2011-2018 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis <davidsondfgl@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

/* Must come first. */
#define _ASM_FILE_

#include <arch/or1k/core.h>
#include <arch/or1k/mmu.h>

/* Exported symbols. */
.globl kernel_pgtab
.globl kpool_pgtab
.globl idle_pgdir
.globl dtlb_table
.globl itlb_table
.globl core0.kstack

.section .data
/*----------------------------------------------------------------------------*
 * kernel_pgtab                                                               *
 *----------------------------------------------------------------------------*/

/**
 * @brief Page table for kernel code and data.
 */
.align OR1K_PAGE_SIZE
kernel_pgtab:
	.fill OR1K_PAGE_SIZE/OR1K_PTE_SIZE, OR1K_PTE_SIZE, 0

/*----------------------------------------------------------------------------*
 * kpool_pgtab                                                                *
 *----------------------------------------------------------------------------*/

/**
 * @brief Page table for kernel page pool.
 */
.align OR1K_PAGE_SIZE
kpool_pgtab:
	.fill OR1K_PAGE_SIZE/OR1K_PTE_SIZE, OR1K_PTE_SIZE, 0

/*----------------------------------------------------------------------------*
 * idle_pgdir                                                                 *
 *----------------------------------------------------------------------------*/

/**
 * @brief Page directory of idle process.
 */
.align OR1K_PAGE_SIZE
idle_pgdir:
	.fill 256, OR1K_PTE_SIZE, 0

/*----------------------------------------------------------------------------*
 * dtlb                                                                       *
 *----------------------------------------------------------------------------*/

/**
 * @brief dtlb entries
 */
.align OR1K_PAGE_SIZE
dtlb_table:
	.fill 64, DWORD_SIZE, 0

/*----------------------------------------------------------------------------*
 * itlb                                                                       *
 *----------------------------------------------------------------------------*/

/**
 * @brief itlb entries
 */
.align OR1K_PAGE_SIZE
itlb_table:
	.fill 64, DWORD_SIZE, 0

/*----------------------------------------------------------------------------*
 * core0.kstack                                                               *
 *----------------------------------------------------------------------------*/

/**
 * @brief Kernel stack for core 0.
 */
.align OR1K_PAGE_SIZE
core0.kstack:
	.fill OR1K_PAGE_SIZE/OR1K_PTE_SIZE, OR1K_PTE_SIZE, 0

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

/*
 * OpenRISC head.S
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * Modifications for the OpenRISC architecture:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

/* Must come first. */
#define _ASM_FILE_

#include <arch/or1k/core.h>
#include <arch/or1k/mmu.h>

/* Exported symbols. */
.globl start

/**
 * Reset exception.
 */
.org 0x100
l.j start
l.nop

/*============================================================================*
 *                              bootstrap section                             *
 *============================================================================*/

.section .bootstrap

/*----------------------------------------------------------------------------*
 * start()                                                                    *
 *----------------------------------------------------------------------------*/

.org 0x2000
/*
 * Kernel entry point.
 */
start:

	/* Supervisor mode. */
	l.ori	r1, r0, 0x1
	l.mtspr	r0, r1, SPR_SR

	/* Build kernel and kernel pool page tables. */
	LOAD_SYMBOL_2_GPR(r1, kpool_pgtab)
	l.addi  r1, r1,  OR1K_PAGE_SIZE
	l.addi  r1, r1, -DWORD_SIZE

	LOAD_SYMBOL_2_GPR(r3, 0x003ffc00)
	l.ori  r3, r3, PT_L | PT_PRESENT | PT_PPI_SPV_EX

	start.loop1:
		l.sw   0(r1), r3
		l.addi r1, r1, -4
		l.addi r3, r3, -1024
		l.sfgtsi r3, 0
		l.bf start.loop1
		l.nop

	/* Build init page directory. */
	LOAD_SYMBOL_2_GPR(r1, kernel_pgtab)
	l.srli r1, r1, PAGE_SHIFT
	l.slli r1, r1, OR1K_PT_SHIFT
	l.ori  r1, r1, PT_PRESENT

	LOAD_SYMBOL_2_GPR(r3, idle_pgdir)
	
	l.sw  OR1K_PTE_SIZE*0(r3),   r1       /* Kernel code + data at 0x00000000 */
	l.sw  OR1K_PTE_SIZE*192(r3), r1       /* Kernel code + data at 0xc0000000 */

	LOAD_SYMBOL_2_GPR(r1, kpool_pgtab)    /* Kernel page pool at 0xc1000000   */
	l.srli r1, r1, PAGE_SHIFT
	l.slli r1, r1, OR1K_PT_SHIFT
	l.ori  r1, r1, PT_PRESENT
	l.sw  OR1K_PTE_SIZE*193(r3), r1

	/* Flush TLB. */
	l.jal _boot_tlb_flush
	l.nop
	
	/* Fill the IMMU/DMMU with the first NSETS pages. */
	LOAD_SYMBOL_2_GPR(r1, SPR_DTLBTR_BASE(0))
	LOAD_SYMBOL_2_GPR(r3, SPR_ITLBTR_BASE(0))
	LOAD_SYMBOL_2_GPR(r5, SPR_DTLBMR_BASE(0))
	LOAD_SYMBOL_2_GPR(r7, SPR_ITLBMR_BASE(0))
	LOAD_SYMBOL_2_GPR(r9,  dtlb_table)
	LOAD_SYMBOL_2_GPR(r11, itlb_table)

	/* Number of TLB sets, here I'm assuming that ITLB_NSET = DTLB_NSET. */
	l.ori   r15, r0,  1
	l.mfspr r13, r0,  SPR_DMMUCFGR
	l.andi  r13, r13, SPR_DMMUCFGR_NTS
	l.srli  r13, r13, SPR_DMMUCFGR_NTS_OFF
	l.sll   r13, r15, r13
	
	/* DTLBTR, Page 0 with Cache Coherency and Supervisor R/W. */
	l.ori  r15, r0, (SPR_DTLBTR_CC  | SPR_DTLBTR_CI | SPR_DTLBTR_SRE |\
		SPR_DTLBTR_SWE)

	/* ITLBTR, Page 0 with Cache Coherency and Supervisor Execute. */
	l.ori  r17, r0, (SPR_ITLBTR_CC  | SPR_ITLBTR_CI | SPR_ITLBTR_SXE)

	/* xTLBMR, Page 0, Valid. */
	l.ori  r19, r0, 1

	start.filltlb:
		
		/* Data TLB. */
		l.mtspr r1, r15, 0x0
		l.mtspr r5, r19, 0x0
		l.addi  r15, r15, (1 << PAGE_SHIFT)
		
		/* Instruction TLB. */
		l.mtspr r3, r17, 0x0
		l.mtspr r7, r19, 0x0
		l.addi  r17, r17, (1 << PAGE_SHIFT)
		
		l.sw 0(r9),  r19
		l.sw 0(r11), r19
		l.addi r19,  r19, (1 << PAGE_SHIFT)

		l.addi r1, r1, 1
		l.addi r3, r3, 1
		l.addi r5, r5, 1
		l.addi r7, r7, 1

		l.addi r9,  r9,  DWORD_SIZE
		l.addi r11, r11, DWORD_SIZE

		l.sfeq	r13, r0
		l.bnf	start.filltlb
		l.addi	r13, r13, -1
		
	/* Enabling IMMU/DMMU */
	l.jal _enable_mmu
	l.nop

	/* Setup stack. */
	LOAD_SYMBOL_2_GPR(r1, core0.kstack)
	l.addi r1, r1,  OR1K_PAGE_SIZE
	l.addi r1, r1, -DWORD_SIZE  /* Stack pointer. */
	l.or   r2, r1,  r0          /* Frame pointer. */

	/*
	 * TODO:
	 * call or1k_core_setup
	 * call kmain
	 */
	
	/* Halt execution. */
	start.halt:
		l.j start.halt
		l.nop

/*----------------------------------------------------------------------------*
 * _boot_tlb_flush()                                                          *
 *----------------------------------------------------------------------------*/

/*
 * @brief Flushes the TLB.
 */
_boot_tlb_flush:
	LOAD_SYMBOL_2_GPR(r1, SPR_DTLBMR_BASE(0))
	LOAD_SYMBOL_2_GPR(r3, SPR_ITLBMR_BASE(0))
	
	l.ori   r7, r0,  1
	l.mfspr r5, r0, SPR_DMMUCFGR
	l.andi  r5, r5, SPR_DMMUCFGR_NTS
	l.srli  r5, r5, SPR_DMMUCFGR_NTS_OFF
	l.sll   r5, r7, r5
	
	.loop:
		l.mtspr	r1, r0, 0x0
		l.mtspr	r3, r0, 0x0

		l.addi	r1, r1, 1
		l.addi	r3, r3, 1
		l.sfeq	r5, r0
		l.bnf	.loop
		l.addi	r5, r5, -1

	l.jr r9
	l.nop

/*----------------------------------------------------------------------------*
 * _enable_mmu()                                                              *
 *----------------------------------------------------------------------------*/

_enable_mmu:
	l.mfspr	r1, r0, SPR_SR
	LOAD_SYMBOL_2_GPR(r3, SPR_SR_DME | SPR_SR_IME)

	l.or	r1, r1, r3
	l.mtspr	r0, r1, SPR_SR
	
	l.jr r9
	l.nop

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

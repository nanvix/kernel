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
.globl kernel_pgtab
.globl kpool_pgtab
.globl idle_pgdir
.globl dtlb_table
.globl itlb_table
.globl core0.kstack

/*============================================================================*
 *                              bootstrap section                             *
 *============================================================================*/

.section .bootstrap

/*----------------------------------------------------------------------------*
 * start()                                                                    *
 *----------------------------------------------------------------------------*/

/*
 * Kernel entry point.
 */
start:

	/* Supervisor mode. */
	l.ori	r1, r0, 0x1
	l.mtspr	r0, r1, OR1K_SPR_SR

	/* Build kernel and kernel pool page tables. */
	OR1K_LOAD_SYMBOL_2_GPR(r1, kpool_pgtab)
	l.addi  r1, r1,  OR1K_PAGE_SIZE
	l.addi  r1, r1, -DWORD_SIZE

	OR1K_LOAD_SYMBOL_2_GPR(r3, 0x003ffc00)
	l.ori  r3, r3, OR1K_PT_L | OR1K_PT_PRESENT | OR1K_PT_PPI_SPV_EX

	start.loop1:
		l.sw   0(r1), r3
		l.addi r1, r1, -4
		l.addi r3, r3, -1024
		l.sfgtsi r3, 0
		l.bf start.loop1
		l.nop

	/* Build init page directory. */
	OR1K_LOAD_SYMBOL_2_GPR(r1, kernel_pgtab)
	l.srli r1, r1, PAGE_SHIFT
	l.slli r1, r1, OR1K_PT_SHIFT
	l.ori  r1, r1, OR1K_PT_PRESENT

	OR1K_LOAD_SYMBOL_2_GPR(r3, idle_pgdir)

	l.sw  OR1K_PTE_SIZE*0(r3),   r1       /* Kernel code + data at 0x00000000 */
	l.sw  OR1K_PTE_SIZE*192(r3), r1       /* Kernel code + data at 0xc0000000 */

	OR1K_LOAD_SYMBOL_2_GPR(r1, kpool_pgtab)  /* Kernel page pool at 0xc1000000   */
	l.srli r1, r1, PAGE_SHIFT
	l.slli r1, r1, OR1K_PT_SHIFT
	l.ori  r1, r1, OR1K_PT_PRESENT
	l.sw  OR1K_PTE_SIZE*193(r3), r1

	/* Flush TLB. */
	l.jal _boot_tlb_flush
	l.nop

	/* Enabling IMMU/DMMU */
	l.jal _enable_mmu
	l.nop

	/* Setup stack. */
	OR1K_LOAD_SYMBOL_2_GPR(r1, core0.kstack)
	l.addi r1, r1,  OR1K_PAGE_SIZE
	l.addi r1, r1, -DWORD_SIZE  /* Stack pointer. */
	l.or   r2, r1,  r0          /* Frame pointer. */

	/* Setup or1k. */
	OR1K_LOAD_SYMBOL_2_GPR(r3, or1k_core_setup)
	l.jalr r3
	l.nop

	/* kmain. */
	OR1K_LOAD_SYMBOL_2_GPR(r3, kmain)
	l.jalr r3
	l.nop

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
	OR1K_LOAD_SYMBOL_2_GPR(r1, OR1K_SPR_DTLBMR_BASE(0))
	OR1K_LOAD_SYMBOL_2_GPR(r3, OR1K_SPR_ITLBMR_BASE(0))

	l.ori   r7, r0,  1
	l.mfspr r5, r0, OR1K_SPR_DMMUCFGR
	l.andi  r5, r5, OR1K_SPR_DMMUCFGR_NTS
	l.srli  r5, r5, OR1K_SPR_DMMUCFGR_NTS_OFF
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
	l.mfspr	r1, r0, OR1K_SPR_SR
	OR1K_LOAD_SYMBOL_2_GPR(r3, OR1K_SPR_SR_DME | OR1K_SPR_SR_IME)

	l.or	r1, r1, r3
	l.mtspr	r0, r1, OR1K_SPR_SR

	l.jr r9
	l.nop

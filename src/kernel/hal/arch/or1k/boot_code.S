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
#include <target/or1k/pc.h>

/* Exported symbols. */
.globl start
.globl kernel_pgtab
.globl kpool_pgtab
.globl idle_pgdir
.globl dtlb_table
.globl itlb_table
.globl core.kstack
.globl ompic_pgtab
.globl uart_pgtab

/*============================================================================*
 *                              bootstrap section                             *
 *============================================================================*/

.section .bootstrap,"ax",@progbits

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

	/* Check current core. */
	l.mfspr r1, r0, OR1K_SPR_COREID
	l.sfnei r1, 0
	l.bf start.secondary_cores
	l.nop

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
	l.lwz r3, 0(r3)

	l.sw  OR1K_PTE_SIZE*0(r3),   r1       /* Kernel code + data at 0x00000000 */
	l.sw  OR1K_PTE_SIZE*192(r3), r1       /* Kernel code + data at 0xc0000000 */

	OR1K_LOAD_SYMBOL_2_GPR(r1, kpool_pgtab)  /* Kernel page pool at 0xc1000000   */
	l.srli r1, r1, PAGE_SHIFT
	l.slli r1, r1, OR1K_PT_SHIFT
	l.ori  r1, r1, OR1K_PT_PRESENT
	l.sw  OR1K_PTE_SIZE*193(r3), r1

	OR1K_LOAD_SYMBOL_2_GPR(r1, uart_pgtab)   /* UART 8250 at 0xc4000000          */
	l.srli r1, r1, PAGE_SHIFT
	l.slli r1, r1, OR1K_PT_SHIFT
	l.ori  r1, r1, OR1K_PT_PRESENT
	l.sw  OR1K_PTE_SIZE*196(r3), r1

	OR1K_LOAD_SYMBOL_2_GPR(r1, uart_pgtab)
	OR1K_LOAD_SYMBOL_2_GPR(r2, (_OR1K_UART_PHYS >> PAGE_SHIFT) << OR1K_PPN_SHIFT)
	l.ori r2, r2, OR1K_PT_L | OR1K_PT_PRESENT
	l.sw 0(r1), r2

	OR1K_LOAD_SYMBOL_2_GPR(r1, ompic_pgtab)  /* OMPIC at 0xc5000000              */
	l.srli r1, r1, PAGE_SHIFT
	l.slli r1, r1, OR1K_PT_SHIFT
	l.ori  r1, r1, OR1K_PT_PRESENT
	l.sw  PTE_SIZE*197(r3), r1

	OR1K_LOAD_SYMBOL_2_GPR(r1, ompic_pgtab)
	OR1K_LOAD_SYMBOL_2_GPR(r2, (_OR1K_OMPIC_PHYS >> PAGE_SHIFT) << OR1K_PPN_SHIFT)
	l.ori r2, r2, OR1K_PT_L | OR1K_PT_PRESENT
	l.sw 0(r1), r2

	start.secondary_cores:

		/* Setup stack. */
		OR1K_LOAD_SYMBOL_2_GPR(r1, core.kstack)
		l.mfspr r3, r0, OR1K_SPR_COREID
		l.addi  r3, r3, 1
		l.slli  r3, r3, OR1K_PAGE_SHIFT
		l.add   r1, r1, r3
		l.addi  r1, r1, -DWORD_SIZE /* Stack pointer. */
		l.or    r2, r1, r0          /* Frame pointer. */

		/* Flush TLB. */
		l.jal _boot_tlb_flush
		l.nop

		l.mfspr r3, r0, OR1K_SPR_COREID
		l.sfnei r3, 0
		l.bf start.secondary_cores.slave
		l.nop

		/* Setup master. */
		OR1K_LOAD_SYMBOL_2_GPR(r3, or1k_master_setup)
		l.jalr r3
		l.nop

		l.j start.halt
		l.nop

		start.secondary_cores.slave:

			/* Setup slave. */
			OR1K_LOAD_SYMBOL_2_GPR(r3, or1k_slave_setup)
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
	OR1K_LOAD_SYMBOL_2_GPR(r3, OR1K_SPR_DTLBMR_BASE(0))
	OR1K_LOAD_SYMBOL_2_GPR(r5, OR1K_SPR_ITLBMR_BASE(0))

	l.ori   r11, r0,  1
	l.mfspr r7,  r0,  OR1K_SPR_DMMUCFGR
	l.andi  r7,  r7,  OR1K_SPR_DMMUCFGR_NTS
	l.srli  r7,  r7,  OR1K_SPR_DMMUCFGR_NTS_OFF
	l.sll   r7,  r11, r7

	.loop:
		l.mtspr	r3, r0, 0x0
		l.mtspr	r5, r0, 0x0

		l.addi	r3, r3, 1
		l.addi	r5, r5, 1
		l.sfeq	r7, r0
		l.bnf	.loop
		l.addi	r7, r7, -1

	l.jr r9
	l.nop

/*----------------------------------------------------------------------------*
 * _enable_mmu()                                                              *
 *----------------------------------------------------------------------------*/

_enable_mmu:
	l.mfspr	r3, r0, OR1K_SPR_SR
	OR1K_LOAD_SYMBOL_2_GPR(r5, OR1K_SPR_SR_DME | OR1K_SPR_SR_IME)

	l.or	r3, r3, r5
	l.mtspr	r0, r3, OR1K_SPR_SR

	l.jr r9
	l.nop

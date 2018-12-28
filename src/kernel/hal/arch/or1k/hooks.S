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
#include <arch/or1k/regs.h>
#include <arch/or1k/excp.h>

/* Exported symbols. */

/*============================================================================*
 * hardcoded hw exceptions                                                    *
 *============================================================================*/

/*
 * In OpenRISC architecture, all the exceptions are hardcoded to a very
 * specific addresses.
 */

/*
 * Reset
 */
.org 0x100
	l.j start
	l.nop

/*
 * Bus error
 */
.org 0x200
	l.nop
	l.nop

/*
 * Data page fault
 */
.org 0x300
	l.nop
	l.nop

/*
 * Instruction page fault
 */
.org 0x400
	l.nop
	l.nop

/*
 * Tick Timer
 */
.org 0x500
	l.nop
	l.nop

/*
 * Alignment
 */
.org 0x600
	l.nop
	l.nop

/*
 * Illegal Instruction
 */
.org 0x700
	l.nop
	l.nop

/*
 * External interrupt
 */
.org 0x800
	l.nop
	l.nop

/*
 * DTLB-miss
 */
.org 0x900
	l.j boot_dtlb_miss_handler
	l.nop

/*
 * ITLB-miss
 */
.org 0xa00
	l.j boot_itlb_miss_handler
	l.nop

/*
 * Range
 */
.org 0xb00
	l.nop
	l.nop

/*
 * System call
 */
.org 0xc00
	l.nop
	l.nop

/*
 * Floating point
 */
.org 0xd00
	l.nop
	l.nop

/*
 * Trap
 */
.org 0xe00
	l.nop
	l.nop

/*============================================================================*
 * boot_dtlb_miss_handler                                                     *
 *============================================================================*/

/*
 * Initial DTLB handler. This handler is used in the earlier stages of the
 * kernel, while the proper TLB handler was not setup yet.
 */

boot_dtlb_miss_handler:
	OR1K_EXCEPTION_STORE_GPR2
	OR1K_EXCEPTION_STORE_GPR3
	OR1K_EXCEPTION_STORE_GPR4
	OR1K_EXCEPTION_STORE_GPR5
	OR1K_EXCEPTION_STORE_GPR6

	l.mfspr r2, r0, OR1K_SPR_EEAR_BASE     /* Effective address.      */
	OR1K_LOAD_SYMBOL_2_GPR(r3, idle_pgdir) /* Initial page directory. */
	
	/* Page directory entry address. */
	l.srli r4, r2, 24
	l.slli r4, r4, 2
	l.add  r4, r4, r3

	/* Page directory entry. */
	l.lwz  r4, 0(r4)
	l.sfeq r4, r0
	l.bf   d_not_pde
	l.nop

	/* PT address.  */
	l.srli r4, r4, 10
	l.slli r4, r4, PAGE_SHIFT
	
	/* PT offset.   */
	l.srli r5, r2, PAGE_SHIFT
	l.andi r5, r5, 0x7FF
	l.slli r5, r5, 2
	l.add  r4, r4, r5      /* PTE address. */

	/* PTE empty. */
	l.lwz  r4, 0(r4)       /* PTE entry.   */
	l.sfeq r4, r0
	l.bf   d_not_pte
	l.nop

	/* PTE not present. */
	l.andi r6, r4, OR1K_PT_PRESENT
	l.sfeq r6, r0
	l.bf   d_not_pte
	l.nop

	l.srli r5, r4, 10      /* Physical page number. */

	/* DTLB TR Register. */
	l.slli r2, r5, PAGE_SHIFT
	l.andi r4, r4, 0xC0  /* Keep the PPI (2 bits).                    */
	l.ori  r4, r4, 0x305 /* Enable supervidor Read/Write, WBC and CC. */
	l.or   r2, r2, r4

	/* Determine number of DMMU sets and FILL DTLB TR Register. */
	l.mfspr r6, r0, OR1K_SPR_DMMUCFGR
	l.andi	r6, r6, OR1K_SPR_DMMUCFGR_NTS
	l.srli	r6, r6, OR1K_SPR_DMMUCFGR_NTS_OFF
	l.ori	r3, r0, 0x1
	l.sll	r3, r3, r6 	/* r3 = number DMMU sets DMMUCFGR. */
	l.addi	r6, r3, -1  /* r6 = nsets mask.                */
	
	l.mfspr r5, r0, OR1K_SPR_EEAR_BASE /* Tag. */
	l.srli  r5, r5, PAGE_SHIFT

	l.and	r5, r5, r6	/* offset: & (NUM_TLB_ENTRIES-1).  */
	l.mtspr	r5, r2, OR1K_SPR_DTLBTR_BASE(0)

	/* Fill DTLB MR Register. */
	l.mfspr r2, r0, OR1K_SPR_EEAR_BASE
	l.addi	r3, r0, 0xffffe000
	l.and	r3, r2, r3
	l.ori   r3, r3, 1                  /* Valid bit. */
	l.mtspr r5, r3, OR1K_SPR_DTLBMR_BASE(0)

d_not_pde:
d_not_pte:
	OR1K_EXCEPTION_LOAD_GPR2
	OR1K_EXCEPTION_LOAD_GPR3
	OR1K_EXCEPTION_LOAD_GPR4
	OR1K_EXCEPTION_LOAD_GPR5
	OR1K_EXCEPTION_LOAD_GPR6
	l.rfe
	l.nop

/*============================================================================*
 * boot_itlb_miss_handler                                                     *
 *============================================================================*/

/*
 * Initial ITLB handler. This handler is used in the earlier stages of the
 * kernel, while the proper TLB handler was not setup yet.
 */

boot_itlb_miss_handler:
	OR1K_EXCEPTION_STORE_GPR2
	OR1K_EXCEPTION_STORE_GPR3
	OR1K_EXCEPTION_STORE_GPR4
	OR1K_EXCEPTION_STORE_GPR5
	OR1K_EXCEPTION_STORE_GPR6

	l.mfspr r2, r0, OR1K_SPR_EEAR_BASE     /* Effective address.      */
	OR1K_LOAD_SYMBOL_2_GPR(r3, idle_pgdir) /* Initial page directory. */
	
	/* Page directory entry address. */
	l.srli r4, r2, 24
	l.slli r4, r4, 2
	l.add  r4, r4, r3

	/* Page directory entry. */
	l.lwz  r4, 0(r4)
	l.sfeq r4, r0
	l.bf   i_not_pde
	l.nop

	/* PT address.  */
	l.srli r4, r4, 10
	l.slli r4, r4, PAGE_SHIFT
	
	/* PT offset.   */
	l.srli r5, r2, PAGE_SHIFT
	l.andi r5, r5, 0x7FF
	l.slli r5, r5, 2
	l.add  r4, r4, r5      /* PTE address. */

	/* If PTE empty. */
	l.lwz  r4, 0(r4)       /* PTE entry.   */
	l.sfeq r4, r0
	l.bf   i_not_pte
	l.nop

	/* If PTE not present. */
	l.andi r6, r4, OR1K_PT_PRESENT
	l.sfeq r6, r0
	l.bf   i_not_pte
	l.nop

	l.srli r5, r4, 10      /* Physical page number. */

	/* ITLB TR Register. */
	l.slli r2, r5, PAGE_SHIFT
	l.ori  r4, r0, 0xC5  /* Enable UXE, SXE, WBC and CC. */
	l.or   r2, r2, r4

	/* Determine number of IMMU sets and FILL ITLB TR Register. */
	l.mfspr r6, r0, OR1K_SPR_IMMUCFGR
	l.andi	r6, r6, OR1K_SPR_IMMUCFGR_NTS
	l.srli	r6, r6, OR1K_SPR_IMMUCFGR_NTS_OFF
	l.ori	r3, r0, 0x1
	l.sll	r3, r3, r6 	/* r3 = number IMMU sets IMMUCFGR. */
	l.addi	r6, r3, -1  /* r6 = nsets mask.                */
	
	l.mfspr r5, r0, OR1K_SPR_EEAR_BASE /* Tag. */
	l.srli  r5, r5, PAGE_SHIFT

	l.and	r5, r5, r6	/* offset: & (NUM_TLB_ENTRIES-1).  */
	l.mtspr	r5, r2, OR1K_SPR_ITLBTR_BASE(0)

	/* Fill ITLB MR Register. */
	l.mfspr r2, r0, OR1K_SPR_EEAR_BASE
	l.addi	r3, r0, 0xffffe000
	l.and	r3, r2, r3
	l.ori   r3, r3, 1                  /* Valid bit. */
	l.mtspr r5, r3, OR1K_SPR_ITLBMR_BASE(0)

i_not_pde:
i_not_pte:
	OR1K_EXCEPTION_LOAD_GPR2
	OR1K_EXCEPTION_LOAD_GPR3
	OR1K_EXCEPTION_LOAD_GPR4
	OR1K_EXCEPTION_LOAD_GPR5
	OR1K_EXCEPTION_LOAD_GPR6
	l.rfe
	l.nop

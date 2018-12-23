/*
 * MIT License
 *
 * Copyright(c) 2018-2018 Davidson Francis <davidsondfgl@gmail.com>
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

/*
 * OpenRISC Linux
 *
 * SPR Definitions
 *
 * Copyright (C) 2000 Damjan Lampret
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2008, 2010 Embecosm Limited
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 * et al.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This file is part of OpenRISC 1000 Architectural Simulator.
 */

#ifndef ARCH_OR1K_REGS_H_
#define ARCH_OR1K_REGS_H_

	/**
	 * @brief Loads a symbol into a register.
	 */
	#define OR1K_LOAD_SYMBOL_2_GPR(gpr,symbol) \
		l.movhi gpr,hi(symbol)                ;\
		l.ori   gpr,gpr,lo(symbol)

	/**
	 * Definition of special-purpose registers (SPRs).
	 */
	#define OR1K_MAX_GRPS (32)
	#define OR1K_MAX_SPRS_PER_GRP_BITS (11)
	#define OR1K_MAX_SPRS_PER_GRP (1 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_MAX_SPRS (0x10000)

	/**
	 * Base addresses for the groups.
	 */
	#define OR1K_SPRGROUP_SYS   (0 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_DMMU  (1 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_IMMU  (2 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_DC    (3 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_IC    (4 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_MAC   (5 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_D     (6 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_PC    (7 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_PM    (8 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_PIC   (9 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_TT    (10 << OR1K_MAX_SPRS_PER_GRP_BITS)
	#define OR1K_SPRGROUP_FP    (11 << OR1K_MAX_SPRS_PER_GRP_BITS)

	/**
	 * System control and status group.
	 */
	#define OR1K_SPR_VR        (OR1K_SPRGROUP_SYS + 0)
	#define OR1K_SPR_UPR       (OR1K_SPRGROUP_SYS + 1)
	#define OR1K_SPR_CPUCFGR   (OR1K_SPRGROUP_SYS + 2)
	#define OR1K_SPR_DMMUCFGR  (OR1K_SPRGROUP_SYS + 3)
	#define OR1K_SPR_IMMUCFGR  (OR1K_SPRGROUP_SYS + 4)
	#define OR1K_SPR_DCCFGR    (OR1K_SPRGROUP_SYS + 5)
	#define OR1K_SPR_ICCFGR    (OR1K_SPRGROUP_SYS + 6)
	#define OR1K_SPR_DCFGR     (OR1K_SPRGROUP_SYS + 7)
	#define OR1K_SPR_PCCFGR    (OR1K_SPRGROUP_SYS + 8)
	#define OR1K_SPR_VR2       (OR1K_SPRGROUP_SYS + 9)
	#define OR1K_SPR_AVR       (OR1K_SPRGROUP_SYS + 10)
	#define OR1K_SPR_EVBAR     (OR1K_SPRGROUP_SYS + 11)
	#define OR1K_SPR_AECR      (OR1K_SPRGROUP_SYS + 12)
	#define OR1K_SPR_AESR      (OR1K_SPRGROUP_SYS + 13)
	#define OR1K_SPR_NPC       (OR1K_SPRGROUP_SYS + 16)
	#define OR1K_SPR_SR        (OR1K_SPRGROUP_SYS + 17)
	#define OR1K_SPR_PPC       (OR1K_SPRGROUP_SYS + 18)
	#define OR1K_SPR_FPCSR     (OR1K_SPRGROUP_SYS + 20)
	#define OR1K_SPR_EPCR_BASE (OR1K_SPRGROUP_SYS + 32)
	#define OR1K_SPR_EPCR_LAST (OR1K_SPRGROUP_SYS + 47)
	#define OR1K_SPR_EEAR_BASE (OR1K_SPRGROUP_SYS + 48)
	#define OR1K_SPR_EEAR_LAST (OR1K_SPRGROUP_SYS + 63)
	#define OR1K_SPR_ESR_BASE  (OR1K_SPRGROUP_SYS + 64)
	#define OR1K_SPR_ESR_LAST  (OR1K_SPRGROUP_SYS + 79)
	#define OR1K_SPR_COREID    (OR1K_SPRGROUP_SYS + 128)
	#define OR1K_SPR_NUMCORES  (OR1K_SPRGROUP_SYS + 129)
	#define OR1K_SPR_GPR_BASE  (OR1K_SPRGROUP_SYS + 1024)

	/**
	 * Data MMU group
	 */
	#define OR1K_SPR_DMMUCR  (OR1K_SPRGROUP_SYS + 0)
	#define OR1K_SPR_DTLBEIR (OR1K_SPRGROUP_SYS + 2)
	#define OR1K_SPR_DTLBMR_BASE(WAY) (OR1K_SPRGROUP_SYS + 0x200 + (WAY) * 0x100)
	#define OR1K_SPR_DTLBMR_LAST(WAY) (OR1K_SPRGROUP_SYS + 0x27f + (WAY) * 0x100)
	#define OR1K_SPR_DTLBTR_BASE(WAY) (OR1K_SPRGROUP_SYS + 0x280 + (WAY) * 0x100)
	#define OR1K_SPR_DTLBTR_LAST(WAY) (OR1K_SPRGROUP_SYS + 0x2ff + (WAY) * 0x100)

	/**
	 * Instruction MMU group
	 */
	#define OR1K_SPR_IMMUCR  (OR1K_SPRGROUP_IMMU + 0)
	#define OR1K_SPR_ITLBEIR (OR1K_SPRGROUP_IMMU + 2)
	#define OR1K_SPR_ITLBMR_BASE(WAY) (OR1K_SPRGROUP_IMMU + 0x200 + (WAY) * 0x100)
	#define OR1K_SPR_ITLBMR_LAST(WAY) (OR1K_SPRGROUP_IMMU + 0x27f + (WAY) * 0x100)
	#define OR1K_SPR_ITLBTR_BASE(WAY) (OR1K_SPRGROUP_IMMU + 0x280 + (WAY) * 0x100)
	#define OR1K_SPR_ITLBTR_LAST(WAY) (OR1K_SPRGROUP_IMMU + 0x2ff + (WAY) * 0x100)

	/**
	 * Data cache group
	 */
	#define OR1K_SPR_DCCR          (OR1K_SPRGROUP_DC + 0)
	#define OR1K_SPR_DCBPR         (OR1K_SPRGROUP_DC + 1)
	#define OR1K_SPR_DCBFR         (OR1K_SPRGROUP_DC + 2)
	#define OR1K_SPR_DCBIR         (OR1K_SPRGROUP_DC + 3)
	#define OR1K_SPR_DCBWR         (OR1K_SPRGROUP_DC + 4)
	#define OR1K_SPR_DCBLR         (OR1K_SPRGROUP_DC + 5)
	#define OR1K_SPR_DCR_BASE(WAY) (OR1K_SPRGROUP_DC + 0x200 + (WAY) * 0x200)
	#define OR1K_SPR_DCR_LAST(WAY) (OR1K_SPRGROUP_DC + 0x3ff + (WAY) * 0x200)

	/**
	 * Instruction cache group
	 */
	#define OR1K_SPR_ICCR          (OR1K_SPRGROUP_IC + 0)
	#define OR1K_SPR_ICBPR         (OR1K_SPRGROUP_IC + 1)
	#define OR1K_SPR_ICBIR         (OR1K_SPRGROUP_IC + 2)
	#define OR1K_SPR_ICBLR         (OR1K_SPRGROUP_IC + 3)
	#define OR1K_SPR_ICR_BASE(WAY) (OR1K_SPRGROUP_IC + 0x200 + (WAY) * 0x200)
	#define OR1K_SPR_ICR_LAST(WAY) (OR1K_SPRGROUP_IC + 0x3ff + (WAY) * 0x200)

	/**
	 * Power management group
	 */
	#define OR1K_SPR_PMR (OR1K_SPRGROUP_PM + 0)

	/**
	 * PIC group
	 */
	#define OR1K_SPR_PICMR (OR1K_SPRGROUP_PIC + 0)
	#define OR1K_SPR_PICPR (OR1K_SPRGROUP_PIC + 1)
	#define OR1K_SPR_PICSR (OR1K_SPRGROUP_PIC + 2)

	/**
	 * Tick Timer group
	 */
	#define OR1K_SPR_TTMR (OR1K_SPRGROUP_TT + 0)
	#define OR1K_SPR_TTCR (OR1K_SPRGROUP_TT + 1)

	/**
	 * Bit definitions for the Version Register
	 */
	#define OR1K_SPR_VR_VER	  0xff000000  /* Processor version.                 */
	#define OR1K_SPR_VR_CFG	  0x00ff0000  /* Processor configuration.           */
	#define OR1K_SPR_VR_RES	  0x0000ffc0  /* Reserved.                          */
	#define OR1K_SPR_VR_REV	  0x0000003f  /* Processor revision.                */
	#define OR1K_SPR_VR_UVRP  0x00000040  /* Updated Version Registers Present. */

	#define OR1K_SPR_VR_VER_OFF	24
	#define OR1K_SPR_VR_CFG_OFF	16
	#define OR1K_SPR_VR_REV_OFF	0

	/**
	 * Bit definitions for the Version Register 2
	 */
	#define OR1K_SPR_VR2_CPUID 0xff000000  /* Processor ID.      */
	#define OR1K_SPR_VR2_VER   0x00ffffff  /* Processor version. */

	/**
	 * Bit definitions for the Unit Present Register
	 */
	#define OR1K_SPR_UPR_UP    0x00000001  /* UPR present.                  */
	#define OR1K_SPR_UPR_DCP   0x00000002  /* Data cache present.           */
	#define OR1K_SPR_UPR_ICP   0x00000004  /* Instruction cache present.    */
	#define OR1K_SPR_UPR_DMP   0x00000008  /* Data MMU present.             */
	#define OR1K_SPR_UPR_IMP   0x00000010  /* Instruction MMU present.      */
	#define OR1K_SPR_UPR_MP    0x00000020  /* MAC present.                  */
	#define OR1K_SPR_UPR_DUP   0x00000040  /* Debug unit present.           */
	#define OR1K_SPR_UPR_PCUP  0x00000080  /* Performance counters present. */
	#define OR1K_SPR_UPR_PICP  0x00000100  /* PIC present.                  */
	#define OR1K_SPR_UPR_PMP   0x00000200  /* Power management present.     */
	#define OR1K_SPR_UPR_TTP   0x00000400  /* Tick timer present.           */
	#define OR1K_SPR_UPR_RES   0x00fe0000  /* Reserved.                     */
	#define OR1K_SPR_UPR_CUP   0xff000000  /* Context units present.        */

	/**
	 * Bit definitions for the CPU configuration register
	 */
	#define OR1K_SPR_CPUCFGR_NSGF   0x0000000f  /* Number of shadow GPR files. */
	#define OR1K_SPR_CPUCFGR_CGF    0x00000010  /* Custom GPR file.            */
	#define OR1K_SPR_CPUCFGR_OB32S  0x00000020  /* ORBIS32 supported.          */
	#define OR1K_SPR_CPUCFGR_OB64S  0x00000040  /* ORBIS64 supported.          */
	#define OR1K_SPR_CPUCFGR_OF32S  0x00000080  /* ORFPX32 supported.          */
	#define OR1K_SPR_CPUCFGR_OF64S  0x00000100  /* ORFPX64 supported.          */
	#define OR1K_SPR_CPUCFGR_OV64S  0x00000200  /* ORVDX64 supported.          */
	#define OR1K_SPR_CPUCFGR_RES    0xfffffc00  /* Reserved.                   */

	/**
	 * Bit definitions for the Supervision Register
	 */
	#define OR1K_SPR_SR_SM     0x00000001  /* Supervisor Mode.             */
	#define OR1K_SPR_SR_TEE    0x00000002  /* Tick timer Exception Enable. */
	#define OR1K_SPR_SR_IEE    0x00000004  /* Interrupt Exception Enable.  */
	#define OR1K_SPR_SR_DCE    0x00000008  /* Data Cache Enable.           */
	#define OR1K_SPR_SR_ICE    0x00000010  /* Instruction Cache Enable.    */
	#define OR1K_SPR_SR_DME    0x00000020  /* Data MMU Enable.             */
	#define OR1K_SPR_SR_IME    0x00000040  /* Instruction MMU Enable.      */
	#define OR1K_SPR_SR_LEE    0x00000080  /* Little Endian Enable.        */
	#define OR1K_SPR_SR_CE     0x00000100  /* CID Enable.                  */
	#define OR1K_SPR_SR_F      0x00000200  /* Condition Flag.              */
	#define OR1K_SPR_SR_CY     0x00000400  /* Carry flag.                  */
	#define OR1K_SPR_SR_OV     0x00000800  /* Overflow flag.               */
	#define OR1K_SPR_SR_OVE    0x00001000  /* Overflow flag Exception.     */
	#define OR1K_SPR_SR_DSX    0x00002000  /* Delay Slot Exception.        */
	#define OR1K_SPR_SR_EPH    0x00004000  /* Exception Prefix High.       */
	#define OR1K_SPR_SR_FO     0x00008000  /* Fixed one.                   */
	#define OR1K_SPR_SR_SUMRA  0x00010000  /* Supervisor SPR read access.  */
	#define OR1K_SPR_SR_RES    0x0ffe0000  /* Reserved.                    */
	#define OR1K_SPR_SR_CID    0xf0000000  /* Context ID.                  */

	/**
	 * Bit definitions for the Data MMU Control Register
	 */
	#define OR1K_SPR_DMMUCR_P2S          0x0000003e  /* Level 2 Page Size.   */
	#define OR1K_SPR_DMMUCR_P1S          0x000007c0  /* Level 1 Page Size.   */
	#define OR1K_SPR_DMMUCR_VADDR_WIDTH  0x0000f800  /* Virtual ADDR Width.  */
	#define OR1K_SPR_DMMUCR_PADDR_WIDTH  0x000f0000  /* Physical ADDR Width. */

	/**
	 * Bit definitions for the Instruction MMU Control Register
	 */
	#define OR1K_SPR_IMMUCR_P2S          0x0000003e  /* Level 2 Page Size.   */
	#define OR1K_SPR_IMMUCR_P1S          0x000007c0  /* Level 1 Page Size.   */
	#define OR1K_SPR_IMMUCR_VADDR_WIDTH  0x0000f800  /* Virtual ADDR Width.  */
	#define OR1K_SPR_IMMUCR_PADDR_WIDTH  0x000f0000  /* Physical ADDR Width. */

	/**
	 * Bit definitions for the Data TLB Match Register
	 */
	#define OR1K_SPR_DTLBMR_V       0x00000001  /* Valid.                       */
	#define OR1K_SPR_DTLBMR_PL1     0x00000002  /* Page Level 1 (if 0 then PL2).*/
	#define OR1K_SPR_DTLBMR_CID     0x0000003c  /* Context ID.                  */
	#define OR1K_SPR_DTLBMR_LRU     0x000000c0  /* Least Recently Used.         */
	#define OR1K_SPR_DTLBMR_VPN     0xfffff000  /* Virtual Page Number.         */

	/**
	 * Bit definitions for the Data TLB Translate Register
	 */
	#define OR1K_SPR_DTLBTR_CC      0x00000001  /* Cache Coherency.        */
	#define OR1K_SPR_DTLBTR_CI      0x00000002  /* Cache Inhibit.          */
	#define OR1K_SPR_DTLBTR_WBC     0x00000004  /* Write-Back Cache.       */
	#define OR1K_SPR_DTLBTR_WOM     0x00000008  /* Weakly-Ordered Memory.  */
	#define OR1K_SPR_DTLBTR_A       0x00000010  /* Accessed.               */
	#define OR1K_SPR_DTLBTR_D       0x00000020  /* Dirty.                  */
	#define OR1K_SPR_DTLBTR_URE     0x00000040  /* User Read Enable.       */
	#define OR1K_SPR_DTLBTR_UWE     0x00000080  /* User Write Enable.      */
	#define OR1K_SPR_DTLBTR_SRE     0x00000100  /* Supervisor Read Enable. */
	#define OR1K_SPR_DTLBTR_SWE     0x00000200  /* Supervisor Write Enable.*/
	#define OR1K_SPR_DTLBTR_PPN     0xfffff000  /* Physical Page Number.   */

	/**
	 * Bit definitions for the Instruction TLB Match Register
	 */
	#define OR1K_SPR_ITLBMR_V       0x00000001  /* Valid.                      */
	#define OR1K_SPR_ITLBMR_PL1     0x00000002  /* Page Level 1 (if 0 then PL2)*/
	#define OR1K_SPR_ITLBMR_CID     0x0000003c  /* Context ID.                 */
	#define OR1K_SPR_ITLBMR_LRU     0x000000c0  /* Least Recently Used.        */
	#define OR1K_SPR_ITLBMR_VPN     0xfffff000  /* Virtual Page Number.        */

	/**
	 * Bit definitions for the Instruction TLB Translate Register
	 */
	#define OR1K_SPR_ITLBTR_CC      0x00000001  /* Cache Coherency.      */
	#define OR1K_SPR_ITLBTR_CI      0x00000002  /* Cache Inhibit.        */
	#define OR1K_SPR_ITLBTR_WBC     0x00000004  /* Write-Back Cache.     */
	#define OR1K_SPR_ITLBTR_WOM     0x00000008  /* Weakly-Ordered Memory.*/
	#define OR1K_SPR_ITLBTR_A       0x00000010  /* Accessed.             */
	#define OR1K_SPR_ITLBTR_D       0x00000020  /* Dirty.                */
	#define OR1K_SPR_ITLBTR_SXE     0x00000040  /* User Read Enable.     */
	#define OR1K_SPR_ITLBTR_UXE     0x00000080  /* User Write Enable.    */
	#define OR1K_SPR_ITLBTR_PPN     0xfffff000  /* Physical Page Number. */

	/**
	 * Bit definitions for Data Cache Control register
	 */
	#define OR1K_SPR_DCCR_EW  0x000000ff  /* Enable ways */

	/**
	 * Bit definitions for Insn Cache Control register
	 */
	#define OR1K_SPR_ICCR_EW  0x000000ff  /* Enable ways */

	/**
	 * Bit definitions for Data Cache Configuration Register
	 */
	#define OR1K_SPR_DCCFGR_NCW     0x00000007
	#define OR1K_SPR_DCCFGR_NCS     0x00000078
	#define OR1K_SPR_DCCFGR_CBS     0x00000080
	#define OR1K_SPR_DCCFGR_CWS     0x00000100
	#define OR1K_SPR_DCCFGR_CCRI    0x00000200
	#define OR1K_SPR_DCCFGR_CBIRI   0x00000400
	#define OR1K_SPR_DCCFGR_CBPRI   0x00000800
	#define OR1K_SPR_DCCFGR_CBLRI   0x00001000
	#define OR1K_SPR_DCCFGR_CBFRI   0x00002000
	#define OR1K_SPR_DCCFGR_CBWBRI  0x00004000

	#define OR1K_SPR_DCCFGR_NCW_OFF  0
	#define OR1K_SPR_DCCFGR_NCS_OFF  3
	#define OR1K_SPR_DCCFGR_CBS_OFF  7

	/**
	 * Bit definitions for Instruction Cache Configuration Register
	 */
	#define OR1K_SPR_ICCFGR_NCW    0x00000007
	#define OR1K_SPR_ICCFGR_NCS    0x00000078
	#define OR1K_SPR_ICCFGR_CBS    0x00000080
	#define OR1K_SPR_ICCFGR_CCRI   0x00000200
	#define OR1K_SPR_ICCFGR_CBIRI  0x00000400
	#define OR1K_SPR_ICCFGR_CBPRI  0x00000800
	#define OR1K_SPR_ICCFGR_CBLRI  0x00001000

	#define OR1K_SPR_ICCFGR_NCW_OFF 0
	#define OR1K_SPR_ICCFGR_NCS_OFF 3
	#define OR1K_SPR_ICCFGR_CBS_OFF 7

	/**
	 * Bit definitions for Data MMU Configuration Register
	 */
	#define OR1K_SPR_DMMUCFGR_NTW     0x00000003
	#define OR1K_SPR_DMMUCFGR_NTS     0x0000001C
	#define OR1K_SPR_DMMUCFGR_NAE     0x000000E0
	#define OR1K_SPR_DMMUCFGR_CRI     0x00000100
	#define OR1K_SPR_DMMUCFGR_PRI     0x00000200
	#define OR1K_SPR_DMMUCFGR_TEIRI   0x00000400
	#define OR1K_SPR_DMMUCFGR_HTR     0x00000800
	#define OR1K_SPR_DMMUCFGR_NTW_OFF 0
	#define OR1K_SPR_DMMUCFGR_NTS_OFF 2

	/**
	 * Bit definitions for Instruction MMU Configuration Register
	 */
	#define OR1K_SPR_IMMUCFGR_NTW   0x00000003
	#define OR1K_SPR_IMMUCFGR_NTS   0x0000001C
	#define OR1K_SPR_IMMUCFGR_NAE   0x000000E0
	#define OR1K_SPR_IMMUCFGR_CRI   0x00000100
	#define OR1K_SPR_IMMUCFGR_PRI   0x00000200
	#define OR1K_SPR_IMMUCFGR_TEIRI 0x00000400
	#define OR1K_SPR_IMMUCFGR_HTR   0x00000800
	#define OR1K_SPR_IMMUCFGR_NTW_OFF 0
	#define OR1K_SPR_IMMUCFGR_NTS_OFF 2

	/**
	 * Bit definitions for the Power management register
	 */
	#define OR1K_SPR_PMR_DME	0x00000010  /* Doze mode enable */

	/**
	 * Bit definitions for Tick Timer Control Register
	 */
	#define OR1K_SPR_TTCR_CNT 0xffffffff  /* Count, time period. */
	#define OR1K_SPR_TTMR_TP  0x0fffffff  /* Time period.        */
	#define OR1K_SPR_TTMR_IP  0x10000000  /* Interrupt Pending   */
	#define OR1K_SPR_TTMR_IE  0x20000000  /* Interrupt Enable.   */
	#define OR1K_SPR_TTMR_DI  0x00000000  /* Disabled.           */
	#define OR1K_SPR_TTMR_RT  0x40000000  /* Restart tick.       */
	#define OR1K_SPR_TTMR_SR  0x80000000  /* Single run.         */
	#define OR1K_SPR_TTMR_CR  0xc0000000  /* Continuous run.     */
	#define OR1K_SPR_TTMR_M   0xc0000000  /* Tick mode.          */

#endif	/* ARCH_OR1K_REGS_H_ */

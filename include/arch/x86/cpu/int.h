/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_INT_H_
#define ARCH_X86_CPU_INT_H_

/**
 * @addtogroup x86-cpu-int x86 Interrupts
 * @ingroup x86
 *
 * @brief x86 Interrupts
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <arch/x86/cpu/lpic.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of interrupts.
 */
#define INTERRUPTS_NUM LPIC_NUM_IRQS

/**
 * @name Hardware Interrupts for the IBM PC Target
 */
/**@{*/
#define INTERRUPT_TIMER 0    /** Programmable interrupt timer.              */
#define INTERRUPT_KEYBOARD 1 /** Keyboard.                                  */
#define INTERRUPT_COM2 3     /** COM2.                                      */
#define INTERRUPT_COM1 4     /** COM1.                                      */
#define INTERRUPT_LPT2 5     /** LPT2.                                      */
#define INTERRUPT_FLOPPY 6   /** Floppy disk.                               */
#define INTERRUPT_LPT1 7     /** LPT1.                                      */
#define INTERRUPT_CMOS 8     /** CMOS real-time timer.                      */
#define INTERRUPT_SCSI1 9    /** Free for peripherals (legacy SCSI or NIC). */
#define INTERRUPT_SCSI2 10   /** Free for peripherals (legacy SCSI or NIC). */
#define INTERRUPT_SCSI3 11   /** Free for peripherals (legacy SCSI or NIC). */
#define INTERRUPT_MOUSE 12   /** PS2 mouse.                                 */
#define INTERRUPT_COPROC 13  /** FPU, coprocessor or inter-processor.       */
#define INTERRUPT_ATA1 14    /** Primary ATA hard disk.                     */
#define INTERRUPT_ATA2 15    /** Secondary ATA hard disk.                   */
/**@}*/

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_INT_H_ */

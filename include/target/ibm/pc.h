/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef TARGET_IBM_PC_H_
#define TARGET_IBM_PC_H_

/**
 * @defgroup i386-pc IBM PC
 * @ingroup targets
 */
/**@{*/

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_stdout_init
	#define __hal_stdout_write
	/**@}*/

	#include <arch/i386/i386.h>

	#include <driver/console.h>

/*============================================================================*
 * Interrupt and Exception Interfaces                                         *
 *============================================================================*/

	/**
	 * @name Hardware Interrupts for the IBM PC Target
	 */
	/**@{*/
	#define I386_PC_INT_CLOCK    0 /*< Programmable interrupt timer.              */
	#define I386_PC_INT_KEYBOARD 1 /*< Keyboard.                                  */
	#define I386_PC_INT_COM2     3 /*< COM2.                                      */
	#define I386_PC_INT_COM1     4 /*< COM1.                                      */
	#define I386_PC_INT_LPT2     5 /*< LPT2.                                      */
	#define I386_PC_INT_FLOPPY   6 /*< Floppy disk.                               */
	#define I386_PC_INT_LPT1     7 /*< LPT1.                                      */
	#define I386_PC_INT_CMOS     8 /*< CMOS real-time clock.                      */
	#define I386_PC_INT_SCSI1    9 /*< Free for peripherals (legacy SCSI or NIC). */
	#define I386_PC_INT_SCSI2   10 /*< Free for peripherals (legacy SCSI or NIC). */
	#define I386_PC_INT_SCSI3   11 /*< Free for peripherals (legacy SCSI or NIC). */
	#define I386_PC_INT_MOUSE   12 /*< PS2 mouse.                                 */
	#define I386_PC_INT_COPROC  13 /*< FPU, coprocessor or inter-processor.       */
	#define I386_PC_INT_ATA1    14 /*< Primary ATA hard disk.                     */
	#define I386_PC_INT_ATA2    15 /*< Secondary ATA hard disk.                   */
	/**@}*/

	/**
	 * @brief Number of hardware interrupts in the IBM PC target.
	 */
	#define _HAL_INT_NR I386_NUM_HWINT

	/**
	 * @brief Number of exceptions in the IBM PC target.
	 */
	#define _HAL_NUM_EXCEPTION I386_NUM_EXCEPTION

/*============================================================================*
 * Memory Interface                                                           *
 *============================================================================*/

	/**
	 * @brief Kernel stack size (in bytes).
	 */
	#define _KSTACK_SIZE I386_PAGE_SIZE

	/**
	 * @brief Memory size (in bytes).
	 */
	#define _HAL_MEM_SIZE 0x01000000

	/**
	 * @name Virtual Memory Layout
	 */
	/**@{*/
	#define _UBASE_VIRT  0x02000000 /**< User base.        */
	#define _USTACK_ADDR 0xc0000000 /**< User stack.       */
	#define _KBASE_VIRT  0xc0000000 /**< Kernel base.      */
	#define _KPOOL_VIRT  0xc1000000 /**< Kernel page pool. */
	#define _INITRD_VIRT 0xc2000000 /**< Initial RAM disk. */
	/**@}*/

	/**
	 * @name Physical Memory Layout
	 */
	/**@{*/
	#define _KBASE_PHYS 0x00000000 /**< Kernel base.      */
	#define _KPOOL_PHYS 0x01000000 /**< Kernel page pool. */
	#define _UBASE_PHYS 0x02000000 /**< User base.        */
	/**@}*/

	/**
	 * @brief User memory size (in bytes).
	 */
	#define _UMEM_SIZE (16*1024*1024)

	/**
	 * @brief Kernel page pool size (in bytes).
	 */
	#define _KPOOL_SIZE 0x400000

/*============================================================================*
 * Clock Interface                                                            *
 *============================================================================*/

	/**
	 * @name Hardware Interrupts
	 */
	/**@{*/
	#define HAL_INT_CLOCK I386_PC_INT_CLOCK /*< Programmable interrupt timer. */
	/**@}*/

/*============================================================================*
 * Processor Interface                                                        *
 *============================================================================*/

	/**
	 * @brief Number of cores in a a CPU in the the IBM PC target.
	 */
	#define _HAL_NUM_CORES I386_NUM_CORES

/*============================================================================*
 * Debug Interface                                                            *
 *============================================================================*/

	/**
	 * @see console_init()
	 */
	static inline void hal_stdout_init(void)
	{
		console_init();
	}

	/**
	 * @see console_write()
	 */
	static inline void hal_stdout_write(const char *buf, size_t n)
	{
		console_write(buf, n);
	}

/**@}*/

#endif /* TARGET_BIM_PC_H_ */

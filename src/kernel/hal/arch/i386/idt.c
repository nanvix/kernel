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

#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <arch/i386/8259.h>
#include <arch/i386/excp.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/int.h>

/**
 * @brief Interrupt Descriptor Table (IDT).
 */
PRIVATE struct idte idt[IDT_SIZE];

/**
 * @brief Pointer to Interrupt Descriptor Table (IDTPTR)
 */
PRIVATE struct idtptr idtptr;

/**
 * @brief Sets a IDT entry.
 *
 * @param n        Target entry.
 * @param handler  Handler.
 * @param selector Segment selctor.
 * @param flags    Handler flags.
 * @param type     Interrupt type.
 */
PRIVATE void set_idte
(int n, unsigned handler, unsigned selector, unsigned flags, unsigned type)
{
	/* Set handler. */
	idt[n].handler_low = (handler & 0xffff);
	idt[n].handler_high = (handler >> 16) & 0xffff;

	/* Set GDT selector. */
	idt[n].selector = selector;

   /* Set type and flags. */
	idt[n].type = type;
	idt[n].flags = flags;
}

/**
 * @brief Sets up the IDT.
 */
PUBLIC void idt_setup(void)
{
	/* Size-error checking. */
	KASSERT_SIZE(sizeof(struct idte), IDTE_SIZE);
	KASSERT_SIZE(sizeof(struct idtptr), IDTPTR_SIZE);

	/* Blank IDT and IDT pointer. */
	kmemset(idt, 0, sizeof(idt));
	kmemset(&idtptr, 0, IDTPTR_SIZE);

	/* Re-initialize PIC. */
	i386_pic_setup(0x20, 0x28);

	/* Set software interrupts (exceptions). */
	set_idte(0, (unsigned)_do_excp0, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(1, (unsigned)_do_excp1, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(2, (unsigned)_do_excp2, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(3, (unsigned)_do_excp3, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(4, (unsigned)_do_excp4, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(5, (unsigned)_do_excp5, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(6, (unsigned)_do_excp6, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(7, (unsigned)_do_excp7, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(8, (unsigned)_do_excp8, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(9, (unsigned)_do_excp9, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(10, (unsigned)_do_excp10, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(11, (unsigned)_do_excp11, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(12, (unsigned)_do_excp12, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(13, (unsigned)_do_excp13, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(14, (unsigned)_do_excp14, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(15, (unsigned)_do_excp15, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(16, (unsigned)_do_excp16, KERNEL_CS, 0x8, IDT_INT32);
	for (int i = 21; i < 30; i++)
		set_idte(i, (unsigned)_do_excp15, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(30, (unsigned)_do_excp30, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(31, (unsigned)_do_excp15, KERNEL_CS, 0x8, IDT_INT32);

	/* Set hardware interrupts. */
	set_idte(32, (unsigned)_do_hwint0, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(33, (unsigned)_do_hwint1, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(34, (unsigned)_do_hwint2, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(35, (unsigned)_do_hwint3, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(36, (unsigned)_do_hwint4, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(37, (unsigned)_do_hwint5, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(38, (unsigned)_do_hwint6, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(39, (unsigned)_do_hwint7, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(40, (unsigned)_do_hwint8, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(41, (unsigned)_do_hwint9, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(42, (unsigned)_do_hwint10, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(43, (unsigned)_do_hwint11, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(44, (unsigned)_do_hwint12, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(45, (unsigned)_do_hwint13, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(46, (unsigned)_do_hwint14, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(47, (unsigned)_do_hwint15, KERNEL_CS, 0x8, IDT_INT32);

	/* Set system call interrupt. */
	set_idte(128, (unsigned)syscall, KERNEL_CS, 0xe, IDT_INT32);

	/* Set IDT pointer. */
	idtptr.size = sizeof(idt) - 1;
	idtptr.ptr = (unsigned)&idt;

	/* Flush IDT. */
	idt_flush(&idtptr);
}

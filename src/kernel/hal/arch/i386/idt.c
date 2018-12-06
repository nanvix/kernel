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
	pic_setup(0x20, 0x28);
	
	/* Set software interrupts (exceptions). */
	set_idte(0, (unsigned)swint0, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(1, (unsigned)swint1, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(2, (unsigned)swint2, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(3, (unsigned)swint3, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(4, (unsigned)swint4, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(5, (unsigned)swint5, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(6, (unsigned)swint6, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(7, (unsigned)swint7, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(8, (unsigned)swint8, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(9, (unsigned)swint9, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(10, (unsigned)swint10, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(11, (unsigned)swint11, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(12, (unsigned)swint12, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(13, (unsigned)swint13, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(14, (unsigned)swint14, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(15, (unsigned)swint15, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(16, (unsigned)swint16, KERNEL_CS, 0x8, IDT_INT32);
	for (int i = 17; i < 32; i++)
		set_idte(i, (unsigned)swint17, KERNEL_CS, 0x8, IDT_INT32);
	 
	/* Set hardware interrupts. */
	set_idte(32, (unsigned)hwint0, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(33, (unsigned)hwint1, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(34, (unsigned)hwint2, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(35, (unsigned)hwint3, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(36, (unsigned)hwint4, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(37, (unsigned)hwint5, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(38, (unsigned)hwint6, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(39, (unsigned)hwint7, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(40, (unsigned)hwint8, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(41, (unsigned)hwint9, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(42, (unsigned)hwint10, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(43, (unsigned)hwint11, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(44, (unsigned)hwint12, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(45, (unsigned)hwint13, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(46, (unsigned)hwint14, KERNEL_CS, 0x8, IDT_INT32);
	set_idte(47, (unsigned)hwint15, KERNEL_CS, 0x8, IDT_INT32);
	 
	/* Set system call interrupt. */
	set_idte(128, (unsigned)syscall, KERNEL_CS, 0xe, IDT_INT32);
  
	/* Set IDT pointer. */
	idtptr.size = sizeof(idt) - 1;
	idtptr.ptr = (unsigned)&idt;
	 
	/* Flush IDT. */
	idt_flush(&idtptr);
}

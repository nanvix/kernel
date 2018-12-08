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
#include <arch/i386/8259.h>
#include <arch/i386/io.h>
#include <stdint.h>

/*============================================================================*
 *                               pic_mask()                                   *
 *============================================================================*/
 
/**
 * The pic_mask() function sets the interrupt mask to @p mask, thus
 * preventing related interrupt requested to be fired.
 */
PUBLIC void pic_mask(uint16_t mask)
{
	i386_outb(PIC_DATA_MASTER, mask & 0xff);
	i386_outb(PIC_DATA_SLAVE, mask >> 8);
}

/*============================================================================*
 *                               pic_setup()                                  *
 *============================================================================*/

/**
 * The pic_setup() function setups the PIC by effectively remapping
 * interrupt vectors. This is mandatory when operating in protected
 * mode, since the default hardware interrupt vectors conflicts with
 * CPU exception vectors.
 */
PUBLIC void pic_setup(uint8_t offset1, uint8_t offset2)
{
	/*
	 * Starts initialization sequence
	 * in cascade mode.
	 */
	i386_outb(PIC_CTRL_MASTER, 0x11);
	i386_iowait();
	i386_outb(PIC_CTRL_SLAVE, 0x11);
	i386_iowait();
	
	/* Send new vector offset. */
	i386_outb(PIC_DATA_MASTER, offset1);
	i386_iowait();
	i386_outb(PIC_DATA_SLAVE, offset2);
	i386_iowait();
	
	/*
	 * Tell the master that there is a slave
	 * PIC hired up at IRQ line 2 and tell
	 * the slave PIC that it is the second PIC. 
	 */
	i386_outb(PIC_DATA_MASTER, 0x04);
	i386_iowait();
	i386_outb(PIC_DATA_SLAVE, 0x02);
	i386_iowait();
	
	/* Set 8086 mode. */
	i386_outb(PIC_DATA_MASTER, 0x01);
	i386_iowait();
	i386_outb(PIC_DATA_SLAVE, 0x01);
	i386_iowait();
	
	/* Clears interrupt mask. */
	pic_mask(0x0000);
}

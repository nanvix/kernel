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
#include <arch/core/i386/8259.h>
#include <arch/core/i386/pmio.h>
#include <stdint.h>

/**
 * @brief Masks of interrupt levels.
 *
 * Lookup table for masks of interrupt levels.
 */
PRIVATE uint16_t intlvl_masks[I386_NUM_INTLVL] = {
	I386_INTLVL_MASK_0,
	I386_INTLVL_MASK_1,
	I386_INTLVL_MASK_2,
	I386_INTLVL_MASK_3,
	I386_INTLVL_MASK_4,
	I386_INTLVL_MASK_5,
};

/**
 * @brief Current interrupt level.
 *
 * Current interrupt level of the underlying i386 core.
 */
PRIVATE int currlevel = I386_INTLVL_5;

/**
 * @brief Current interrupt mask.
 *
 * Current interrupt mask of the underlying i386 core.
 */
PRIVATE uint16_t currmask = I386_INTLVL_MASK_5;

/*============================================================================*
 * i386_pic_mask()                                                            *
 *============================================================================*/
 
/**
 * The i386_pic_mask() function masks the interrupt request line in
 * which the interrupt @p intnum is hooked up.
 */
PUBLIC void i386_pic_mask(int intnum)
{
	uint16_t port;
	uint8_t value;
	uint16_t newmask;

	if (intnum < 8)
	{
		port = PIC_DATA_MASTER;
		newmask = currmask | (1 << intnum);
		value = newmask & 0xff;
	}
	else
	{
		port = PIC_DATA_SLAVE;
		newmask = currmask | (1 << intnum);
		value = (newmask >> 8) & 0xff;
	}

	currmask = newmask;

	i386_output8(port, value);
}

/*============================================================================*
 * i386_pic_unmask()                                                          *
 *============================================================================*/

/**
 * The i386_pic_unmask() function unmasks the interrupt request line
 * in which the interrupt @p intnum is hooked up.
 */
PUBLIC void i386_pic_unmask(int intnum)
{
	uint16_t port;
	uint8_t value;
	uint16_t newmask;

	if (intnum < 8)
	{
		port = PIC_DATA_MASTER;
		newmask = currmask & ~(1 << intnum);
		value = newmask & 0xff;
	}
	else
	{
		port = PIC_DATA_SLAVE;
		newmask = currmask & ~(1 << intnum);
		value = (newmask >> 8) & 0xff;
	}

	currmask = newmask;

	i386_output8(port, value);
}

/*============================================================================*
 * i386_pic_lvl_set()                                                         *
 *============================================================================*/

/**
 * The i386_pic_set() function sets the interrupt level of the calling
 * core to @p newlevel. The old interrupt level is returned.
 */
PUBLIC int i386_pic_lvl_set(int newlevel)
{
	int oldlevel;
	uint16_t mask;

	mask = intlvl_masks[newlevel];

	i386_output8(PIC_DATA_MASTER, mask & 0xff);
	i386_output8(PIC_DATA_SLAVE, mask >> 8);

	currmask = mask;
	oldlevel = currlevel;
	currlevel = newlevel;

	return (oldlevel);
}

/*============================================================================*
 * i386_pic_setup()                                                           *
 *============================================================================*/

/**
 *
 * The i386_pic_setup() function initializes the programmble interrupt
 * controler of the i386 core. Upon completion, it drops the interrupt
 * level to the slowest ones, so that all interrupt lines are enabled.
 */
PUBLIC void i386_pic_setup(uint8_t offset1, uint8_t offset2)
{
	/*
	 * Starts initialization sequence
	 * in cascade mode.
	 */
	i386_output8(PIC_CTRL_MASTER, 0x11);
	i386_iowait();
	i386_output8(PIC_CTRL_SLAVE, 0x11);
	i386_iowait();
	
	/* Send new vector offset. */
	i386_output8(PIC_DATA_MASTER, offset1);
	i386_iowait();
	i386_output8(PIC_DATA_SLAVE, offset2);
	i386_iowait();
	
	/*
	 * Tell the master that there is a slave
	 * PIC hired up at IRQ line 2 and tell
	 * the slave PIC that it is the second PIC. 
	 */
	i386_output8(PIC_DATA_MASTER, 0x04);
	i386_iowait();
	i386_output8(PIC_DATA_SLAVE, 0x02);
	i386_iowait();
	
	/* Set 8086 mode. */
	i386_output8(PIC_DATA_MASTER, 0x01);
	i386_iowait();
	i386_output8(PIC_DATA_SLAVE, 0x01);
	i386_iowait();
	
	/* Clears interrupt mask. */
	i386_pic_lvl_set(I386_INTLVL_0);
}

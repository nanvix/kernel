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

/**
 * @addtogroup i386-8259 8259 Chip
 * @ingroup i386
 *
 * @brief Programmable Interrupt Controller
 */
/**@{*/

#ifndef ARCH_I386_8259_H_
#define ARCH_I386_8259_H_

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_intlvl_drop
	#define __hal_intlvl_raise
	/**@}*/

	/**
	 * @name Master PIC Registers
	 */
	/**@{*/
	#define PIC_CTRL_MASTER 0x20 /**< Control register. */
	#define PIC_DATA_MASTER 0x21 /**< Data register.    */
	/**@}*/
	
	/**
	 * @name Slave PIC Registers
	 */
	/**@{*/
	#define PIC_CTRL_SLAVE 0xa0 /**< Control register. */
	#define PIC_DATA_SLAVE 0xa1 /**< Data register.    */
	/**@}*/

#ifndef _ASM_FILE_

	#include <nanvix/const.h>
	#include <stdint.h>

	/**
	 * @brief Sets interrupt mask.
	 * 
	 * @param mask Interrupt mask to be set.
	 */
	EXTERN void pic_mask(uint16_t mask);
	
	/**
	 * @brief Setups the programmable interrupt controller
	 * 
	 * @param offset1 Vector offset for master PIC.
	 * @param offset2 Vector offset for slave PIC.
	 */
	EXTERN void pic_setup(uint8_t offset1, uint8_t offset2);

#endif /* _ASM_FILE_ */

/**@}*/

#endif /* ARCH_I386_8259_H_ */


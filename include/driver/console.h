/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef DRIVER_CONSOLE_H_
#define DRIVER_CONSOLE_H_

/**
 * @addtogroup dev-console Console
 * @ingroup dev
 */
/**@{*/

	#include <stddef.h>

	/* Console colors. */
	#define BLACK         0x00
	#define BLUE          0x01
	#define GREEN         0x02
	#define CYAN          0x03
	#define RED           0x04
	#define MAGNETA       0x05
	#define BROWN         0x06
	#define LIGHT_GREY    0x07
	#define DARK_GREY     0x08
	#define LIGHT_BLUE    0x09
	#define LIGHT_GREEN   0x0a
	#define LIGHT_CYAN    0x0b
	#define LIGHT_RED     0x0c
	#define LIGHT_MAGNETA 0x0d
	#define LIGHT_BROWN   0x0e
	#define WHITE         0x0f
	
	/**
	 * @name Video Specifications (Text Mode)
	 */
	/**@{*/
	#define VIDEO_ADDR  0xb8000 /**< Video memory address. */
	#define VIDEO_WIDTH      80 /**< Video width.          */
	#define VIDEO_HIGH       25 /**< Video high.           */
	/**@}*/
	
	/**
	 * @name Video Registers
	 */
	/**@{*/
	#define VIDEO_CRTL_REG 0x3d4 /**< Video control register. */
	#define VIDEO_DATA_REG 0x3d5 /**< Video data register.    */
	/**@}*/
		
	/**
	 * @name Offset to Video Control Registers
	 */
	/**@{*/
	#define VIDEO_HTOT 0x00 /**< Horizontal total.               */
	#define VIDEO_HDEE 0x01 /**< Horizontal display enabled end. */
	#define VIDEO_SHB  0x02 /**< Start horizontal blanking.      */
	#define VIDEO_EHB  0x03 /**< End Horizontal blanking.        */
	#define VIDEO_SHRP 0x04 /**< Start horizontal retrace pulse. */
	#define VIDEO_EHRP 0x05 /**< End horizontal retrace pulse.   */
	#define VIDEO_VTR  0x06 /**< Vertical total.                 */
	#define VIDEO_OVRF 0x07 /**< Overflow.                       */
	#define VIDEO_PRS  0x08 /**< Preset row scan.                */
	#define VIDEO_MSL  0x09 /**< Maximum scan line.              */
	#define VIDEO_CS   0x0a /**< Cursor start.                   */
	#define VIDEO_CE   0x0b /**< Cursor end.                     */
	#define VIDEO_SAH  0x0c /**< Start address high.             */
	#define VIDEO_SAL  0x0d /**< Start address low.              */
	#define VIDEO_CLH  0x0e /**< Cursor location high.           */
	#define VIDEO_CLL  0x0f /**< Cursor location low.            */
	#define VIDEO_RSR  0x10 /**< Vertical retrace start.         */
	#define VIDEO_RSE  0x11 /**< Vertical retrace end.           */
	#define VIDEO_VDEE 0x12 /**< Vertical display-enable end.    */
	#define VIDEO_OFF  0x13 /**< Offset.                         */
	#define VIDEO_ULOC 0x14 /**< Underline location.             */
	#define VIDEO_SVB  0x15 /**< Start vertical blanking.        */
	#define VIDEO_EVB  0x16 /**< End vertical blanking.          */
	#define VIDEO_CMC  0x17 /**< CRT mode control.               */
	#define VIDEO_LCMP 0x18 /**< Line compare.                   */
	/**@}*/

	/**
	 * @brief Initializes the console driver.
	 */
	extern void console_init(void);

	/**
	 * @brief Writes a buffer on the console device.
	 *
	 * @param buf Target buffer.
	 * @param n   Number of bytes to write.
	 */
	extern void console_write(const char *buf, size_t n);

/**@}*/

#endif /* DRIVER_CONSOLE_H_ */

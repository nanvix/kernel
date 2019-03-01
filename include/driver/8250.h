/*
 * MIT License
 *
 * Copyright(c) 2018 Davidson Francis <davidsondfgl@gmail.com>
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

#ifndef DRIVER_8250_H_
#define DRIVER_8250_H_

/**
 * @addtogroup dev-8250 8250
 * @ingroup dev
 */
/**@{*/

	#include <stddef.h>
	#include <stdint.h>

	/**
	 * @brief UART definitions
	 */
	/**@{*/
	#define UART_CLOCK_SIGNAL 50000000
	#define UART_ADDR         _OR1K_UART_VIRT
	#define UART_BAUD         115200
	#define UART_IRQ          2
	/**@}*/

	/**
	 * @brief Register interface
	 */
	/**@{*/
	#define RB  0 /**< Receiver Buffer (R).                   */
	#define THR 0 /**< Transmitter Holding Register (W).      */
	#define IER 1 /**< Interrupt Enable Register (RW).        */
	#define IIR 2 /**< Interrupt Identification Register (R). */
	#define FCR 2 /**< FIFO Control Register (W).             */
	#define LCR 3 /**< Line Control Register (RW).            */
	#define MCR 4 /**< Modem Control Register (W).            */
	#define LSR 5 /**< Line Status Register (R).              */
	#define MSR 6 /**< Modem Status Register (R).             */
	/**@}*/

	/**
	 * Divisor Register (Accessed when DLAB bit in LCR is set)
	 */
	/**@{*/
	#define DLB1 0 /**< Divisor Latch LSB (RW). */
	#define DLB2 1 /**< Divisor Latch MSB (RW). */

	/**
	 * Interrupt Enable Register bits.
	 */
	/**@{*/
	#define IER_RDAI 0 /**< Receiver Data Available Interrupt.            */
	#define IER_TEI  1 /**< Transmitter Holding Register Empty Interrupt. */
	#define IER_RLSI 2 /**< Receiver Line Status Interrupt.               */
	#define IER_MSI  3 /**< Modem Status Interrupt.                       */
	/**@}*/

	/**
	 * Interrupt Identification Register Values.
	 */
	/**@{*/
	#define IIR_RLS  0xC6 /**< Receiver Line Status.               */
	#define IIR_RDA  0xC4 /**< Receiver Data Available.            */
	#define IIR_TO   0xCC /**< Timeout.                            */
	#define IIR_THRE 0xC2 /**< Transmitter Holding Register Empty. */
	#define IIT_MS   0xC0 /**< Modem Status.                       */
	/**@}*/

	/**
	 * FIFO Control Register bits.
	 */
	/**@{*/
	#define FCR_CLRRECV 0x1  /**< Clear receiver FIFO.    */
	#define FCR_CLRTMIT 0x2  /**< Clear transmitter FIFO. */
	/**@}*/

	/**
	 * FIFO Control Register bit 7-6 values.
	 */
	/**@{*/
	#define FCR_TRIG_1  0x0  /**< Trigger level 1 byte.   */
	#define FCR_TRIG_4  0x40 /**< Trigger level 4 bytes.  */
	#define FCR_TRIG_8  0x80 /**< Trigger level 8 bytes.  */
	#define FCR_TRIG_14 0xC0 /**< Trigger level 14 bytes. */
	/**@}*/

	/**
	 * Line Control Reigster values and bits.
	 */
	/**@{*/
	#define LCR_BPC_5 0x0  /**< 5 bits per character.                            */
	#define LCR_BPC_6 0x1  /**< 6 bits per character.                            */
	#define LCR_BPC_7 0x2  /**< 7 bits per character.                            */
	#define LCR_BPC_8 0x3  /**< 8 bits per character.                            */
	#define LCR_SB_1  0x0  /**< 1 stop bit.                                      */
	#define LCR_SB_2  0x4  /**< 1.5 stop bits (LCR_BPC_5) or 2 stop bits (else). */
	#define LCR_PE    0x8  /**< Parity Enabled.                                  */
	#define LCR_EPS   0x10 /**< Even Parity Select.                              */
	#define LCR_SP    0x20 /**< Stick Parity.                                    */
	#define LCR_BC    0x40 /**< Break Control.                                   */
	#define LCR_DLA   0x80 /**< Divisor Latch Access.                            */
	/**@}*/

	/**
	 * Line Status Register.
	 */
	/**@{*/
	#define LSR_DR  0x0  /**< Data Ready.                  */
	#define LSR_OE  0x2  /**< Overrun Error.               */
	#define LSR_PE  0x4  /**< Parity Error.                */
	#define LSR_FE  0x8  /**< Framing Error.               */
	#define LSR_BI  0x10 /**< Break Interrupt.             */
	#define LSR_TFE 0x20 /**< Transmitter FIFO Empty.      */
	#define LSR_TEI 0x40 /**< Transmitter Empty Indicator. */
	/**@}*/

	/**
	 * @brief Initializes the 8250 driver.
	 */
	extern void uart8250_init(void);

	/**
	 * @brief Writes a buffer on the 8250 device.
	 *
	 * @param buf Target buffer.
	 * @param n   Number of bytes to write.
	 */
	extern void uart8250_write(const char *buf, size_t n);

	/* Memory definitions. */
	#include <target/or1k/pc.h>

/**@}*/

#endif /* DRIVER_8250_H_ */

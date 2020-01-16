/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

#ifndef DEV_NET_ETHOC_H_
#define DEV_NET_ETHOC_H_

	#ifndef __NEED_ETHOC
		#error "do not include this file"
	#endif

	#include <nanvix/const.h>

	/**
	 * @brief Device base Addresses
	 */
	#define ETHOC_BASE 0x92000000

	/**
	 * @name Register Offsets
	 */
	/**@{*/
	#define ETHOC_MODER       0x00 /**< Mode Register                                */
	#define ETHOC_INT_SOURCE  0x04 /**< Interrupt Source Register                    */
	#define ETHOC_INT_MASK    0x08 /**< Interrupt Mask Register                      */
	#define ETHOC_IPGT        0x0c /**< Back to Back Inter Packet Gap Register       */
	#define ETHOC_IPGR1       0x10 /**< Non Back to Back Inter Packet Gap Register 1 */
	#define ETHOC_IPGR2       0x14 /**< Non Back to Back Inter Packet Gap Register 2 */
	#define ETHOC_PACKETLEN   0x18 /**< Packet Length Register                       */
	#define ETHOC_COLLCONF    0x1c /**< Collision and Retry Configuration Register   */
	#define ETHOC_TX_BD_NUM   0x20 /**< Transmit Buffer Descriptor Number Register   */
	#define ETHOC_CTRLMODER   0x24 /**< Control Module Mode Register                 */
	#define ETHOC_MIIMODER    0x28 /**< MII Mode Register                            */
	#define ETHOC_MIICOMMAND  0x2c /**< MII Command Register                         */
	#define ETHOC_MIIADDRESS  0x30 /**< MII Address Register                         */
	#define ETHOC_MIITX_DATA  0x34 /**< MII Transmit Data Register                   */
	#define ETHOC_MIIRX_DATA  0x38 /**< MII Receive Data Retister                    */
	#define ETHOC_MIISTATUS   0x3c /**< MII Status Register                          */
	#define ETHOC_MAC_ADDR0   0x40 /**< MAC Individual Address 0                     */
	#define ETHOC_MAC_ADDR1   0x44 /**< MAC Individual Address 1                     */
	#define ETHOC_ETH_HASH0   0x48 /**< HASH 0 Register                              */
	#define ETHOC_ETH_HASH1   0x4c /**< HASH 1 Register                              */
	#define ETHOC_ETH_TXCTRL  0x50 /**< Transmit Control Register                    */
	/**@}*/

	/**
	 * @brief Size of a buffer descriptor (in bytes).
	 */
	#define ETHOC_BD_SIZE 8

	/**
	 * @brief Number of TX buffers.
	 */
	#define ETHOC_TX_BUF_COUNT 64

	/**
	 * @brief Number of RX buffers.
	 */
	#define ETHOC_RX_BUF_COUNT 64

	/**
	 * @brief Offset for TX buffers.
	 */
	#define ETHOC_TX_BD_BASE 0x400

	/**
	 * @brief Offset for RX buffers.
	 */
	#define ETHOC_RX_BD_BASE (0x400 + ETHOC_TX_BUF_COUNT*ETHOC_BD_SIZE)

	/**
	 * @brief Buffer size (in bytes).
	 */
	#define	ETHOC_BUFSIZ 1536

/*============================================================================*
 * Mode Register                                                              *
 *============================================================================*/

	/**
	 * @name Moder Register
	 */
	/**@{*/
	#define	ETHOC_MODER_RXEN  (1 <<  0) /**< Receive Enable                     */
	#define	ETHOC_MODER_TXEN  (1 <<  1) /**< Transmit Enable                    */
	#define	ETHOC_MODER_NOPRE (1 <<  2) /**< No Preamble                        */
	#define	ETHOC_MODER_BRO	  (1 <<  3) /**< Broadcast Address                  */
	#define	ETHOC_MODER_IAM	  (1 <<  4) /**< Individual Address mode            */
	#define	ETHOC_MODER_PRO	  (1 <<  5) /**< Promiscuous Mode                   */
	#define	ETHOC_MODER_IFG	  (1 <<  6) /**< Interframe Gap for Incoming Frames */
	#define	ETHOC_MODER_LOOP  (1 <<  7) /**< Loopback                           */
	#define	ETHOC_MODER_NBO	  (1 <<  8) /**< No Back-Off                        */
	#define	ETHOC_MODER_EDE	  (1 <<  9) /**< Excess Defer Enable                */
	#define	ETHOC_MODER_FULLD (1 << 10) /**< Full Duplex                        */
	#define	ETHOC_MODER_RESET (1 << 11) /**< Reset                              */
	#define	ETHOC_MODER_DCRC  (1 << 12) /**< Delayed CRC Enable                 */
	#define	ETHOC_MODER_CRC	  (1 << 13) /**< CRC Enable                         */
	#define	ETHOC_MODER_HUGE  (1 << 14) /**< Huge Packets Enable                */
	#define	ETHOC_MODER_PAD	  (1 << 15) /**< Padding Enabled                    */
	#define	ETHOC_MODER_RSM	  (1 << 16) /**< Receive Small Packets              */
	/**@}*/

/*============================================================================*
 * Interrupt Source and Mask Registers                                        *
 *============================================================================*/

	/**
	 * @name Interrupt Source Register
	 */
	/**@{*/
	#define	ETHOC_INT_SOURCE_TXF  (1 << 0) /**< Transmit Frame         */
	#define	ETHOC_INT_SOURCE_TXE  (1 << 1) /**< Transmit Error         */
	#define	ETHOC_INT_SOURCE_RXF  (1 << 2) /**< Receive Frame          */
	#define	ETHOC_INT_SOURCE_RXE  (1 << 3) /**< Receive Error          */
	#define	ETHOC_INT_SOURCE_BUSY (1 << 4) /**< Busy                   */
	#define	ETHOC_INT_SOURCE_TXC  (1 << 5) /**< Transmit Control Frame */
	#define	ETHOC_INT_SOURCE_RXC  (1 << 6) /**< Receive Control Frame  */
	/**@}*/

	/**
	 * @name Interrupt Mask Register
	 */
	/**@{*/
	#define	ETHOC_INT_MASK_TXF  (1 << 0) /**< Transmit Frame         */
	#define	ETHOC_INT_MASK_TXE  (1 << 1) /**< Transmit Error         */
	#define	ETHOC_INT_MASK_RXF  (1 << 2) /**< Receive Frame          */
	#define	ETHOC_INT_MASK_RXE  (1 << 3) /**< Receive Error          */
	#define	ETHOC_INT_MASK_BUSY (1 << 4) /**< Busy                   */
	#define	ETHOC_INT_MASK_TXC  (1 << 5) /**< Transmit Control Frame */
	#define	ETHOC_INT_MASK_RXC  (1 << 6) /**< Receive Control Frame  */
	/**@}*/

	/**
	 * @brief Mask for TX buffers.
	 */
	#define	ETHOC_INT_MASK_TX (ETHOC_INT_MASK_TXF | ETHOC_INT_MASK_TXE)

	/**
	 * @brief Mask for RX buffers.
	 */
	#define	ETHOC_INT_MASK_RX (ETHOC_INT_MASK_RXF | ETHOC_INT_MASK_RXE)

	/**
	 * @brief Mask for TX and RX frames.
	 */
	#define	ETHOC_INT_MASK_FR (ETHOC_INT_MASK_TXC | ETHOC_INT_MASK_RXC)

	/**
	 * @brief Mask for everything.
	 */
	#define	ETHOC_INT_MASK_ALL                   \
		(ETHOC_INT_MASK_TX | ETHOC_INT_MASK_RX | \
		 ETHOC_INT_MASK_FR | ETHOC_INT_MASK_BUSY)

/*============================================================================*
 * Packet Length Register                                                     *
 *============================================================================*/

	/**
	 * @brief Minimum packet length.
	 */
	#define ETHOC_PACKETLEN_SIZE_MIN 0x040

	/**
	 * @brief Maximum packet length.
	 */
	#define ETHOC_PACKETLEN_SIZE_MAX 0x600

	/**
	 * @name Packet Length Register
	 */
	/**@{*/
	#define	ETHOC_PACKETLEN_MIN(min) (((min) & 0xffff) << 16) /**< Minimum Packet Length */
	#define	ETHOC_PACKETLEN_MAX(max) (((max) & 0xffff) <<  0) /**< Maximum Packet Length */
	/**@}*/

/*============================================================================*
 * Transmit Buffer Number Register                                            *
 *============================================================================*/

	/**
	 * @name Transmit Buffer Number Register
	 */
	/**@{*/
	#define	ETHOC_TX_BD_NUM_VAL(x) (((x) <= 0x80) ? (x) : 0x80)
	/**@}*/

/*============================================================================*
 * Control Module Mode Register                                               *
 *============================================================================*/

	/**
	 * @name Control Module mode Register
	 */
	/**@{*/
	#define	ETHOC_CTRLMODER_PASSALL (1 << 0) /**< Pass All Receive Frames */
	#define	ETHOC_CTRLMODER_RXFLOW  (1 << 1) /**< Receive Control Flow    */
	#define	ETHOC_CTRLMODER_TXFLOW  (1 << 2) /**< Transmit Control Flow   */
	/**@}*/

/*============================================================================*
 * TX Buffer Descriptor                                                       *
 *============================================================================*/

	/**
	 * @name TX Buffer Descriptor
	 */
	/**@{*/
	#define	ETHOC_TX_BD_CS	       (1 <<  0)              /**< Carrier Sense Lost           */
	#define	ETHOC_TX_BD_DF	       (1 <<  1)              /**< Defer Indication             */
	#define	ETHOC_TX_BD_LC	       (1 <<  2)              /**< Late Collision               */
	#define	ETHOC_TX_BD_RL         (1 <<  3)              /**< Retransmission Limit         */
	#define	ETHOC_TX_BD_RETRY(x)   (((x) & 0x00f0) >>  4) /**< Retry Count                  */
	#define	ETHOC_TX_BD_UR	       (1 <<  8)              /**< Transmitter Underrun         */
	#define	ETHOC_TX_BD_CRC        (1 << 11)              /**< CRC Enable                   */
	#define	ETHOC_TX_BD_PAD        (1 << 12)              /**< Pad Enable for Short Packets */
	#define	ETHOC_TX_BD_WRAP       (1 << 13)              /**< Wrap                         */
	#define	ETHOC_TX_BD_IRQ        (1 << 14)              /**< Interrupt Request Enable     */
	#define	ETHOC_TX_BD_READY      (1 << 15)              /**< TX Buffer Ready              */
	#define	ETHOC_TX_BD_LEN(x)     (((x) & 0xffff) << 16) /**< Buffer Length                */
	/**@}*/

	/**
	 * @name Masks for TX Buffers
	 */
	/**@{*/
	#define	ETHOC_TX_BD_RETRY_MASK	(0x00f0)      /**< Retry Count   */
	#define	ETHOC_TX_BD_LEN_MASK   (0xffff << 16) /**< Buffer Length */
	/**@}*/

	/**
	 * @brief TX buffer status.
	 */
	#define	ETHOC_TX_BD_STATS                                     \
		(ETHOC_TX_BD_CS | ETHOC_TX_BD_DF | ETHOC_TX_BD_LC |       \
		 ETHOC_TX_BD_RL | ETHOC_TX_BD_RETRY_MASK | ETHOC_TX_BD_UR)

/*============================================================================*
 * RX Buffer Descriptor                                                       *
 *============================================================================*/

	/**
	 * @name RX Buffer Descriptor
	 */
	/**@{*/
	#define	ETHOC_RX_BD_LC     (1 <<  0)              /**< Late Collision           */
	#define	ETHOC_RX_BD_CRC	   (1 <<  1)              /**< RX CRC Error             */
	#define	ETHOC_RX_BD_SF     (1 <<  2)              /**< Short Frame              */
	#define	ETHOC_RX_BD_TL     (1 <<  3)              /**< Too Long                 */
	#define	ETHOC_RX_BD_DN     (1 <<  4)              /**< Dribble Nibble           */
	#define	ETHOC_RX_BD_IS     (1 <<  5)              /**< Invalid Symbol           */
	#define	ETHOC_RX_BD_OR     (1 <<  6)              /**< Receiver Overrun         */
	#define	ETHOC_RX_BD_MISS   (1 <<  7)              /**< Miss                     */
	#define	ETHOC_RX_BD_CF     (1 <<  8)              /**< Control Frame            */
	#define	ETHOC_RX_BD_WRAP   (1 << 13)              /**< Wrap                     */
	#define	ETHOC_RX_BD_IRQ	   (1 << 14)              /**< Interrupt Request Enable */
	#define	ETHOC_RX_BD_EMPTY  (1 << 15)              /**< Empty                    */
	#define	ETHOC_RX_BD_LEN(x) (((x) & 0xffff) << 16) /**< Buffer Length            */
	/**@}*/

	/**
	 * @name Shifts for RX Buffers
	 */
	/**@{*/
	#define	ETHOC_RX_BD_LEN_SHIFT 16 /**< Buffer Length */
	/**@}*/

	/**
	 * @name Masks for RX Buffers
	 */
	/**@{*/
	#define	ETHOC_RX_BD_LEN_MASK (0xffff << 16) /**< Buffer Length */
	/**@}*/

	/**
	 * @brief RX buffer status.
	 */
	#define	ETHOC_RX_BD_STATS                                \
		(ETHOC_RX_BD_LC | ETHOC_RX_BD_CRC | ETHOC_RX_BD_SF | \
		 ETHOC_RX_BD_TL | ETHOC_RX_BD_DN | ETHOC_RX_BD_IS  | \
		 ETHOC_RX_BD_OR | ETHOC_RX_BD_MISS)

/*============================================================================*/

	/**
	 * @brief Ethernet device.
	 */
	struct ethoc_dev
	{
		/**
		 * @brief MAC address.
		 */
		uint8_t mac[6];

		/**
		 * @brief RX buffers.
		 */
		char rx_buffer[ETHOC_RX_BUF_COUNT][ETHOC_PACKETLEN_SIZE_MAX];

		/**
		 * @brief Next RX buffer.
		 */
		int rx_head;

		/**
		 * @brief TX buffers.
		 */
		char tx_buffer[ETHOC_TX_BUF_COUNT][ETHOC_PACKETLEN_SIZE_MAX];

		/**
		 * @brief Next TX buffer.
		 */
		int tx_head;
	};

	/**
	 * Launches self-tests.
	 */
	EXTERN void ethoc_test(void);

#endif /* !DEV_NET_ETHOC_H_ */

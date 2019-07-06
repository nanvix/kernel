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

/**
 * it seems that I should add some doxygen stuff here, but I don't know what
 */

#ifndef RTL8139_H_
#define RTL8139_H_

#include <stdint.h>

// Define some constants
#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

#define RX_BUF_ALLOC_SIZE 8192 / 4 + 16 + 1500
#define RX_BUF_SIZE 8192 / 4 + 16

#define CAPR 0x38
#define RX_READ_POINTER_MASK (~3)
#define ROK (1 << 0)
#define RER (1 << 1)
#define TOK (1 << 2)
#define TER (1 << 3)
#define TX_TOK (1 << 15)

enum RTL8139_registers
{
	COMMAND = 0x37,
	RX_BUFFER = 0x30,
	RX_CONFIG = 0x44,
	CONFIG1 = 0x52,
	INTERRUPT_MASK = 0x3C,
	INTERRUPT_STATUS = 0x3E,
	MAG0 = 0x00,	  // Ethernet hardware address
	MAR0 = 0x08,	  // Multicast filter
	TxStatus0 = 0x10, // Transmit status (Four 32bit registers)
	TxAddr0 = 0x20,   // Tx descriptors (also four 32bit)
	RxEarlyCnt = 0x34,
	RxEarlyStatus = 0x36,
	RxBufPtr = 0x38,
	RxBufAddr = 0x3A,
	TxConfig = 0x40,
	Timer = 0x48,	// A general-purpose counter
	RxMissed = 0x4C, // 24 bits valid, write clears
	Cfg9346 = 0x50,
	Config0 = 0x51,
	FlashReg = 0x54,
	GPPinData = 0x58,
	GPPinDir = 0x59,
	MII_SMI = 0x5A,
	HltClk = 0x5B,
	MultiIntr = 0x5C,
	TxSummary = 0x60,
	MII_BMCR = 0x62,
	MII_BMSR = 0x64,
	NWayAdvert = 0x66,
	NWayLPAR = 0x68,
	NWayExpansion = 0x6A,

	// Undocumented registers, but required for proper operation
	FIFOTMS = 0x70, // FIFO Control and test
	CSCR = 0x74,	// Chip Status and Configuration Register
	PARA78 = 0x78,
	PARA7c = 0x7c, // Magic transceiver parameter register
};

// struct tx_desc
// {
// 	uint32_t phys_addr;
// 	uint32_t packet_size;
// };

struct rtl8139_dev
{
	uint8_t bar_type;
	uint16_t io_base;
	uint32_t mem_base;
	int eeprom_exist;
	uint8_t mac_addr[6];
	uint8_t* rx_buffer;
	int tx_cur;
};

void dev_net_rtl8139_init();
void dev_net_rtl8139_send_packet(void *data, uint32_t len);

#endif /* RTL8139_H_ */
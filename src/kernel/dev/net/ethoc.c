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

/* Must come first. */
#define __NEED_ETHOC

#include <nanvix/hal/hal.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <dev/net/ethoc.h>
#include <stdint.h>

#include <dev/net/net.h>
#include <lwip/netif.h>

/**
 * @brief Disable driver debug mode?
 */
#define ETHOC_NDEBUG 0

/*===========================================================================*
 * OpenCores Ethernet Driver                                                 *
 *===========================================================================*/

/**
 * @brief Default MAC Address.
 */
PRIVATE const uint8_t MAC_DEFAULT[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };

/**
 * @brief Ethernet device.
 */
PRIVATE struct ethoc_dev ALIGN(PAGE_SIZE) ethoc;

/**
 * @brief Writes to a 32-bit register.
 *
 * @param reg Target register.
 * @param val Target value.
 */
PRIVATE inline void ethoc_write32(unsigned reg, uint32_t val)
{
	vaddr_t regptr;

	regptr = (vaddr_t) mmio_get(ETHOC_BASE);
	regptr += reg;

	*((uint32_t *) regptr) = val;
}

/**
 * @brief Reads a 32-bit register.
 *
 * @param reg Target register.
 *
 * @returns The register value.
 */
PRIVATE inline uint32_t ethoc_read32(unsigned reg)
{
	uint32_t val;
	vaddr_t regptr;

	regptr = (vaddr_t) mmio_get(ETHOC_BASE);
	regptr += reg;

	val = *((uint32_t *) regptr);

	return (val);
}

/**
 * @brief Enables the ethernet device.
 */
PRIVATE inline void ethoc_enable(void)
{
	uint32_t reg;

	reg = ethoc_read32(ETHOC_MODER);
	reg |= ETHOC_MODER_TXEN | ETHOC_MODER_RXEN;
	ethoc_write32(ETHOC_MODER, reg);
}

/**
 * @brief Disables the ethernet device.
 */
PRIVATE inline void ethoc_disable(void)
{
	uint32_t reg;

	reg = ethoc_read32(ETHOC_MODER);
	reg &= ~(ETHOC_MODER_TXEN | ETHOC_MODER_RXEN);
	ethoc_write32(ETHOC_MODER, reg);
}

/**
 * @brief Enables IRQs in the ethernet device.
 *
 * @param mask IRQ mask.
 */
PRIVATE inline void ethoc_irq_enable(uint32_t mask)
{
	uint32_t reg;

	reg = ethoc_read32(ETHOC_INT_MASK);
	reg |= mask;
	ethoc_write32(ETHOC_INT_MASK, reg);
}

/**
 * @brief Disables IRQs in the ethernet device.
 *
 * @param mask IRQ mask.
 */
PRIVATE inline void ethoc_irq_disable(uint32_t mask)
{
	uint32_t reg;

	reg = ethoc_read32(ETHOC_INT_MASK);
	reg &= ~mask;
	ethoc_write32(ETHOC_INT_MASK, reg);
}

/**
 * @brief Acknowledges IRQs.
 *
 * @param mask IRQ mask.
 */
PRIVATE inline void ethoc_irq_ack(uint32_t mask)
{
	ethoc_write32(ETHOC_INT_SOURCE, mask);
}

/**
 * @brief Writes an MAC address to the ethernet device.
 *
 * @param mac MAC address.
 */
PRIVATE void ethoc_mac_set(const uint8_t mac[6])
{
	uint32_t reg;

	reg =  (mac[2] << 24);
	reg |= (mac[3] << 16);
	reg |= (mac[4] <<  8);
	reg |= (mac[5] <<  0);
	ethoc_write32(ETHOC_MAC_ADDR0, reg);

	reg =  (mac[0] << 8);
	reg |= (mac[1] << 0);
	ethoc_write32(ETHOC_MAC_ADDR1, reg);
}

/**
 * @brief Reads the MAC address of the ethernet device.
 *
 * @param mac Store location for MAC address.
 */
PRIVATE void ethoc_mac_get(uint8_t mac[6])
{
	uint32_t reg;

	reg = ethoc_read32(ETHOC_MAC_ADDR0);
	mac[2] = (reg >> 24) & 0xff;
	mac[3] = (reg >> 16) & 0xff;
	mac[4] = (reg >>  8) & 0xff;
	mac[5] = (reg >>  0) & 0xff;

	reg = ethoc_read32(ETHOC_MAC_ADDR1);
	mac[0] = (reg >>  8) & 0xff;
	mac[1] = (reg >>  0) & 0xff;

    kprintf("[eth] mac address %x:%x:%x:%x:%x:%x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
	);
}

/**
 * @brief Initializes TX buffers.
 */
PRIVATE void ethoc_tx_buf_init(void)
{
	unsigned bd;  /* Working buffer descriptor. */
	uint32_t reg; /* Register value.            */

	/* TX buffer descriptors. */
	bd = ETHOC_TX_BD_BASE;
	reg = 0;
#ifdef ETH_USES_IRQ
	reg |= ETHOC_TX_BD_IRQ;
#endif
	for (int i = 0; i < ETHOC_TX_BUF_COUNT; i++)
	{
		if (i == (ETHOC_TX_BUF_COUNT - 1))
			reg |= ETHOC_TX_BD_WRAP;

		ethoc_write32(bd, reg);
		ethoc_write32(bd + 4, (uint32_t)&ethoc.tx_buffer[i][0]);

		bd += ETHOC_BD_SIZE;
	}

	ethoc.tx_head = 0;
}

/**
 * @brief Initializes RX buffers.
 */
PRIVATE void ethoc_rx_buf_init(void)
{
	unsigned bd;  /* Working buffer descriptor. */
	uint32_t reg; /* Register value.            */

	/* RX buffer descriptors */
	bd = ETHOC_RX_BD_BASE;
	reg = ETHOC_RX_BD_EMPTY;
#ifdef ETH_USES_IRQ
	reg |= ETHOC_RX_BD_IRQ;
#endif
	for (int i = 0; i < ETHOC_RX_BUF_COUNT; i++)
	{
		if (i == (ETHOC_RX_BUF_COUNT - 1))
			reg |= ETHOC_RX_BD_WRAP;

		ethoc_write32(bd, reg);
		ethoc_write32(bd + 4, (uint32_t)&ethoc.rx_buffer[i][0]);

		bd += ETHOC_BD_SIZE;
	}

	ethoc.rx_head = 0;
}

/**
 * @brief Resets the device.
 */
PRIVATE void ethoc_reset(void)
{
	uint32_t reg;

	/*  Disable buffers. */
	ethoc_disable();

	ethoc_tx_buf_init();
	ethoc_rx_buf_init();
	ethoc_mac_set(MAC_DEFAULT);

	/* Enable full duplex and small packets. */
	reg = ethoc_read32(ETHOC_MODER);
		reg |=  ETHOC_MODER_FULLD;
		reg |=  ETHOC_MODER_RSM;
		// reg |=  ETHOC_MODER_LOOP;
		reg |=  ETHOC_MODER_PAD;
	ethoc_write32(ETHOC_MODER, reg);

	/* Reconfigure timing. */
	ethoc_write32(ETHOC_IPGT, 0x15);

	/*
	 * ACK previous IRQs and unmask
	 * them all if we are using interrupts.
	 */
	ethoc_irq_ack(ETHOC_INT_MASK_ALL);
#ifdef ETH_USES_IRQ
	ethoc_irq_enable(ETHOC_INT_MASK_ALL);
#else
	ethoc_irq_disable(ETHOC_INT_MASK_ALL);
#endif

	/* Re-enable buffers. */
	ethoc_enable();
}

/**
 * @brief Interrupt handler.
 */
PRIVATE void do_ethoc(int num)
{
    UNUSED(num);

	kprintf("[eth] interrupt received!");

	while(1);
}

/**
 * @brief Gets a non-empty RX buffer.
 */
PRIVATE int ethoc_rx_get(void)
{
	int bd;

	bd = ethoc.rx_head;

	/* Search for a non-empty buffer. */
	for (int i = 0; i < ETHOC_RX_BUF_COUNT; i++)
	{
		uint32_t rx_reg;
		rx_reg = ethoc_read32(ETHOC_RX_BD_BASE + (bd*ETHOC_BD_SIZE));

		/* Found. */
		if (!(rx_reg & ETHOC_RX_BD_EMPTY))
		{
			ethoc.rx_head = (ethoc.rx_head + 1)%ETHOC_RX_BUF_COUNT;
			return (bd);
		}

		bd = (bd + 1)%ETHOC_RX_BUF_COUNT;
	}

	return (-EAGAIN);
}

/**
 * @brief Pools the device for an incoming data frame.
 *
 * @param buf  Store location for the incoming data frame.
 * @param size Buffer size.
 *
 * @returns Upon successful completion, an incoming data frame is
 * copied to @p dataf and the size of this data frame is returned.
 * Upon failure, a negative error code is returned instead.
 */
PRIVATE int ethoc_pool(void *dataf, size_t size)
{
	int bd;
	uint32_t rx_reg;

	/* Invalid dataffer. */
	if (dataf == NULL)
		return (-EINVAL);

	/* Invalid dataffer size. */
	if ((size == 0) || (size >= ETHOC_BUFSIZ))
		return (-EINVAL);

	/* There are no non-empty RX dataffers. */
	if ((bd = ethoc_rx_get()) < 0)
		return (-EAGAIN);

	kmemset(dataf, 0, size);
	kmemcpy(dataf, &ethoc.rx_buffer[bd][0], size);

	rx_reg = ethoc_read32(ETHOC_RX_BD_BASE + (bd*ETHOC_BD_SIZE));
		size = (rx_reg & ETHOC_RX_BD_LEN_MASK) >> ETHOC_RX_BD_LEN_SHIFT;
		#if !(defined(ETHOC_NDEBUG) && (ETHOC_NDEBUG == 1))
		kprintf("[eth] rx.bd = %d  rx.reg = %x size = %d", bd, rx_reg, size);
		#endif
		rx_reg &= ~(ETHOC_RX_BD_LEN_MASK | ETHOC_RX_BD_STATS);
		rx_reg |= ETHOC_RX_BD_EMPTY;
	ethoc_write32(ETHOC_RX_BD_BASE + (bd*ETHOC_BD_SIZE), rx_reg);

	return (size);
}

/**
 * @brief Gets an empty TX buffer.
 *
 * @returns Upon successful completion, the ID of an empty TX buffer
 * is returned. Upon failure, a negative error code is returned
 * instead.
 */
PRIVATE int ethoc_tx_get(void)
{
	int bd;

	bd = ethoc.tx_head;

	/*
	 * TODO: check status of this buffer.
	 */

	ethoc.tx_head = (ethoc.tx_head + 1)%ETHOC_TX_BUF_COUNT;

	return (bd);
}

/**
 * @brief Sends a data frame.
 *
 * @param dataf  Output data frame.
 * @param size   Data frame size.
 *
 * @returns Upon successful completion, zero is returned.  Upon
 * failure, a negative error code is returned instead.
 */
PRIVATE int ethoc_send(const void *dataf, size_t size)
{
	int bd;
	uint32_t tx_reg;

	/* Invalid dataffer. */
	if (dataf == NULL)
		return (-EINVAL);

	/* Invalid dataffer size. */
	if ((size == 0) || (size >= ETHOC_BUFSIZ))
		return (-EINVAL);

	/* No TX dataffer available. */
	if ((bd = ethoc_tx_get()) < 0)
		return (-EAGAIN);

	kmemcpy(&ethoc.tx_buffer[bd][0], dataf, size);

	tx_reg = ethoc_read32(ETHOC_TX_BD_BASE + (bd*ETHOC_BD_SIZE));
		tx_reg &= ~(ETHOC_TX_BD_LEN_MASK | ETHOC_TX_BD_STATS);
		tx_reg |= ETHOC_TX_BD_LEN(size);
		tx_reg |= ETHOC_TX_BD_READY;
	ethoc_write32(ETHOC_TX_BD_BASE + (bd*ETHOC_BD_SIZE), tx_reg);

	#if !(defined(ETHOC_NDEBUG) && (ETHOC_NDEBUG == 1))
	tx_reg = ethoc_read32(ETHOC_TX_BD_BASE + (bd*ETHOC_BD_SIZE));
	kprintf("[eth] tx.bd = %d tx.reg = %x", bd, tx_reg);
	#endif

	return (0);
}

/**
 * @brief Initializes the device.
 *
 * @param irqnum IRQ number.
 */
PRIVATE void ethoc_init(int irqnum)
{
	interrupt_register(irqnum, do_ethoc);

	ethoc_reset();

	ethoc_mac_get(ethoc.mac);
}

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_init(struct netif* netif)
{
	UNUSED(netif);
	ethoc_init(OR1K_INT_ETHOC);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_send_packet(struct packet packet)
{
	ethoc_send(packet.data, packet.len);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC int network_get_new_packet(struct packet* packet)
{
	int res = ethoc_pool(packet->data, ETHOC_PACKETLEN_SIZE_MAX - 1);
	packet->len = res;

	return (res >= 0);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC int network_is_new_packet(void)
{
	int bd;
	bd = ethoc.rx_head;

	/* Search for a non-empty buffer. */
	for (int i = 0; i < ETHOC_RX_BUF_COUNT; i++)
	{
		uint32_t rx_reg;
		rx_reg = ethoc_read32(ETHOC_RX_BD_BASE + (bd*ETHOC_BD_SIZE));

		/* Found. */
		if (!(rx_reg & ETHOC_RX_BD_EMPTY))
		{
			return (1);
		}

		bd = (bd + 1)%ETHOC_RX_BUF_COUNT;
	}
	return (0);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC int network_loopback_supported(void)
{
	return (0);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_loopback_enable(void)
{
	kprintf("[eth] loopback mode not supported");
	KASSERT(false);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_loopback_disable(void)
{
	kprintf("[eth] loopback mode not supported");
	KASSERT(false);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_get_mac_adress(uint8_t mac[6])
{
	ethoc_mac_get(mac);
}

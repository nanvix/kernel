/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <arch/arch.h>
#include <dev/uart.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief General Register Interface
 */
/**@{*/
#define UART_DATA 0x00 /** Data Register (R/W)                   */
#define UART_IIR 0x02  /** Interrupt Identification Register (R) */
#define UART_FCR 0x02  /** FIFO Control Register (W)             */
#define UART_LCR 0x03  /** Line Control Register (RW)            */
#define UART_MCR 0x04  /** Modem Control Register (W)            */
#define UART_LSR 0x05  /** Line Status Register (R)              */
#define UART_MSR 0x06  /** Modem Status Register (R)             */
#define UART_SCR 0x07  /** Scratch Register (RW)                 */
/**@}*/

/**
 * @brief Register Interface (when DLA is Unset in LCR)
 */
/**@{*/
#define UART_RBR 0x00 /** Receiver Buffer (R)              */
#define UART_THR 0x00 /** Transmitter Holding Register (W) */
#define UART_IER 0x01 /** Interrupt Enable Register (RW)   */
/**@}*/

/**
 * @brief Register Interface (when DLA is Set in LCR)
 */
/**@{*/
#define UART_DLL 0x00 /** Divisor Latch LSB (RW) */
#define UART_DLM 0x01 /** Divisor Latch MSB (RW) */
/**@}*/

/**
 * @brief Bits in the Interrupt Enable Register (IER)
 */
/**@{*/
#define UART_IER_RDAI (1 << 0) /** Receiver Data Available Interrupt      */
#define UART_IER_THRI (1 << 1) /** Transmitter Holding Register Interrupt */
#define UART_IER_RLSI (1 << 2) /** Receiver Line Status Interrupt         */
#define UART_IER_MSI (1 << 3)  /** Modem Status Interrupt                 */
/**@}*/

/**
 * @brief Values for the Interrupt Identification Register
 */
/**@{*/
#define UART_IIR_MS 0x00   /** Modem Status                       */
#define UART_IIR_THRE 0x02 /** Transmitter Holding Register Empty */
#define UART_IIR_RDA 0x04  /** Receiver Data Available            */
#define UART_IIR_RLS 0x06  /** Receiver Line Status               */
/**@}*/

/**
 * @brief Bits in the FIFO Control Register
 */
/**@{*/
#define UART_FCR_DISABLE_BIT (1 << 0) /** Disable FIFO           */
#define UART_FCR_CLRRECV_BIT (1 << 1) /** Clear Receiver FIFO    */
#define UART_FCR_CLRTMIT_BIT (1 << 2) /** Clear Transmitter FIFO */
#define UART_FCR_DMA_SEL_BIT (1 << 3) /** DMA Select             */
/**@}*/

/**
 * @brief Values for the FIFO Control (Bits 7-6)
 */
/**@{*/
#define UART_FCR_TRIG_1 0x00  /** Trigger Level 1 byte   */
#define UART_FCR_TRIG_4 0x40  /** Trigger Level 4 bytes  */
#define UART_FCR_TRIG_8 0x80  /** Trigger Level 8 bytes  */
#define UART_FCR_TRIG_14 0xc0 /** Trigger Level 14 bytes */
/**@}*/

/**
 * @brief Values for the Line Control Register (LCR)
 */
/**@{*/
#define UART_LCR_DLA 0x80           /** Divisor Latch Access */
#define UART_LCR_BPC_5 0x00         /** 5 Bits per Character */
#define UART_LCR_BPC_6 0x01         /** 6 Bits per           */
#define UART_LCR_BPC_7 0x02         /** 7 Bits per Character */
#define UART_LCR_BPC_8 0x03         /** 8 Bits per Character */
#define UART_LCR_STOP_SINGLE 0x00   /** Single Stop Bit      */
#define UART_LCR_STOP_VARIABLE 0x04 /** Variable Stop Bits   */
#define UART_LCR_PARITY_NONE 0x00   /** No Parity            */
#define UART_LCR_PARITY_ODD 0x08    /** Odd Parity           */
#define UART_LCR_PARITY_EVEN 0x0c   /** Even Parity          */
/**@}*/

/**
 * @brief Bits in the Modem Control Register
 */
/**@{*/
#define UART_MCR_DTR (1 << 0)  /* Data Terminal Ready */
#define UART_MCR_RTS (1 << 1)  /* Request Send Ping   */
#define UART_MCR_OUT1 (1 << 2) /* Output Pin 1        */
#define UART_MCR_OUT2 (1 << 3) /* Output Pint 2       */
#define UART_MCR_LOOP (1 << 4) /* Loopback Mode       */
/**@}*/

/**
 * @brief Bits in the Line Status Register (LSR)
 */
/**@{*/
#define UART_LSR_DR (1 << 0)  /** Data Ready                  */
#define UART_LSR_OE (1 << 1)  /** Overrun Error               */
#define UART_LSR_PE (1 << 2)  /** Parity Error                */
#define UART_LSR_FE (1 << 3)  /** Framing Error               */
#define UART_LSR_BI (1 << 4)  /** Break Indicator             */
#define UART_LSR_TFE (1 << 5) /** Transmitter FIFO Empty      */
#define UART_LSR_TEI (1 << 6) /** Transmitter Empty Indicator */
#define UART_LSR_ERR (1 << 7) /** Erroneous Data in FIFO      */
/**@}*/

/**
 * @brief Bits in the Modem Status Register (MSR)
 */
/**@{*/
#define UART_MSR_CCTS (1 << 0) /** Change in CTS        */
#define UART_MSR_CDSR (1 << 1) /** Change id DSR        */
#define UART_MSR_TERI (1 << 2) /** Trailing Edge RI     */
#define UART_MSR_CDCD (1 << 3) /** Change in CD         */
#define UART_MSR_CTS (1 << 4)  /** Clear to Send (CTS)  */
#define UART_MSR_DSR (1 << 5)  /** Data Set Ready (DSR) */
#define UART_MSR_RI (1 << 6)   /** Ring Indicator (RI)  */
#define UART_MSR_CD (1 << 7)   /** Carrier Detect CD    */
/**@}*/

/*============================================================================*
 * Global Variables                                                           *
 *============================================================================*/

/**
 * @brief Set once the device was initialized.
 */
static bool initialized = false;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

#if defined(__TARGET_HAS_PMIO) && (__TARGET_HAS_PMIO == 1)

/**
 * @brief Base address of UART device.
 */
static uint16_t uart_base_addr = 0;

/**
 * @brief Reads the line control register.
 *
 * @returns The contents of the line control register.
 */
static uint8_t uart_read_lcr(void)
{
    return (input8(uart_base_addr + UART_LCR));
}

/**
 * @brief Writes to the line control register.
 *
 * @param value Value to be written.
 */
static void uart_write_lcr(uint8_t value)
{
    output8(uart_base_addr + UART_LCR, value);
}

/**
 * @brief Writes to the modem control register.
 *
 * @param value Value to be written.
 */
static void uart_write_mcr(uint8_t value)
{
    output8(uart_base_addr + UART_MCR, value);
}

/**
 * @brief Writes to the fifo control register.
 *
 * @param value Value to be written.
 */
static void uart_write_fcr(uint8_t value)
{
    output8(uart_base_addr + UART_FCR, value);
}

/**
 * @brief Writes to the data register.
 *
 * @param value Value to be written.
 */
static void uart_write_data(uint8_t value)
{
    output8(uart_base_addr + UART_DATA, value);
}

/**
 * @brief Disables all interrupts.
 */
static void uart_disable_interrupts(void)
{
    output8(uart_base_addr + UART_IER, 0x00);
}

/**
 * @brief Sets baud rate.
 *
 * @param divisor Divisor value.
 */
static void uart_set_baud_rate(uint16_t divisor)
{
    uint8_t lcr = uart_read_lcr();

    /* Enable divisor latch access bit. */
    uart_write_lcr(lcr | UART_LCR_DLA);

    /* Set divisor LSB. */
    output8(uart_base_addr + UART_DLL, divisor & 0xff);

    /* Set divisor MSB */
    output8(uart_base_addr + UART_DLM, (divisor >> 8) & 0xff);

    /* Disable divisor latch access bit. */
    uart_write_lcr(lcr & ~UART_LCR_DLA);
}

/**
 * @brief Waits for the transmitter FIFO to be empty.
 */
static void uart_wait_for_transmitter_empty(void)
{
    while ((input8(uart_base_addr + UART_LSR) & UART_LSR_TFE) == 0) {
        /* noop */;
    }
}

#else

/**
 * @brief Base address of UART device.
 */
static volatile uint8_t *uart_base_addr = NULL;

/**
 * @brief Reads the line control register.
 *
 * @returns The contents of the line control register.
 */
static uint8_t uart_read_lcr(void)
{
    return (uart_base_addr[UART_LCR]);
}

/**
 * @brief Writes to the line control register.
 *
 * @param value Value to be written.
 */
static void uart_write_lcr(uint8_t value)
{
    uart_base_addr[UART_LCR] = value;
}

/**
 * @brief Writes to the modem control register.
 *
 * @param value Value to be written.
 */
static void uart_write_mcr(uint8_t value)
{
    uart_base_addr[UART_MCR] = value;
}

/**
 * @brief Writes to the fifo control register.
 *
 * @param value Value to be written.
 */
static void uart_write_fcr(uint8_t value)
{
    uart_base_addr[UART_FCR] = value;
}

/**
 * @brief Writes to the data register.
 *
 * @param value Value to be written.
 */
static void uart_write_data(uint8_t value)
{
    uart_base_addr[UART_DATA] = value;
}

/**
 * @brief Disables all interrupts.
 */
static void uart_disable_interrupts(void)
{
    uart_base_addr[UART_IER] = 0x00;
}

/**
 * @brief Sets baud rate.
 *
 * @param divisor Divisor value.
 */
static void uart_set_baud_rate(uint16_t divisor)
{
    uint8_t lcr = uart_read_lcr();

    /* Enable divisor latch access bit. */
    uart_write_lcr(lcr | UART_LCR_DLA);

    /* Set divisor LSB. */
    uart_base_addr[UART_DLL] = divisor & 0xff;

    /* Set divisor MSB */
    uart_base_addr[UART_DLM] = (divisor >> 8) & 0xff;

    /* Disable divisor latch access bit. */
    uart_write_lcr(lcr & ~UART_LCR_DLA);
}

/**
 * @brief Waits for the transmitter FIFO to be empty.
 */
static void uart_wait_for_transmitter_empty(void)
{
    while ((uart_base_addr[UART_LSR] & UART_LSR_TFE) == 0) {
        /* noop */;
    }
}

#endif /* __TARGET_HAS_PMIO == 1 */

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Writes a buffer on the UART device.
 */
void uart_write(const char *buf, size_t len)
{
    /* Device is not initialized, do nothing. */
    if (!initialized) {
        return;
    }

    /* Write buffer. */
    while (len-- > 0) {
        /* Wait until FIFO is empty */
        uart_wait_for_transmitter_empty();

        /* Write a single character. */
        uart_write_data(*buf++);
    }
}

/**
 * @details Initializes the UART device.
 */
int uart_init(uintptr_t addr, uint16_t baud)
{
    /* Do not re-initialize the device. */
    if (initialized) {
        return -1;
    }

#if defined(__TARGET_HAS_PMIO) && (__TARGET_HAS_PMIO == 1)
    uart_base_addr = (uint16_t)addr;
#else
    uart_base_addr = (uint8_t *)addr;
#endif /* __TARGET_HAS_PMIO == 1 */

    /* Disable all interrupts */
    uart_disable_interrupts();

    /* Set baud rate. */
    uart_set_baud_rate(baud);

    /* 8 bits per character, no parity, one stop bit */
    uart_write_lcr(UART_LCR_BPC_8 | UART_LCR_PARITY_NONE |
                   UART_LCR_STOP_SINGLE);

    /* Enable FIFOs, clear them, use 14-byte threshold. */
    uart_write_fcr(UART_FCR_CLRRECV_BIT | UART_FCR_CLRTMIT_BIT |
                   UART_FCR_TRIG_14);

    /* Enable data terminal ready, request to send, output 1 and output 2. */
    uart_write_mcr(UART_MCR_DTR | UART_MCR_RTS | UART_MCR_OUT1 | UART_MCR_OUT2);

    // TODO: Add a test here to check if the output serial line is faulty.

    /* Device is now initialized. */
    initialized = true;

    return (0);
}

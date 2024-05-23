/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>
#include <stdbool.h>
#include <stdint.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Size of the I/O APIC structure
 */
#define IOAPIC_SIZE 0x14

/**
 * @brief Base interrupt vector number for I/O APIC.
 */
#define IOAPIC_INTVEC_BASE 32

/**
 * @addtogroup ioapic-registers I/O APIC Registers
 */
/**@{*/

/**
 * @brief I/O APIC ID (RW)
 *
 * @details This register contains the 4-bit APIC ID. The ID serves as a
 * physical name of the I/O APIC. All APIC devices using the APIC bus should
 * have a unique APIC ID. This register must be programmed with the correct ID
 * value before using the I/O APIC for message transmission.

 * @addtogroup ioapic-ioapicid I/O APIC ID Register
 */
#define IOAPICID 0x00

/**
 * @brief I/O APIC Version (RO)
 *
 * @details The IOAPIC Version Register identifies the APIC hardware version.
 * Software can use this to provide compatibility between different APIC
 * implementations and their versions. In addition, this register provides the
 * maximum number of entries in the I/O Redirection Table.
 *
 * @addtogroup ioapic-ioapicver I/O APIC Version Register
 */
#define IOAPICVER 0x01

/**
 * @brief I/O APIC Arbitration ID (RO)
 *
 * @details The APICARB Register contains the bus arbitration priority for the
 * IOAPIC. This register is loaded when the IOAPIC ID Register is written.
 *
 * @addtogroup ioapic-ioapicarb I/O APIC Arbitration ID Register
 */
#define IOAPICARB 0x02

/**
 * @brief I/O APIC Redirection Table (RW)
 *
 * @details The I/O Redirection Table is a 24-entry table that contains the
 * redirection entries. Each entry is 64 bits wide and controls how each
 * interrupt is routed to the CPU.
 *
 * @addtogroup ioapic-ioredtbl I/O APIC Redirection Table Register
 */
#define IOREDTBL 0x10
/**@}*/

/**
 * @ingroup ioapic-ioapicid
 */
/**@{*/
#define IOAPICID_RESERVED_0_SHIFT (0)
#define IOAPICID_SHIFT (24)
#define IOAPICID_RESERVED_1_SHIFT (28)
#define IOAPICID_RESERVED_0_MASK (0xffffff << IOAPICID_RESERVED_0_SHIFT)
#define IOAPICID_ID_MASK (0x0F << IOAPICID_SHIFT)
#define IOAPICID_RESERVED_1_MASK (0xf << IOAPICID_RESERVED_1_SHIFT)
/**@}*/

/**
 * @ingroup ioapic-ioapicver
 */
/**@{*/
#define IOAPICVER_VERSION_SHIFT (0)
#define IOAPICVER_RESERVED_0_SHIFT (8)
#define IOAPICVER_MAXREDIR_SHIFT (16)
#define IOAPICVER_RESERVED_1_SHIFT (24)
#define IOAPICVER_VERSION_MASK (0xFF << IOAPICVER_VERSION_SHIFT)
#define IOAPICVER_RESERVED_0_MASK (0xFF << IOAPICVER_RESERVED_0_SHIFT)
#define IOAPICVER_MAXREDIR_MASK (0xFF << IOAPICVER_MAXREDIR_SHIFT)
#define IOAPICVER_RESERVED_1_MASK (0xFF << IOAPICVER_RESERVED_1_SHIFT)
/**@}*/

/**
 * @ingroup ioapic-ioapicarb
 */
/**@{*/
#define IOAPICARB_RESERVED_0_SHIFT (0)
#define IOAPICARB_ID_SHIFT (24)
#define IOAPICARB_RESERVED_1_SHIFT (28)
#define IOAPICARB_RESERVED_0_MASK (0xFF << IOAPICARB_RESERVED_0_SHIFT)
#define IOAPICARB_ID_MASK (0xFF << IOAPICARB_ID_SHIFT)
#define IOAPICARB_RESERVED_1_MASK (0xF << IOAPICARB_RESERVED_1_SHIFT)
/**@}*/

/**
 * @brief I/O APIC Redirection Table Entry (31:0)
 *
 * @addtogroup ioapic-ioredtbl-low Lower 32 Bits
 * @ingroup ioapic-ioredtbl
 */
/**@{*/
#define IOREDTBL_INTVEC_SHIFT (0)
#define IOREDTBL_DELIVMODE_SHIFT (8)
#define IOREDTBL_DESTMOD_SHIFT (11)
#define IOREDTBL_DELIVS_SHIFT (12)
#define IOREDTBL_INTPOL_SHIFT (13)
#define IOREDTBL_RIRR_SHIFT (14)
#define IOREDTBL_TRIGGER_SHIFT (15)
#define IOREDTBL_INTMASK_SHIFT (16)
#define IOREDTBL_INTVEC_MASK (0xFF << IOREDTBL_INTVEC_SHIFT)
#define IOREDTBL_DELIVMODE_MASK (0x7 << IOREDTBL_DELIVMODE_SHIFT)
#define IOREDTBL_DESTMOD_MASK (0x1 << IOREDTBL_DESTMOD_SHIFT)
#define IOREDTBL_DELIVS_MASK (0x1 << IOREDTBL_DELIVS_SHIFT)
#define IOREDTBL_INTPOL_MASK (0x1 << IOREDTBL_INTPOL_SHIFT)
#define IOREDTBL_RIRR_MASK (0x1 << IOREDTBL_RIRR_SHIFT)
#define IOREDTBL_TRIGGER_MASK (0x1 << IOREDTBL_TRIGGER_SHIFT)
#define IOREDTBL_INTMASK_MASK (0x1 << IOREDTBL_INTMASK_SHIFT)
/**@}*/

/**
 * @brief I/O APIC Redirection Table Entry (63:32)
 *
 * @addtogroup ioapic-ioredtbl-high Higher 32 Bits
 * @ingroup ioapic-ioredtbl
 */
/**@{*/
#define IOREDTBL_DEST_SHIFT (24)
#define IOREDTBL_DEST_MASK (0xFF << IOREDTBL_DEST_SHIFT)
/**@}*/

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief I/O APIC
 */
static volatile struct ioapic {
    uint32_t reg;    /** I/O Register Select (IOREGSEL) */
    uint32_t pad[3]; /** Padding                        */
    uint32_t data;   /** I/O Window (IOWIN)            */
} *ioapic = NULL;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Reads a register from the I/O APIC.
 *
 * @param reg Register to read.
 *
 * @return Returns the value of the register.
 */
static uint32_t ioapic_read(const uint32_t reg)
{
    ioapic->reg = reg;
    return (ioapic->data);
}

static void ioapic_write(const uint32_t reg, const uint32_t data)
{
    ioapic->reg = reg;
    ioapic->data = data;
}

/**
 * @brief Reads the ID of the I/O APIC.
 *
 * @return Returns the ID of the I/O APIC.
 */
static uint8_t ioapic_id_read(void)
{
    const uint32_t id =
        (ioapic_read(IOAPICID) & IOAPICID_ID_MASK) >> IOAPICID_SHIFT;
    // NOTE: The following cast is safe because "id" is a 4-bit value.
    return ((uint8_t)(id & 0xff));
}

/**
 * @brief Reads the version of the I/O APIC.
 *
 * @return Returns the version of the I/O APIC.
 */
static uint8_t ioapic_version_read(void)
{
    const uint32_t version =
        (ioapic_read(IOAPICVER) & IOAPICVER_VERSION_MASK) >>
        IOAPICVER_VERSION_SHIFT;
    // NOTE: The following cast is safe because "version" is an 8-bit value.
    return ((uint8_t)(version & 0xff));
}

/**
 * @brief Reads the number of redirection entries of the I/O APIC.
 *
 * @return Returns the number of redirection entries of the I/O APIC.
 */
static uint8_t ioapic_maxredir_read(void)
{
    const uint32_t maxredir =
        (ioapic_read(IOAPICVER) & IOAPICVER_MAXREDIR_MASK) >>
        IOAPICVER_MAXREDIR_SHIFT;
    // NOTE: The following cast is not safe, thus we must check "maxredir".
    KASSERT(maxredir < (uint8_t)-1);
    // NOTE: We add one because the count is zero-based.
    return ((uint8_t)((maxredir + 1) & 0xff));
}

/**
 * @brief Writes to a redirection table entry (low).
 *
 * @param irq IRQ number.
 * @param data Data to write.
 */
static void ioapic_redtbl_low_write(const uint8_t irq, const uint32_t data)
{
    // NOTE: The following cast is safe because "irq" is an 8-bit value.
    ioapic_write(IOREDTBL + 2 * (uint32_t)irq, data);
}

/**
 * @brief Writes to a redirection table entry (high).
 *
 * @param irq IRQ number.
 * @param data Data to write.
 */
static void ioapic_redtbl_high_write(const uint8_t irq, const uint32_t data)
{
    // NOTE: The following cast is safe because "irq" is an 8-bit value.
    ioapic_write(IOREDTBL + 2 * (uint32_t)irq + 1, data);
}

/**
 * @brief Writes to a redirection table entry.
 *
 * @param irq IRQ number.
 * @param high High data to write.
 * @param low Low data to write.
 */
static void ioapic_redtbl_write(const uint8_t irq, const uint32_t high,
                                const uint32_t low)
{
    ioapic_redtbl_low_write(irq, low);
    ioapic_redtbl_high_write(irq, high);
}

/**
 * @brief Prints information about the I/O APIC.
 */
static void ioapic_info(void)
{
    info("ioapic id: %d", ioapic_id_read());
    info("ioapic version: %d", ioapic_version_read());
    info("ioapic max redirection entries: %d", ioapic_maxredir_read());
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Enables an interrupt line.
 *
 * @param irq Number of target interrupt line.
 * @param cpunum CPU number.
 */
int ioapic_enable(uint8_t irq, uint8_t cpunum)
{
    // When using physical destination mode, only the lower 4 bits of the
    // destination field are used. The specification is unclear about the
    // behavior of the upper bits.  See 82093AA I/O ADVANCED PROGRAMMABLE
    // INTERRUPT CONTROLLER (IOAPIC) for details
    const char MAXIMUM_NUMBER_CPUS = 16;

    // Check IRQ lies in a valid range.
    if (irq >= ioapic_maxredir_read()) {
        error("invalid irq number (irq=%d)", irq);
        return (-EINVAL);
    }

    // Check CPU number lies in a valid range.
    if (cpunum > MAXIMUM_NUMBER_CPUS) {
        error("invalid cpunum number (cpunum=%d)", cpunum);
        return (-EINVAL);
    }

    // Set physical destination mode to cpunum; set high activate; set
    // edge-triggered; set enabled; set fixed delivery mode; identity map
    // interrupt.
    // NOTE: The following cast is safe because "cpunum" is a 4-bit value.
    // NOTE: The following cast is safe because "irq" is an 8-bit value.
    ioapic_redtbl_write(
        irq, ((uint32_t)cpunum) << 24, IOAPIC_INTVEC_BASE + (uint32_t)irq);

    return (0);
}

/**
 * @details Initializes an I/O APIC.
 */
int ioapic_init(uint8_t id, uint32_t addr, uint32_t gsi)
{
    static bool initialized = false;

    // Check for double initialization.
    if (initialized) {
        error("ioapic already initialized");
        return (-EBUSY);
    }

    // Sanity check sizes.
    KASSERT_SIZE(sizeof(struct ioapic), IOAPIC_SIZE);

    info("initializing ioapic (id=%d, addr=%x, gsi=%d)", id, addr, gsi);

    // Set I/O APIC base address.
    ioapic = (volatile struct ioapic *)addr;

    // Check ID mismatch.
    if (ioapic_id_read() != id) {
        error("ID mistmatch (id=%d, ioapicid=%d)", id, ioapic_id_read());
        return (-EINVAL);
    }

    ioapic_info();

    const uint8_t maxintr = ioapic_maxredir_read();

    // For all interrupts: set physical destination mode to APIC ID 0; set high
    // activate; set edge-triggered; set disabled; set fixed delivery mode;
    // identity map interrupts.
    for (uint8_t i = 0; i < maxintr; i++) {
        // NOTE: The following cast is safe because "i" is an 8-bit value.
        ioapic_redtbl_write(
            i, 0, IOREDTBL_INTMASK_MASK | (IOAPIC_INTVEC_BASE + (uint32_t)i));
    }

    // Initialization completed.
    initialized = true;

    return (0);
}

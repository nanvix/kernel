/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>
#include <stdbool.h>
#include <stdint.h>

// Enable conversions check
#pragma GCC diagnostic error "-Wconversion"
#pragma GCC diagnostic error "-Warith-conversion"

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief xAPIC Registers
 */
/**@{*/
#define XAPIC_ID 0x0020    /* ID Register                         */
#define XAPIC_VER 0x0030   /* Version Register                    */
#define XAPIC_TPR 0x0080   /* Task Priority                       */
#define XAPIC_APR 0x0090   /* Arbitration Priority                */
#define XAPIC_PPR 0x00A0   /* Processor Priority                  */
#define XAPIC_EOI 0x00B0   /* EOI Register                        */
#define XAPIC_RRD 0x00C0   /* Remote Read Register                */
#define XAPIC_LDR 0x00D0   /* Logical Destination Register        */
#define XAPIC_DFR 0x00E0   /* Destination Format Register         */
#define XAPIC_SVR 0x00F0   /* Spurious Interrupt Vector Register  */
#define XAPIC_ISR 0x0100   /* In-Service Register                 */
#define XAPIC_TMR 0x0180   /* Trigger Mode Register               */
#define XAPIC_IRR 0x0200   /* Interrupt Request Register          */
#define XAPIC_ESR 0x0280   /* Error Status Register               */
#define XAPIC_CMCI 0x02F0  /* CMCI LVT Register                   */
#define XAPIC_ICRLO 0x0300 /* Interrupt Command Register          */
#define XAPIC_ICRHI 0x0310 /* Interrupt Command Register          */
#define XAPIC_TIMER 0x0320 /* Timer LVT Register                  */
#define XAPIC_THERM 0x0330 /* Thermal Sensor LVT Register         */
#define XAPIC_PCINT 0x0340 /* Performance Counter LVT Register    */
#define XAPIC_LINT0 0x0350 /* Local Interrupt 0 LVT Register      */
#define XAPIC_LINT1 0x0360 /* Local Interrupt 1 LVT Register      */
#define XAPIC_ERROR 0x0370 /* Error LVT Register                  */
#define XAPIC_TICR 0x0380  /* Timer Initial Count Register        */
#define XAPIC_TCCR 0x0390  /* Timer Current Count Register        */
#define XAPIC_TDCR 0x03E0  /* Timer Divide Configuration Register */
/**@}*/

/**
 * @brief Size of ID Register (ID).
 */
#define XAPIC_ID_SIZE 4

/**
 * @brief Size of Version Register (VER).
 */
#define XAPIC_VER_SIZE 4

/**
 * @brief Size of Task Priority Register (TPR).
 */
#define XAPIC_TPR_SIZE 4

/**
 * @brief Size of the Spurious-Interrupt Vector Register (SVR).
 */
#define XAPIC_SVR_SIZE 4

/**
 * @brief Size of the Error Status Register (ESR).
 */
#define XAPIC_ESR_SIZE 4

/**
 * @brief Size of a Local Vector Table (LVT) Entry.
 */
#define XAPIC_LVT_ENTRY_SIZE 4

/**
 * @brief Size of the Low Interrupt Command Register (ICR).
 */
#define XAPIC_ICRLO_SIZE 4

/**
 * @brief Size of the High Interrupt Command Register (ICR).
 */
#define XAPIC_ICRHI_SIZE 4

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief ID Register (ID).
 */
struct xapic_id {
    uint32_t id : 8; /* Local APIC ID */
    uint32_t : 24;   /* Reserved      */
};

/**
 * @brief Version Register (VER).
 */
struct xapic_ver {
    uint32_t version : 8;       /* Version Number                       */
    uint32_t : 8;               /* Reserved                             */
    uint32_t max_lvt : 8;       /* Maximum LVT Entry                    */
    uint32_t eoi_broadcast : 1; /* EOI Broadcast Suppression Supported? */
    uint32_t : 7;               /* Reserved                             */
};

/**
 * @brief Task Priority Register (TPR).
 */
struct xapic_tpr {
    uint32_t priority_subclass : 4; /* Priority Subclass */
    uint32_t priority_class : 4;    /* Priority Class    */
    uint32_t : 24;                  /* Reserved          */
};

/**
 * @brief Spurious-Interrupt Vector Register (SVR).
 */
struct xapic_svr {
    uint32_t vector : 8;         /* Spurious Vector              */
    uint32_t apic_enabled : 1;   /* APIC Software Enable/Disable */
    uint32_t focus_checking : 1; /* Focus Processor Checking     */
    uint32_t : 2;                /* Reserved                     */
    uint32_t eoi_broadcast : 1;  /* EOI Broadcast Suppression    */
    uint32_t : 19;               /* Reserved                     */
};

/**
 * @brief Error Status Register (ESR).
 */
struct xapic_esr {
    uint32_t send_checksum_error : 1;      /* Send Checksum Error      */
    uint32_t receive_checksum_error : 1;   /* Receive Checksum Error   */
    uint32_t send_accept_error : 1;        /* Send Accept Error        */
    uint32_t receive_accept_error : 1;     /* Receive Accept Error     */
    uint32_t redirectable : 1;             /* Redirectable IPI         */
    uint32_t send_illegal_vector : 1;      /* Send Illegal Vector      */
    uint32_t received_illegal_vector : 1;  /* Received Illegal Vector  */
    uint32_t illegal_register_address : 1; /* Illegal Register Address */
    uint32_t : 24;                         /* Reserved                 */
};

/**
 * @brief Corrected Machine Check Interrupt (CMCI) Register.
 */
struct xapic_cmci {
    uint32_t vector : 8;          /* CMCI Vector     */
    uint32_t delivery_mode : 3;   /* Delivery Mode   */
    uint32_t : 1;                 /* Reserved        */
    uint32_t delivery_status : 1; /* Delivery Status */
    uint32_t : 3;                 /* Reserved        */
    uint32_t mask : 1;            /* Mask            */
    uint32_t : 15;                /* Reserved        */
};

/**
 * @brief Lower 32-bits of the Interrupt Command Register (ICR).
 */
struct xapic_icrlo {
    uint32_t vector : 8;                /* Vector (RW)                */
    uint32_t delivery_mode : 3;         /* Delivery Mode (RW)         */
    uint32_t destination_mode : 1;      /* Destination Mode (RW)      */
    uint32_t delivery_status : 1;       /* Delivery Status (RO)       */
    uint32_t : 1;                       /* Reserved                   */
    uint32_t level : 1;                 /* Level (RW)                 */
    uint32_t trigger_mode : 1;          /* Trigger Mode (RW)          */
    uint32_t : 2;                       /* Reserved                   */
    uint32_t destination_shorthand : 2; /* Destination Shorthand (RW) */
    uint32_t : 12;                      /* Reserved                   */
};

/**
 * @brief Higher 32-bits of the Interrupt Command Register (ICR).
 */
struct xapic_icrhi {
    uint32_t reserved : 24;   /* Reserved              */
    uint32_t destination : 8; /* Destination           */
};

/**
 * @brief Timer Register.
 */
struct xapic_timer {
    uint32_t vector : 8;          /* Timer Vector    */
    uint32_t : 4;                 /* Reserved        */
    uint32_t delivery_status : 1; /* Delivery Status */
    uint32_t : 3;                 /* Reserved        */
    uint32_t mask : 1;            /* Mask            */
    uint32_t mode : 2;            /* Timer Mode      */
    uint32_t : 12;                /* Reserved        */
};

/**
 * @brief Thermal Register.
 */
struct xapic_thermal {
    uint32_t vector : 8;          /* Thermal Vector  */
    uint32_t delivery_mode : 3;   /* Delivery Mode   */
    uint32_t : 1;                 /* Reserved        */
    uint32_t delivery_status : 1; /* Delivery Status */
    uint32_t : 3;                 /* Reserved        */
    uint32_t mask : 1;            /* Mask            */
    uint32_t : 15;                /* Reserved        */
};

/**
 * @brief Performance Counter (PCINT) Register.
 */
struct xapic_pcint {
    uint32_t vector : 8;          /* Performance Vector */
    uint32_t delivery_mode : 3;   /* Delivery Mode      */
    uint32_t : 1;                 /* Reserved           */
    uint32_t delivery_status : 1; /* Delivery Status    */
    uint32_t : 3;                 /* Reserved           */
    uint32_t mask : 1;            /* Mask               */
    uint32_t : 15;                /* Reserved           */
};

/**
 * @brief Local Interrupt (LINT) Register.
 */
struct xapic_lint {
    uint32_t vector : 8;          /* LINT Vector      */
    uint32_t delivery_mode : 3;   /* Delivery Mode    */
    uint32_t : 1;                 /* Reserved         */
    uint32_t delivery_status : 1; /* Delivery Status  */
    uint32_t polarity : 1;        /* Polarity         */
    uint32_t remote_irr : 1;      /* Remote IRR       */
    uint32_t trigger_mode : 1;    /* Trigger Mode     */
    uint32_t mask : 1;            /* Mask             */
    uint32_t : 15;                /* Reserved         */
};

/**
 * @brief Error Register.
 */
struct xapic_error {
    uint32_t vector : 8;          /* Error Vector     */
    uint32_t : 4;                 /* Reserved         */
    uint32_t delivery_status : 1; /* Delivery Status  */
    uint32_t : 3;                 /* Reserved         */
    uint32_t mask : 1;            /* Mask             */
    uint32_t : 15;                /* Reserved         */
};

/**
 * @brief Delivery mode for LVT registers.
 */
enum xapic_lvt_delivery_mode {
    XAPIC_LVT_FIXED = 0,  /* Fixed delivery mode.    */
    XAPIC_LVT_SMI = 2,    /* System management mode. */
    XAPIC_LVT_NMI = 4,    /* Non-maskable interrupt. */
    XAPIC_LVT_INIT = 5,   /* INIT mode.              */
    XAPIC_LVT_EXTINT = 7, /* External interrupt.     */
};

/**
 * @brief Delivery status for LVT registers.
 */
enum xapic_lvt_delivery_status {
    XAPIC_LVT_IDLE = 0,         /* Idle.         */
    XAPIC_LVT_SEND_PENDING = 1, /* Send pending. */
};

/**
 * @brief Delivery mode.
 */
enum xapic_icr_delivery_mode {
    XAPIC_ICR_FIXED_DELIVERY = 0,  /* Fixed delivery mode.    */
    XAPIC_ICR_LOWEST_PRIORITY = 1, /* Lowest priority mode.   */
    XAPIC_ICR_SMI = 2,             /* System management mode. */
    XAPIC_ICR_NMI = 4,             /* Non-maskable interrupt. */
    XAPIC_ICR_INIT = 5,            /* INIT mode.              */
    XAPIC_ICR_STARTUP = 6,         /* Startup mode.           */
    XAPIC_ICR_EXTINT = 7,          /* External interrupt.     */
};

/**
 * @brief Destination shorthand.
 */
enum xapic_icr_destination_shorthand {
    XAPIC_ICR_NO_SHORTHAND = 0,       /* No shorthand.       */
    XAPIC_ICR_SELF = 1,               /* Self.               */
    XAPIC_ICR_ALL_INCLUDING_SELF = 2, /* All including self. */
    XAPIC_ICR_ALL_EXCLUDING_SELF = 3, /* All excluding self. */
};

/**
 * @brief Delivery status.
 */
enum xapic_icr_delivery_status {
    XAPIC_ICR_IDLE = 0,         /* Idle.         */
    XAPIC_ICR_SEND_PENDING = 1, /* Send pending. */
};

/**
 * @brief xAPIC Interrupt Vectors.
 */
enum xapic_intvec {
    XAPIC_INTVEC_CMCI = 247,    /* CMCI interrupt vector.                */
    XAPIC_INTVEC_THERMAL = 248, /* Thermal Sensor interrupt vector.      */
    XAPIC_INTVEC_PCINT = 249,   /* Performance Counter interrupt vector. */
    XAPIC_INTVEC_TIMER = 250,   /* Timer interrupt vector.               */
    XAPIC_INTVEC_LINT1 = 251,   /* Local Interrupt 1 vector.             */
    XAPIC_INTVEC_LINT0 = 252,   /* Local Interrupt 0 vector.             */
    XAPIC_INTVEC_ERROR = 253,   /* Error interrupt vector.               */
    XAPIC_INTVEC_IPI = 254,     /* Inter-Processor Interrupt.            */
    XAPIC_INTVEC_SPURIOUS = 255 /* Spurious interrupt vector.            */
};

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Base address of xAPIC.
 */
static volatile uint32_t *xapic = NULL;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Reads a 32-bit value from the xAPIC.
 *
 * @param off Offset in the xAPIC.
 *
 * @returns The value read from the xAPIC.
 */
static void xapic_read(uintptr_t off, uint32_t *val)
{
    volatile uint32_t *reg = (volatile uint32_t *)((uintptr_t)xapic + off);
    *val = *reg;
}

/**
 * @brief Writes a 32-bit value to the xAPIC.
 *
 * @param off Offset in the xAPIC.
 * @param val Value to write.
 */
static void xapic_write(uintptr_t off, const uint32_t *value)
{
    volatile uint32_t *reg = (volatile uint32_t *)((uintptr_t)xapic + off);
    *reg = *value;
    // Wait for write to finish ID register.
    reg = (volatile uint32_t *)((uintptr_t)xapic + XAPIC_ID);
}

/**
 * @brief Initializes the xAPIC.
 */
static void _xapic_init(void)
{
    // Setup SVR
    const struct xapic_svr svr = {
        .vector = XAPIC_INTVEC_SPURIOUS,
        .apic_enabled = 1,
        .focus_checking = 0,
        .eoi_broadcast = 0,
    };
    xapic_write(XAPIC_SVR, (const uint32_t *)&svr);

    // Clear error status register (requires back-to-back writes).
    const struct xapic_esr esr = {0};
    xapic_write(XAPIC_ESR, (const uint32_t *)&esr);
    xapic_write(XAPIC_ESR, (const uint32_t *)&esr);

    // Ack any outstanding interrupts.
    const uint32_t zero = 0;
    xapic_write(XAPIC_EOI, &zero);

    // Send an Init Level De-Assert to synchronize arbitration ID's.
    const struct xapic_icrhi icrhi = {0};
    xapic_write(XAPIC_ICRHI, (const uint32_t *)&icrhi);
    struct xapic_icrlo icrlo = {
        .vector = XAPIC_INTVEC_IPI,
        .delivery_mode = XAPIC_ICR_INIT,
        .destination_mode = 0,
        .delivery_status = 0,
        .level = 0,
        .trigger_mode = 0,
        .destination_shorthand = XAPIC_ICR_ALL_INCLUDING_SELF,
    };
    xapic_write(XAPIC_ICRLO, (const uint32_t *)&icrlo);

    // Poll delivery status until it is set to zero.
    while (true) {
        xapic_read(XAPIC_ICRLO, (uint32_t *)&icrlo);
        if (icrlo.delivery_status == XAPIC_ICR_IDLE) {
            break;
        }
    }

    // Disable timer interrupt.
    const struct xapic_timer timer = {
        .vector = XAPIC_INTVEC_TIMER,
        .delivery_status = 0,
        .mask = 1,
        .mode = 0,
    };
    xapic_write(XAPIC_TIMER, (const uint32_t *)&timer);

    struct xapic_ver ver = {0};
    xapic_read(XAPIC_VER, (uint32_t *)&ver);

    // Check if performance counter register is suported.
    if (ver.max_lvt >= 4) {
        // It is, so disable performance counter interrupt.
        const struct xapic_pcint perf = {
            .vector = XAPIC_INTVEC_PCINT,
            .delivery_mode = 0,
            .delivery_status = 0,
            .mask = 1,
        };
        xapic_write(XAPIC_PCINT, (const uint32_t *)&perf);
    }

    // Check if thermal register is supported.
    if (ver.max_lvt >= 5) {
        // It is, so disable thermal interrupt.
        const struct xapic_thermal thermal = {
            .vector = XAPIC_INTVEC_THERMAL,
            .delivery_mode = 0,
            .delivery_status = 0,
            .mask = 1,
        };
        xapic_write(XAPIC_THERM, (const uint32_t *)&thermal);
    }

    // Check if CMCI register is supported.
    if (ver.max_lvt >= 6) {
        // It is, so disable CMCI interrupt.
        const struct xapic_cmci cmci = {
            .vector = XAPIC_INTVEC_CMCI,
            .delivery_mode = 0,
            .delivery_status = 0,
            .mask = 1,
        };
        xapic_write(XAPIC_CMCI, (const uint32_t *)&cmci);
    }

    // Disable local interrupt 0.
    const struct xapic_lint lint0 = {
        .vector = XAPIC_INTVEC_LINT0,
        .delivery_mode = 0,
        .delivery_status = 0,
        .polarity = 0,
        .remote_irr = 0,
        .trigger_mode = 0,
        .mask = 1,
    };
    xapic_write(XAPIC_LINT0, (const uint32_t *)&lint0);

    // Disable local interrupt 1.
    const struct xapic_lint lint1 = {
        .vector = XAPIC_INTVEC_LINT1,
        .delivery_mode = 0,
        .delivery_status = 0,
        .polarity = 0,
        .remote_irr = 0,
        .trigger_mode = 0,
        .mask = 1,
    };
    xapic_write(XAPIC_LINT1, (const uint32_t *)&lint1);

    // Disable error interrupt.
    const struct xapic_error error = {
        .vector = XAPIC_INTVEC_ERROR,
        .delivery_status = 0,
        .mask = 1,
    };
    xapic_write(XAPIC_ERROR, (const uint32_t *)&error);

    // Enable interrupts on the APIC (but not on the processor).
    const struct xapic_tpr tpr = {
        .priority_subclass = 0,
        .priority_class = 0,
    };
    xapic_write(XAPIC_TPR, (const uint32_t *)&tpr);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Acknowledges an interrupt request.
 */
void xapic_ack(int irq)
{
    UNUSED(irq);

    const uint32_t zero = 0;
    xapic_write(XAPIC_EOI, &zero);
}

/**
 * @details Initializes the xAPIC.
 */
int xapic_init(void)
{
    static bool initialized = false;

    KASSERT_SIZE(sizeof(struct xapic_id), XAPIC_ID_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_ver), XAPIC_VER_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_tpr), XAPIC_TPR_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_svr), XAPIC_SVR_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_esr), XAPIC_ESR_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_cmci), XAPIC_LVT_ENTRY_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_icrlo), XAPIC_ICRLO_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_icrhi), XAPIC_ICRHI_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_timer), XAPIC_LVT_ENTRY_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_thermal), XAPIC_LVT_ENTRY_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_pcint), XAPIC_LVT_ENTRY_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_lint), XAPIC_LVT_ENTRY_SIZE);
    KASSERT_SIZE(sizeof(struct xapic_error), XAPIC_LVT_ENTRY_SIZE);

    // Check if xAPIC is enabled.
    if (!apic_base_en_read()) {
        error("xapic is not enabled");
        return (-EAGAIN);
    }

    // Check for double initialization.
    if (initialized) {
        error("xapic already initialized");
        return (-EBUSY);
    }

    xapic = (uint32_t *)apic_base_base_read();

    // Print address.
    info("initializing xapic (base_address=%x)", xapic);

    _xapic_init();

    // Initialization completed.
    initialized = true;

    return (0);
}

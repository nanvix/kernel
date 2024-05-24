/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <stdint.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief IA32_APIC_BASE MSR (R/W)
 *
 * @details This register holds the APIC base address.
 */
#define MSR_IA32_APIC_BASE 0x0000001b

/**
 * @ingroup x86-cpu-msr-ia32-apic-base IA32_APIC_BASE
 */
/**@{*/
#define MSR_IA32_APIC_BASE_BSP_SHIFT 8
#define MSR_IA32_APIC_BASE_BSP_MASK (1llu << MSR_IA32_APIC_BASE_BSP_SHIFT)

#define MSR_IA32_APIC_BASE_EXTD_SHIFT 10
#define MSR_IA32_APIC_BASE_EXTD_MASK (1llu << MSR_IA32_APIC_BASE_EXTD_SHIFT)

#define MSR_IA32_APIC_BASE_EN_SHIFT 11
#define MSR_IA32_APIC_BASE_EN_MASK (1llu << MSR_IA32_APIC_BASE_EN_SHIFT)

#define MSR_IA32_APIC_BASE_BASE_SHIFT 12
#define MSR_IA32_APIC_BASE_BASE_MASK                                           \
    (0xfffffllu << MSR_IA32_APIC_BASE_BASE_SHIFT)
/**@}*/

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Reads IA32_APIC_BASE MSR.
 *
 * @return The contents of the IA32_APIC_BASE MSR.
 */
static uint64_t apic_base_read(void)
{
    return (rdmsr(MSR_IA32_APIC_BASE));
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Reads the BSP flag (Processor is BSP) in the IA32_APIC_BASE MSR.
 *
 * @returns The BSP flag in the IA32_APIC_BASE MSR.
 */
uint8_t apic_base_bsp_read(void)
{
    const uint64_t value = (apic_base_read() & MSR_IA32_APIC_BASE_BSP_MASK) >>
                           MSR_IA32_APIC_BASE_BSP_SHIFT;
    // NOTE: the following cast is safe because the BSP flag is a single bit.
    return ((uint8_t)value);
}

/**
 * @brief Reads the EXTD flag (Enable x2APIC Mode) in the IA32_APIC_BASE MSR.
 *
 * @returns The EXTD flag in the IA32_APIC_BASE MSR.
 */
uint8_t apic_base_extd_read(void)
{
    const uint64_t value = (apic_base_read() & MSR_IA32_APIC_BASE_EXTD_MASK) >>
                           MSR_IA32_APIC_BASE_EXTD_SHIFT;
    // NOTE: the following cast is safe because the EXTD flag is a single bit.
    return ((uint8_t)value);
}

/**
 * @brief Reads the EN flag (xAPIC Enable) in the IA32_APIC_BASE MSR.
 *
 * @returns The EN flag in the IA32_APIC_BASE MSR.
 */
uint8_t apic_base_en_read(void)
{
    const uint64_t value = (apic_base_read() & MSR_IA32_APIC_BASE_EN_MASK) >>

                           MSR_IA32_APIC_BASE_EN_SHIFT;
    // NOTE: the following cast is safe because the EN flag is a single bit.
    return ((uint8_t)value);
}

/**
 * @brief Reads the base address in the IA32_APIC_BASE MSR.
 *
 * @returns The base address in the IA32_APIC_BASE MSR.
 */
uint32_t apic_base_base_read(void)
{
    // NOTE: we do not need to shift because address is aligned to 4KB.
    const uint64_t value = (apic_base_read() & MSR_IA32_APIC_BASE_BASE_MASK);
    // NOTE: the following cast is safe because the base address is 32 bits.
    return ((uint32_t)value);
}

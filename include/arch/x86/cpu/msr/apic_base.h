/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_MSR_APIC_BASE_H_
#define ARCH_X86_CPU_MSR_APIC_BASE_H_

/**
 * @addtogroup x86-cpu-msr-apic-base IA32_APIC_BASE
 * @ingroup x86-cpu-msr
 *
 * @brief IA32_APIC_BASE MSR
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#ifndef _ASM_FILE_
#include <nanvix/cc.h>
#include <stdint.h>
#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Reads the BSP flag (Processor is BSP) in the IA32_APIC_BASE MSR.
 *
 * @returns The BSP flag in the IA32_APIC_BASE MSR.
 */
extern uint8_t apic_base_bsp_read(void);

/**
 * @brief Reads the EXTD flag (Enable x2APIC Mode) in the IA32_APIC_BASE
 * MSR.
 *
 * @returns The EXTD flag in the IA32_APIC_BASE MSR.
 */
extern uint8_t apic_base_extd_read(void);

/**
 * @brief Reads the EN flag (xAPIC Enable) in the IA32_APIC_BASE MSR.
 *
 * @returns The EN flag in the IA32_APIC_BASE MSR.
 */
extern uint8_t apic_base_en_read(void);

/**
 * @brief Reads the base address in the IA32_APIC_BASE MSR.
 *
 * @returns The base address in the IA32_APIC_BASE MSR.
 */
extern uint32_t apic_base_base_read(void);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_MSR_APIC_BASE_H_ */

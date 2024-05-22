/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_HAL_ARCH_X86_IOAPIC_H_
#define NANVIX_HAL_ARCH_X86_IOAPIC_H_

/**
 * @brief I/O APIC HAL Interface
 *
 * @addtogroup x86-ioapic I/O APIC
 * @ingroup x86
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#ifndef _ASM_FILE_
#include <stdint.h>
#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes an I/O APIC.
 *
 * @param id ID of the I/O APIC
 * @param addr Base address of the I/O APIC
 * @param gsi Global system interrupt number.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int ioapic_init(uint8_t id, uint32_t addr, uint32_t gsi);

/**
 * @brief Enables an interrupt line.
 *
 * @param irq Number of target interrupt line.
 * @param cpunum CPU number.
 */
extern int ioapic_enable(uint8_t irq, uint8_t cpunum);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_HAL_ARCH_X86_IOAPIC_H_ */

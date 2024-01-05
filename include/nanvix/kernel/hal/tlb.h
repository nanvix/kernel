/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_TLB_H_
#define NANVIX_KERNEL_HAL_TLB_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal/arch.h>

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @name TLB Types
 */
/**@{*/
#define TLB_INSTRUCTION 0 /** Instruction TLB */
#define TLB_DATA 1        /** Data TLB        */
/**@}*/

/**
 * @brief Gets the virtual address encoded in a TLB entry.
 *
 * @param tlbe Target TLB entry.
 *
 * @returns The virtual address encoded in the TLB entry pointed
 * to by @p tlbe.
 */
extern vaddr_t tlbe_vaddr_get(const struct tlbe *tlbe);

/**
 * @brief Gets the physical address encoded in a TLB entry.
 *
 * @param tlbe Target TLB entry.
 *
 * @returns The physical address encoded in the TLB entry pointed
 * to by @p tlbe.
 */
extern paddr_t tlbe_paddr_get(const struct tlbe *tlbe);

/**
 * @brief Assesses if a TLB entry is valid
 *
 * @param tlbe Target TLB entry.
 *
 * The tlbe_is_valid() function assess if a TLB entry
 * has the status bit valid.
 *
 * @return Non zero if is the TLB entry is valid, zero otherwise.
 */
extern int tlbe_is_valid(const struct tlbe *tlbe);

/**
 * @brief Gets the tlbe entry index in TLB.
 *
 * @param vaddr Target virtual address.
 *
 * @return Index of target entry in the TLB.
 */
extern unsigned tlbe_get_index(vaddr_t vaddr);

/**
 * @brief Writes a TLB entry.
 *
 * @param tlbe     The updated value of target TLB entry.
 * @param tlb_type Target TLB.
 * @param user     User adderss flag.
 * @param inst     Instruction flag.
 * @param vaddr    Target virtual address.
 * @param paddr    Target physical address.
 *
 * @return Zero if successfully writes a TLB entry,
 * non zero otherwise.
 */
extern int tlbe_write(struct tlbe *tlbe, int tlb_type, vaddr_t vaddr,
                      paddr_t paddr, int user, int inst);

/**
 * @brief Invalidates a TLB entry.
 *
 * @param tlbe  The updated value of target TLB entry.
 * @param tlb_type Target TLB.
 * @param vaddr Target virtual address.
 *
 * @return Zero if successfully writes a TLB entry,
 * non zero otherwise.
 */
extern int tlbe_inval(struct tlbe *tlbe, int tlb_type, vaddr_t vaddr);

/**
 * @brief Encodes a virtual address into the TLB.
 *
 * @param tlb_type Target TLB (D-TLB or I-TLB).
 * @param vaddr    Target virtual address.
 * @param paddr    Target physical address.
 * @param user     User address?
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int tlb_write(int tlb_type, vaddr_t vaddr, paddr_t paddr, int user);

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_TLB_H_ */

/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018-2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_OR1K_MMU_H_
#define ARCH_OR1K_MMU_H_

/**
 * @addtogroup or1k-mmu MMU
 * @ingroup or1k-memory
 *
 * @brief Memory Management Unit
 */
/**@{*/

	/**
	 * @name Provided Interface
	 *
	 * @cond or1k
	 */
	/**@{*/
	#define __frame_t            /**< Page Frame Number */
	#define __paddr_t            /**< Physical Address  */
	#define __vaddr_t            /**< Virtual Address   */
	#define __pde_clear_fn       /**< pde_clear()       */
	#define __pde_frame_get_fn   /**< pde_frame_get()   */
	#define __pde_frame_set_fn   /**< pde_frame_set()   */
	#define __pde_get_fn         /**< pde_get()         */
	#define __pde_is_present_fn  /**< pde_is_present()  */
	#define __pde_is_user_fn     /**< pde_is_user()     */
	#define __pde_is_write_fn    /**< pde_is_write()    */
	#define __pde_present_set_fn /**< pde_present_set() */
	#define __pde_user_set_fn    /**< pde_user_set()    */
	#define __pde_write_set_fn   /**< pde_write_set()   */
	#define __pte_clear_fn       /**< pde_clear()       */
	#define __pte_frame_set_fn   /**< pde_frame_set()   */
	#define __pte_get_fn         /**< pde_get()         */
	#define __pte_is_present_fn  /**< pde_is_present()  */
	#define __pte_is_user_fn     /**< pte_is_user()     */
	#define __pte_is_write_fn    /**< pte_is_write()    */
	#define __pte_present_set_fn /**< pte_preset_set()  */
	#define __pte_user_set_fn    /**< pte_user_set()    */
	#define __pte_write_set_fn   /**< pte_write_set()   */
	/**@}*/
	/**@endcond**/

	/**
	 * @name Page Shifts and Masks
	 */
	/**@{*/
	#define OR1K_PAGE_SHIFT  13                       /**< Page Shift       */
	#define OR1K_PGTAB_SHIFT 24                       /**< Page Table Shift */
	#define OR1K_PAGE_MASK   (~(OR1K_PAGE_SIZE - 1))  /**< Page Mask        */
	#define OR1K_PGTAB_MASK  (~(OR1K_PGTAB_SIZE - 1)) /**< Page Table Mask  */
	/**@}*/

	/**
	 * @name Size of Pages and Page Tables
	 */
	/**@{*/
	#define OR1K_PAGE_SIZE  (1 << OR1K_PAGE_SHIFT)  /**< Page Size                 */
	#define OR1K_PGTAB_SIZE (1 << OR1K_PGTAB_SHIFT) /**< Page Table Size           */
	#define OR1K_PTE_SIZE   4                       /**< Page Table Entry Size     */
	#define OR1K_PDE_SIZE   4                       /**< Page Directory Entry Size */
 	#define OR1K_PT_SIZE    4096                    /**< Page table size           */
	#define OR1K_PT_SHIFT   10                      /**< Page table shift          */
	/**@}*/

	/**
	 * @name Page Shifts
	 *
	 * @cond or1k
	 */
	/**@{*/
	#define PAGE_SHIFT  OR1K_PAGE_SHIFT  /**< Page Shift       */
	#define PGTAB_SHIFT OR1K_PGTAB_SHIFT /**< Page Table Shift */
	#define PAGE_MASK   OR1K_PAGE_MASK   /**< Page Mask        */
	#define PGTAB_MASK  OR1K_PGTAB_MASK  /**< Page Table Mask  */
	/**@}*/
	/**@endcond*/

	/**
	 * @name Size of Pages and Page Tables
	 *
	 * @cond or1k
	 */
	/**@{*/
	#define PAGE_SIZE  OR1K_PAGE_SIZE  /**< Page Size                 */
	#define PGTAB_SIZE OR1K_PGTAB_SIZE /**< Page Table Size           */
	#define PTE_SIZE   OR1K_PTE_SIZE   /**< Page Table Entry Size     */
	#define PDE_SIZE   OR1K_PDE_SIZE   /**< Page Directory Entry Size */
	/**@}*/
	/**@endcond*/
	
	/**
	 * Page table entry constants.
	 */
	#define PT_CC  0x1         /* Cache Coherency.       */
	#define PT_CI  0x2         /* Cache Inhibit.         */
	#define PT_WBC 0x4         /* Write-Back Cache.      */
	#define PT_WOM 0x8         /* Weakly-Ordered Memory. */
	#define PT_A   0x10        /* Accesed.               */
	#define PT_D   0x20        /* Dirty.                 */
	#define PT_PPI 0x1C0       /* Page Protection Index. */
	#define PT_L   0x200       /* Last.                  */
	#define PT_PPN 0xFFFFFC00  /* Physical Page Number.  */

	/**
	 * Page table new fields.
	 */
	#define PT_COW     PT_CC   /* Copy on write.     */
	#define PT_ZERO    PT_CI   /* Demand zero.       */
	#define PT_FILL    PT_WBC  /* Demand fill.       */
	#define PT_PRESENT PT_WOM  /* Present in memory. */

	/**
	 * Page Protection Index, offset.
	 */
	#define PT_PPI_OFFSET   0x6   /* PPI offset. */
	
	/**
	 * Page Protection Index, data.
	 */
	#define PT_PPI_USR_RD   0x40  /* Supervisor Read/Write, User: Read.       */
	#define PT_PPI_USR_WR   0x80  /* Supervisor Read/Write, User: Write.      */
	#define PT_PPI_USR_RDWR 0xC0  /* Supervisor Read/Write, User: Read/Write. */

	/**
	 * Page Protection Index, instruction.
	 */
	#define PT_PPI_USR_EX   0x80  /* User Execute.       */
	#define PT_PPI_SPV_EX   0x40  /* Supervisor Execute. */

#ifndef _ASM_FILE_
#define _ASM_FILE_
#endif

/**@}*/

#endif /* ARCH_OR1K_MMU_H_ */

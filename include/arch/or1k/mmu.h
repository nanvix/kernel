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

	#include <nanvix/klib.h>

	/**
	 * @brief Virtual address.
	 */
	typedef uint32_t vaddr_t;

	/**
	 * @brief Physical address.
	 */
	typedef uint32_t paddr_t;

	/**
	 * @brief Frame number.
	 */
	typedef uint32_t frame_t;

	/**
	 * @brief Page directory entry.
	 */
	struct pde
	{
		unsigned frame      : 22; /* Frame number.          */
		unsigned last       :  1; /* Last PTE.              */
		unsigned ppi        :  3; /* Page protection index. */
		unsigned dirty      :  1; /* Dirty?                 */
		unsigned accessed   :  1; /* Accessed?              */
		unsigned wom        :  1; /* Weakly-Ordered Memory. */
		unsigned wbc        :  1; /* Write-Back Cache.      */
		unsigned ci         :  1; /* Cache Inhibit.         */
		unsigned cc         :  1; /* Cache Coherency.       */
	};
	
	/**
	 * @brief Page table entry.
	 */
	struct pte
	{
		unsigned frame      : 22; /* Frame number.          */
		unsigned last       :  1; /* Last PTE.              */
		unsigned ppi        :  3; /* Page protection index. */
		unsigned dirty      :  1; /* Dirty?                 */
		unsigned accessed   :  1; /* Accessed?              */
		unsigned wom        :  1; /* Weakly-Ordered Memory. */
		unsigned wbc        :  1; /* Write-Back Cache.      */
		unsigned ci         :  1; /* Cache Inhibit.         */
		unsigned cc         :  1; /* Cache Coherency.       */
	};

	/**
	 * @brief Clears a page directory entry.
	 *
	 * @param pde Target page directory entry.
	 */
	static inline void pde_clear(struct pde *pde)
	{
		kmemset(pde, 0, PDE_SIZE);
	}

	/**
	 * @brief Sets the frame of a page table.
	 *
	 * @param pde Page directory entry of target page table.
	 * @param frame Frame number.
	 */
	static inline void pde_frame_set(struct pde *pde, frame_t frame)
	{
		pde->frame = frame;
	}

	/**
	 * @brief Sets/clears the present bit of a page table.
	 *
	 * @param pde Page directory entry of target page table.
	 * @param set Set bit?
	 */
	static inline void pde_present_set(struct pde *pde, int set)
	{
		((void)pde);
		((void)set);
	}

	/**
	 * @brief Asserts if the present bit of a page table is set.
	 *
	 * @param pde Page directory entry of target page table.
	 *
	 * @returns If the present bit of the target page table is set,
	 * non zero is returned. Otherwise, zero is returned instead.
	 */
	static inline int pde_is_present(struct pde *pde)
	{
		((void)pde);
		return (1);
	}

	/**
	 * @brief Gets the frame number of a page directory entry.
	 *
	 * @param pde Target page directory entry.
	 *
	 * @returns The frame number of the target page directory entry.
	 */
	static inline frame_t pde_frame_get(struct pde *pde)
	{
		return (pde->frame);
	}

	/**
	 * @brief Sets/clears the write bit of a page table.
	 *
	 * @param pde Page directory entry of target page table.
	 * @param set Set bit?
	 */
	static inline void pde_write_set(struct pde *pde, int set)
	{
		pde->ppi = (set) ? (PT_PPI_USR_RDWR >> PT_PPI_OFFSET)
			: (PT_PPI_USR_RD >> PT_PPI_OFFSET);
	}

	/**
	 * @brief Asserts if the write bit of a page table is set.
	 *
	 * @param pde Page directory entry of target page table.
	 *
	 * @returns If the write bit of the target page table is set, non
	 * zero is returned. Otherwise, zero is returned instead.
	 */
	static inline int pde_is_write(struct pde *pde)
	{
		return (pde->ppi == (PT_PPI_USR_RDWR >> PT_PPI_OFFSET));
	}

	/**
	 * @brief Sets/clears the user bit of a page table.
	 *
	 * @param pde Page directory entry of target page table.
	 * @param set Set bit?
	 */
	static inline void pde_user_set(struct pde *pde, int set)
	{
		pde->ppi = (set) ? (PT_PPI_USR_RD >> PT_PPI_OFFSET) : 0;
	}

	/**
	 * @brief Asserts if the user bit of a page table is set.
	 *
	 * @param pde Page directory entry of target page table.
	 *
	 * @returns If the user bit of the target page table is set, non
	 * zero is returned. Otherwise, zero is returned instead.
	 */
	static inline int pde_is_user(struct pde *pde)
	{
		return (pde->ppi & (PT_PPI_USR_RD >> PT_PPI_OFFSET));
	}

	/**
	 * @brief Clears a page table entry.
	 *
	 * @param pte Target page table entry.
	 */
	static inline void pte_clear(struct pte *pte)
	{
		kmemset(pte, 0, PTE_SIZE);
	}

	/**
	 * @brief Sets/clears the present bit of a page.
	 *
	 * @param pte Page table entry of target page.
	 * @param set Set bit?
	 */
	static inline void pte_present_set(struct pte *pte, int set)
	{
		((void)pte);
		((void)set);
	}

	/**
	 * @brief Asserts if the present bit of a page.
	 *
	 * @param pte Page table entry of target page.
	 *
	 * @returns If the present bit of the target page, non zero is
	 * returned. Otherwise, zero is returned instead.
	 */
	static inline int pte_is_present(struct pte *pte)
	{
		((void)pte);
		return (1);
	}

	/**
	 * @brief Sets the frame of a page.
	 *
	 * @param pte   Page table entry of target page.
	 * @param frame Frame number.
	 */
	static inline void pte_frame_set(struct pte *pte, frame_t frame)
	{
		pte->frame = frame;
	}

	/**
	 * @brief Gets the frame linked to page.
	 *
	 * @param pte   Page table entry of target page.
	 *
	 * @returns Number of the frame that is linked to the target page.
	 */
	static inline frame_t pte_frame_get(struct pte *pte)
	{
		return (pte->frame);
	}

	/**
	 * @brief Sets/clears the write bit of a page.
	 *
	 * @param pte Page table entry of target page.
	 * @param set Set bit?
	 */
	static inline void pte_write_set(struct pte *pte, int set)
	{
		pte->ppi = (set) ? (PT_PPI_USR_RDWR >> PT_PPI_OFFSET)
			: (PT_PPI_USR_RD >> PT_PPI_OFFSET);
	}

	/**
	 * @brief Asserts if the write bit of a page.
	 *
	 * @param pte Page table entry of target page.
	 *
	 * @returns If the write bit of the target page, non zero is
	 * returned. Otherwise, zero is returned instead.
	 */
	static inline int pte_is_write(struct pte *pte)
	{
		return (pte->ppi == (PT_PPI_USR_RDWR >> PT_PPI_OFFSET));
	}

	/**
	 * @brief Sets/clears the user bit of a page.
	 *
	 * @param pte Page table entry of target page.
	 * @param set Set bit?
	 */
	static inline void pte_user_set(struct pte *pte, int set)
	{
		pte->ppi = (set) ? (PT_PPI_USR_RD >> PT_PPI_OFFSET) : 0;
	}

	/**
	 * @brief Asserts if the user bit of a page.
	 *
	 * @param pte Page table entry of target page.
	 *
	 * @returns If the user bit of the target page, non zero is
	 * returned. Otherwise, zero is returned instead.
	 */
	static inline int pte_is_user(struct pte *pte)
	{
		return (pte->ppi & (PT_PPI_USR_RD >> PT_PPI_OFFSET));
	}

	/**
	 * @brief Gets the page table index of a page.
	 *
	 * @param vaddr Target virtual address.
	 *
	 * @returns Returns the page table index of the page that lies at
	 * address @p vaddr.
	 */
	static inline unsigned pte_idx_get(vaddr_t vaddr)
	{
		return (((unsigned)(vaddr) & (OR1K_PGTAB_MASK^OR1K_PAGE_MASK)) >> OR1K_PAGE_SHIFT);
	}

	/**
	 * @brief Gets the page directory index of a page.
	 *
	 * @param vaddr Target virtual address.
	 *
	 * @returns Returns the page directory index of the page that lies
	 * at address @p vaddr.
	 */
	static inline unsigned pde_idx_get(vaddr_t vaddr)
	{
		return ((unsigned)(vaddr) >> OR1K_PGTAB_SHIFT);
	}

	/**
	 * @brief Gets a page directory entry.
	 *
	 * @param pgdir Target page directory.
	 * @param vaddr  Target virtual address.
	 *
	 * @returns The requested page directory entry.
	 */
	static inline struct pde *pde_get(struct pde *pgdir, vaddr_t vaddr)
	{
		return (&pgdir[pde_idx_get(vaddr)]);
	}

	/**
	 * @brief Gets a page table entry.
	 *
	 * @param pgtab Target page table.
	 * @param vaddr Target virtual address.
	 *
	 * @returns The requested page table entry.
	 */
	static inline struct pte *pte_get(struct pte *pgtab, vaddr_t vaddr)
	{
		return (&pgtab[pte_idx_get(vaddr)]);
	}

#endif

/**@}*/

#endif /* ARCH_OR1K_MMU_H_ */

/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef ARCH_I386_MMU_H_
#define ARCH_I386_MMU_H_

/**
 * @addtogroup i386-mmu MMU
 * @ingroup i386-memory
 *
 * @brief Memory Management Unit
 */
/**@{*/

	/**
	 * @name Provided Interface
	 *
	 * @cond i386
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
	#define __pte_frame_get_fn   /**< pde_frame_get()   */
	#define __pte_frame_set_fn   /**< pde_frame_set()   */
	#define __pte_get_fn         /**< pde_get()         */
	#define __pte_is_present_fn  /**< pde_is_present()  */
	#define __pte_is_user_fn     /**< pte_is_user()     */
	#define __pte_is_write_fn    /**< pte_is_write()    */
	#define __pte_present_set_fn /**< pte_present_set() */
	#define __pte_user_set_fn    /**< pte_user_set()    */
	#define __pte_write_set_fn   /**< pte_write_set()   */
	/**@}*/
	/**@endcond**/

	/**
	 * @name Page Shifts and Masks
	 */
	/**@{*/
	#define I386_PAGE_SHIFT  12                       /**< Page Shift       */
	#define I386_PGTAB_SHIFT 22                       /**< Page Table Shift */
	#define I386_PAGE_MASK   (~(I386_PAGE_SIZE - 1))  /**< Page Mask        */
	#define I386_PGTAB_MASK  (~(I386_PGTAB_SIZE - 1)) /**< Page Table Mask  */
	/**@}*/

	/**
	 * @name Size of Pages and Page Tables
	 */
	/**@{*/
	#define I386_PAGE_SIZE  (1 << I386_PAGE_SHIFT)  /**< Page Size                 */
	#define I386_PGTAB_SIZE (1 << I386_PGTAB_SHIFT) /**< Page Table Size           */
	#define I386_PTE_SIZE   4                       /**< Page Table Entry Size     */
	#define I386_PDE_SIZE   4                       /**< Page Directory Entry Size */
	/**@}*/

	/**
	 * @name Page Shifts
	 *
	 * @cond i386
	 */
	/**@{*/
	#define PAGE_SHIFT  I386_PAGE_SHIFT  /**< Page Shift       */
	#define PGTAB_SHIFT I386_PGTAB_SHIFT /**< Page Table Shift */
	#define PAGE_MASK   I386_PAGE_MASK   /**< Page Mask        */
	#define PGTAB_MASK  I386_PGTAB_MASK  /**< Page Table Mask  */
	/**@}*/
	/**@endcond*/

	/**
	 * @name Size of Pages and Page Tables
	 *
	 * @cond i386
	 */
	/**@{*/
	#define PAGE_SIZE  I386_PAGE_SIZE  /**< Page Size                 */
	#define PGTAB_SIZE I386_PGTAB_SIZE /**< Page Table Size           */
	#define PTE_SIZE   I386_PTE_SIZE   /**< Page Table Entry Size     */
	#define PDE_SIZE   I386_PDE_SIZE   /**< Page Directory Entry Size */
	/**@}*/
	/**@endcond*/

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
		unsigned present  :  1; /*<< Present in memory? */
		unsigned writable :  1; /**< Writable page?     */
		unsigned user     :  1; /**< User page?         */
		unsigned          :  2; /**< Reserved.          */
		unsigned accessed :  1; /**< Accessed?          */
		unsigned dirty    :  1; /**< Dirty?             */
		unsigned          :  2; /**< Reserved.          */
		unsigned          :  3; /**< Unused.            */
		unsigned frame    : 20; /**< Frame number.      */
	};

	/**
	 * @brief Page table entry.
	 */
	struct pte
	{
		unsigned present  :  1; /**< Present in memory? */
		unsigned writable :  1; /**< Writable page?     */
		unsigned user     :  1; /**< User page?         */
		unsigned          :  2; /**< Reserved.          */
		unsigned accessed :  1; /**< Accessed?          */
		unsigned dirty    :  1; /**< Dirty?             */
		unsigned          :  2; /**< Reserved.          */
		unsigned          :  3; /**< Unused.            */
		unsigned frame    : 20; /**< Frame number.      */
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
		pde->present = (set) ? 1 : 0;
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
		return (pde->present);
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
		pde->writable = (set) ? 1 : 0;
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
		return (pde->writable);
	}

	/**
	 * @brief Sets/clears the user bit of a page table.
	 *
	 * @param pde Page directory entry of target page table.
	 * @param set Set bit?
	 */
	static inline void pde_user_set(struct pde *pde, int set)
	{
		pde->user = (set) ? 1 : 0;
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
		return (pde->user);
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
		pte->present = (set) ? 1 : 0;
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
		return (pte->present);
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
		pte->writable = (set) ? 1 : 0;
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
		return (pte->writable);
	}

	/**
	 * @brief Sets/clears the user bit of a page.
	 *
	 * @param pte Page table entry of target page.
	 * @param set Set bit?
	 */
	static inline void pte_user_set(struct pte *pte, int set)
	{
		pte->user = (set) ? 1 : 0;
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
		return (pte->user);
	}

	/*
	 * DESCRIPTION;
	 *   The PG() macro returns the page number where a given virtual address.
	 */

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
		return (((unsigned)(vaddr) & (I386_PGTAB_MASK^I386_PAGE_MASK)) >> I386_PAGE_SHIFT);
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
		return ((unsigned)(vaddr) >> I386_PGTAB_SHIFT);
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

#endif /* ARCH_I386_MMU_H_ */

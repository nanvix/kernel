/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef ARCH_K1B_ELF_H_
#define ARCH_K1B_ELF_H_

/**
 * @addtogroup k1b-elf ELF
 * @ingroup k1b
 *
 * @brief ELF Multibinary Sections
 */
/**@{*/

	#include <nanvix/const.h>

	/**
	 * @name Binary Sections
	 */
	/**@{*/
	#define SECTION_TEXT SECTION(".locked_text")        /**< Text section.              */
	#define SECTION_BINDESC SECTION(".locked.binaries") /**< Binary descriptor section. */
	/**@}*/

	/**
	 * @name Binary Symbols
	 */
	/**@{*/
	extern int _bin_start_frame;   /**< Binary start frame.     */
	extern int _bin_end_frame;     /**< Binary end frame.       */
#ifdef __k1io__
	extern int _ddr_frame_start;   /**< DDR start frame.        */
	extern int _ddr_frame_end;     /**< DDR end frame.          */
	extern int __dtb_debut_offset; /**< Device tree offset.     */
	extern int __dtb_size;         /**< Device tree Size.       */
#endif
	extern int _vstart;            /**< Hypervisor entry point. */
	extern int _scoreboard_offset; /**< Scoreboard offset.      */
	/**@}*/

/**@}*/

#endif /* ARCH_K1B_ELF_H_ */

/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_OR1K_CPU_H_
#define ARCH_OR1K_CPU_H_

/*============================================================================*
 *                             Processor Interface                            *
 *============================================================================*/

/**
 * @addtogroup or1k-cpu CPU
 * @ingroup or1k
 *
 * @brief or1k Processor
 */
/**@{*/

	/**
	 * @brief Number of cores in the or1k architecture.
	 */
	#define OR1K_NUM_CORES 2

#ifndef _ASM_FILE_
	/**
	 * @brief Gets the number of cores.
	 *
	 * The or1k_cpu_get_num_cores() gets the number of cores in the
	 * underlying or1k processor.
	 *
	 * @returns The the number of cores in the underlying processor.
	 */
	static inline int or1k_cpu_get_num_cores(void)
	{
		return (OR1K_NUM_CORES);
	}

/**@}*/

/*============================================================================*
 *                              Exported Interface                            *
 *============================================================================*/

/**
 * @cond or1k
 */

/**
 * @addtogroup kernel-hal-cpu Processor
 * @ingroup kernel-hal
 */
/**@{*/

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_cpu_get_num_cores
	/**@}*/

	/**
	 * @see or1k_cpu_get_num_cores()
	 */
	static inline int hal_cpu_get_num_cores(void)
	{
		return (or1k_cpu_get_num_cores());
	}

/**@}*/

#endif /* _ASM_FILE_ */

/**@endcond*/

#endif /* ARCH_OR1K_CPU_H_ */

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

#ifndef ARCH_I386_CPU_H_
#define ARCH_I386_CPU_H_

/*============================================================================*
 *                             Processor Interface                            *
 *============================================================================*/

/**
 * @addtogroup i386-cluster-cpu CPU
 * @ingroup i386-cluster
 *
 * @brief i386 Processor
 */
/**@{*/

	/**
	 * @brief Number of cores in the i386 architecture.
	 */
	#define I386_NUM_CORES 1

	/**
	 * @brief Gets the number of cores.
	 *
	 * The i386_cpu_get_num_cores() gets the number of cores in the
	 * underlying i386 processor.
	 *
	 * @returns The the number of cores in the underlying processor.
	 */
	static inline int i386_cpu_get_num_cores(void)
	{
		return (I386_NUM_CORES);
	}

/**@}*/

/*============================================================================*
 *                              Exported Interface                            *
 *============================================================================*/

/**
 * @cond i386
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_cpu_get_num_cores
	/**@}*/

	/**
	 * @brief Number of cores in a a CPU in the the IBM PC target.
	 */
	#define HAL_NUM_CORES I386_NUM_CORES

	/**
	 * @see i386_cpu_get_num_cores()
	 */
	static inline int hal_cpu_get_num_cores(void)
	{
		return (i386_cpu_get_num_cores());
	}

/**@endcond*/

#endif /* ARCH_I386_CPU_H_ */

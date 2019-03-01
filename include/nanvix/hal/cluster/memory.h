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

#ifndef NANVIX_HAL_CLUSTER_MEMORY_H_
#define NANVIX_HAL_CLUSTER_MEMORY_H_

	/* Cluster Interface Implementation */
	#include <nanvix/hal/cluster/_cluster.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Constants */
	#ifndef _UBASE_VIRT
	#error "_UBASE_VIRT not defined"
	#endif
	#ifndef _USTACK_ADDR
	#error "_USTACK_ADDR not defined"
	#endif
	#ifndef _KBASE_VIRT
	#error "_KBASE_VIRT not defined"
	#endif
	#ifndef _KPOOL_VIRT
	#error "_KPOOL_VIRT not defined"
	#endif
	#ifndef _KBASE_PHYS
	#error "_KBASE_PHYS not defined"
	#endif
	#ifndef _KPOOL_PHYS
	#error "_KPOOL_PHYS not defined"
	#endif
	#ifndef _UBASE_PHYS
	#error "_UBASE_PHYS not defined"
	#endif
	#ifndef _MEMORY_SIZE
	#error "_MEMORY_SIZE not defined"
	#endif
	#ifndef _KMEM_SIZE
	#error "_KMEM_SIZE not defined"
	#endif
	#ifndef _KPOOL_SIZE
	#error "_KPOOL_SIZE not defined"
	#endif
	#ifndef _UMEM_SIZE
	#error "_UMEM_SIZE not defined"
	#endif

/*============================================================================*
 * Memory Interface                                                           *
 *============================================================================*/

/**
 * @defgroup kernel-hal-cluster-mem Memory
 * @ingroup kernel-hal-cluster
 *
 * @brief Memory HAL Interface
 */

	#include <nanvix/const.h>

	/**
	 * @brief Memory size (in bytes).
	 */
	#define HAL_MEM_SIZE _HAL_MEM_SIZE

/**@}*/

#endif /* NANVIX_HAL_CLUSTER_MEMORY_H_ */

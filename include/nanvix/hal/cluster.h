/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef NANVIX_HAL_CLUSTER_H_
#define NANVIX_HAL_CLUSTER_H_

	#ifndef __NEED_HAL_CLUSTER
		#error "include <nanvix/hal/processor.h> instead"
	#endif

	/* Cluster Interface Implementation */
	#include <nanvix/hal/cluster/_cluster.h>

	#include <nanvix/hal/cluster/cpu.h>
	#include <nanvix/hal/cluster/io.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

/*============================================================================*
 * Cluster Interface                                                          *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-cluster Cluster
 * @ingroup kernel-hal
 *
 * @brief Cluster HAL Interface
 */
/**@{*/

/**@}*/

#endif /* NANVIX_HAL_CLUSTER_H_ */

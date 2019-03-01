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

#ifndef PROCESSOR_BOSTAN_NOC_H_
#define PROCESSOR_BOSTAN_NOC_H_

	/* Cluster API. */
	#include <arch/processor/bostan/_bostan.h>

/**
 * @addtogroup processor-bostan-noc NoC
 * @ingroup processor-bostan
 * 
 * @brief Bostan Processor NoC Interface
 */

	#include <arch/processor/bostan/noc/tag.h>
	#include <arch/processor/bostan/noc/node.h>

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond bostan
 */

	/**
	 * @name Provided Functions
	 */
	/**@{*/
	#define __noc_mailbox_tag  /**< noc_mailbox_tag()  */
	#define __noc_portal_tag   /**< noc_portal_tag()   */
	#define __noc_sync_tag     /**< noc_sync_tag()     */
	#define __noc_tag_is_valid /**< noc_tag_is_valid() */
	#define __hal_node
	#define __node_get_id   /**< node_get_id()  */
	#define __node_get_num  /**< node_get_num() */
	#define __hal_noc
	#define __noc_is_ionode0   /**< noc_is_ionode0()   */
	#define __noc_is_ionode1   /**< noc_is_ionode1()   */
	#define __noc_is_ionode    /**< noc_is_ionode()    */
	#define __noc_is_cnode     /**< noc_is_cnode()     */
	#define __noc_mailbox_tag  /**< noc_mailbox_tag()  */
	#define __noc_portal_tag   /**< noc_portal_tag()   */
	#define __noc_sync_tag     /**< noc_sync_tag()     */
	#define __noc_tag_is_valid /**< noc_tag_is_valid() */
	#define __nodes_convert /**< nodes_convert() */
	/**@}*/

	/**
	 * @see k1b_node_get_id()
	 */
	static inline int node_get_id(void)
	{
		return k1b_node_get_id();
	}

    /**
	 * @see k1b_node_get_num()
	 */
	static inline int node_get_num(int nodeid)
	{
		return k1b_node_get_num(nodeid);
	}

	/**
	 * @see k1b_nodes_convert()
	 */
	static inline int nodes_convert(int *_nodes, const int *nodes, int nnodes)
	{
		return k1b_nodes_convert(_nodes, nodes, nnodes);
	}

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	/**@}*/

	/**
	 * @name Number of NoC nodes attached to an IO device.
	 */
	/**@{*/
	#define HAL_NR_NOC_IONODES K1B_NR_NOC_IONODES
	/**@}*/

	/**
	 * @name Number of NoC nodes not attached to an IO device.
	 */
	/**@{*/
	#define HAL_NR_NOC_CNODES K1B_NR_NOC_CNODES
	/**@}*/

	/**
	 * @name Number of NoC nodes.
	 */
	/**@{*/
	#define HAL_NR_NOC_NODES K1B_NR_NOC_NODES
	/**@}*/

	/**
	 * @brief Hal NoC handler.
	 */
	typedef k1b_noc_handler_fn hal_noc_handler_fn;

	/**
	 * @see k1b_noc_is_ionode0()
	 */
	static inline int noc_is_ionode0(int nodeid)
	{
		return (k1b_noc_is_ionode0(nodeid));
	}

	/**
	 * @see k1b_noc_is_ionode1()
	 */
	static inline int noc_is_ionode1(int nodeid)
	{
		return (k1b_noc_is_ionode1(nodeid));
	}

	/**
	 * @see k1b_noc_is_ionode()
	 */
	static inline int noc_is_ionode(int nodeid)
	{
		return (k1b_noc_is_ionode(nodeid));
	}

	/**
	 * @see k1b_noc_is_cnode()
	 */
	static inline int noc_is_cnode(int nodeid)
	{
		return (k1b_noc_is_cnode(nodeid));
	}

/**@endcond*/

#endif /* PROCESSOR_BOSTAN_NOC_H_ */


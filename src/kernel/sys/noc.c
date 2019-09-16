/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

#include <nanvix/hal/hal.h>

/*============================================================================*
 * kernel_node_get_num()                                                      *
 *============================================================================*/

/**
 * @brief The kernel_node_get_num() function gets the NoC Node Number attached
 * with the core.
 *
 * @param coreid Attached Core ID.
 *
 * @return Positive number with successfully get the node number, negative
 * otherwise.
 *
 * @author João Vicente Souto
 */
PUBLIC int kernel_node_get_num(int coreid)
{
	/* Invalid core ID. */
	if (!WITHIN(coreid, 0, CORES_NUM))
		return (-EINVAL);

    return (processor_node_get_num(coreid));
}

/*============================================================================*
 * kernel_node_set_num()                                                      *
 *============================================================================*/

/**
 * @brief The kernel_node_set_num() function exchange the NoC Node Number
 * attached with the core.
 *
 * @param coreid  Attached Core ID.
 * @param nodenum New NoC Node Number.
 *
 * @return Zero if successfully exchange the node number, non zero otherwise.
 *
 * @author João Vicente Souto
 */
PUBLIC int kernel_node_set_num(int coreid, int nodenum)
{
	/* Invalid core ID. */
	if (!WITHIN(coreid, 0, CORES_NUM))
		return (-EINVAL);

	/* Invalid NoC node number. */
	if (!WITHIN(nodenum, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	return (processor_node_set_num(coreid, nodenum));
}

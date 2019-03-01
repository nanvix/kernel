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

#ifndef PROCESSOR_BOSTAN_NOC_TAG_H_
#define PROCESSOR_BOSTAN_NOC_TAG_H_

	/* Cluster API. */
	#include <arch/processor/bostan/_bostan.h>

	/**
	 * @name Type of operation of the NoC Tag
	 */
	/**@{*/
	#define K1B_NOC_RX_TYPE 0 /**< Receiver tag. */
	#define K1B_NOC_TX_TYPE 1 /**< Transfer tag. */
	/**@}*/

	/**
	 * @name Type of C-NoC Tag
	 */
	/**@{*/
	#define K1B_CNOC_TAG_RX 0 /**< C-NoC receive tag.  */
	#define K1B_CNOC_TAG_TX 1 /**< C-NoC transfer tag. */
	/**@}*/

	/**
	 * @name Type of D-NoC Tag
	 */
	/**@{*/
	#define K1B_DNOC_TAG_RX 2 /**< D-NoC receive tag.  */
	#define K1B_DNOC_TAG_TX 3 /**< D-NoC transfer tag. */
	#define K1B_DNOC_TAG_UC 4 /**< D-NoC ucore tag.    */
	/**@}*/

	/**
	 * @brief Number of reserved tags.
	 */
	#define K1B_NR_RESERVED_TAGS 0

	/**
	 * @name Number of C-NoC buffer
	 */
	/**@{*/
	#define K1B_NR_CNOC_RX 128 - K1B_NR_RESERVED_TAGS /**< Number of receive buffers.  */
	#define K1B_NR_CNOC_TX 	 1	                      /**< Number of transfer buffers. */
	/**@}*/

	/**
	 * @name Number of D-NoC buffer
	 */
	/**@{*/
	#define K1B_NR_DNOC_RX 256 - K1B_NR_RESERVED_TAGS /**< Number of receive buffers.  */
	#define K1B_NR_DNOC_TX 	 8 - K1B_NR_RESERVED_TAGS /**< Number of transfer buffers. */
	#define K1B_NR_DNOC_UC 	 8 - K1B_NR_RESERVED_TAGS /**< Number of ucore threads.	   */
	/**@}*/

	/**
	 * @name C-NoC tag offset
	 */
	/**@{*/
	#define K1B_CNOC_RX_OFFSET 0 /**< Receive tag offset.  */
	#define K1B_CNOC_TX_OFFSET 0 /**< Transfer tag offset. */
	/**@}*/

#endif /* PROCESSOR_BOSTAN_NOC_TAG_H_ */


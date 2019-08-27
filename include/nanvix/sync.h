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

/**
 * @defgroup kernel-sync Synchronization System
 * @ingroup kernel
 *
 * @brief Synchronization System
 */

#ifndef NANVIX_SYNC_H_
#define NANVIX_SYNC_H_

	/* External dependencies. */
	#include <nanvix/hal/hal.h>
	#include <nanvix/const.h>

/*============================================================================*
 *                           Synchronization System                           *
 *============================================================================*/

	/**
	 * @brief Creates a synchronization point.
	 *
	 * @param nodes  Logic IDs of Target Nodes.
	 * @param nnodes Number of Target Nodes.
	 * @param type   Type of synchronization point.
	 *
	 * @returns Upon successful completion, the ID of the newly created
	 * synchronization point is returned. Upon failure, a negative error
	 * code is returned instead.
	 */
	EXTERN int _sync_create(const int * nodes, int nnodes, int type);

	/**
	 * @brief Opens a synchronization point.
	 *
	 * @param nodes  Logic IDs of Target Nodes.
	 * @param nnodes Number of Target Nodes.
	 * @param type   Type of synchronization point.
	 *
	 * @returns Upon successful completion, the ID of the target
	 * synchronization point is returned. Upon failure, a negative error
	 * code is returned instead.
	 *
	 * @todo Check for Invalid Remote
	 */
	EXTERN int _sync_open(const int *nodes, int nnodes, int type);

		/**
	 * @brief Destroys a synchronization point.
	 *
	 * @param syncid ID of the target synchronization point.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int _sync_unlink(int syncid);

	/**
	 * @brief Closes a synchronization point.
	 *
	 * @param syncid ID of the target synchronization point.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int _sync_close(int syncid);

	/**
	 * @brief Waits on a synchronization point.
	 *
	 * @param syncid ID of the target synchronization point.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int _sync_wait(int syncid);

	/**
	 * @brief Signals Waits on a synchronization point.
	 *
	 * @param syncid ID of the target synchronization point.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int _sync_signal(int syncid);

#endif /* NANVIX_SYNC_H_ */

/**@}*/

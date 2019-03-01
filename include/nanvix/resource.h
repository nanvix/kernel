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
 * @defgroup kernel-resource Resource
 * @ingroup kernel
 *
 * @brief Resource abstraction
 */

#ifndef NANVIX_RESOURCE_H_
#define NANVIX_RESOURCE_H_

	#include <stddef.h>

	/**
	 * @brief Resource.
	 */
	struct resource
	{
		int flags; /**< Flags. */
	};

	/**
	 * @brief Resource pool.
	 */
	struct resource_pool
	{
		void *resources;	  /**< Pool of resources.       */
		int nresources;		  /**< Number of resources.     */
		size_t resource_size; /**< Resource size (in byes). */
	};

	/**
	 * @brief Resource allocation interface.
	 */
	/**@{*/
	typedef int (*alloc_fn)(const struct resource_pool *);
	typedef void (*free_fn)(const struct resource_pool *, int);
	/**@}*/

	/* Forward definitions. */
	extern void resource_set_used(struct resource *);
	extern void resource_set_unused(struct resource *);
	extern void resource_set_busy(struct resource *);
	extern void resource_set_notbusy(struct resource *);
	extern void resource_set_rdonly(struct resource *);
	extern void resource_set_wronly(struct resource *);
	extern void resource_set_rdwr(struct resource *);
	extern void resource_set_async(struct resource *);
	extern void resource_set_sync(struct resource *);
	extern int resource_is_used(const struct resource *);
	extern int resource_is_busy(const struct resource *);
	extern int resource_is_readable(const struct resource *);
	extern int resource_is_rdonly(const struct resource *);
	extern int resource_is_writable(const struct resource *);
	extern int resource_is_wronly(const struct resource *);
	extern int resource_is_async(const struct resource *);
	extern int resource_is_sync(const struct resource *);

	/* Forward definitions. */
	extern alloc_fn resource_alloc;
	extern free_fn resource_free;

#endif /** NANVIX_RESOURCE_H_ */

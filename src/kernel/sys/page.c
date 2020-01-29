/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

#include <nanvix/kernel/thread.h>
#include <nanvix/kernel/mm.h>

/**
 * @todo TODO: provide a detailed description for this function.
 */
PUBLIC int kernel_upage_alloc(vaddr_t vaddr)
{
	/* Bad user address. */
	if (vaddr & ~PAGE_MASK)
		return (-EINVAL);

	/* Invalid user address. */
	if (!mm_is_uaddr(vaddr))
		return (-EFAULT);

	return (upage_alloc(root_pgdir, vaddr));
}

/**
 * @todo TODO: provide a detailed description for this function.
 */
PUBLIC int kernel_upage_free(vaddr_t vaddr)
{
	/* Bad user address. */
	if (vaddr & ~PAGE_MASK)
		return (-EINVAL);

	/* Invalid user address. */
	if (!mm_is_uaddr(vaddr))
		return (-EFAULT);

	return (upage_free(root_pgdir, vaddr));
}

/**
 * @todo TODO: provide a detailed description for this function.
 */
PUBLIC int kernel_upage_map(vaddr_t vaddr, frame_t frame)
{
	/* Bad user address. */
	if (vaddr & ~PAGE_MASK)
		return (-EINVAL);

	/* Invalid user address. */
	if (!mm_is_uaddr(vaddr))
		return (-EFAULT);

	/* Invalid frame. */
	if (!frame_is_allocated(frame))
		return (-EFAULT);

	return (upage_map(root_pgdir, vaddr, frame));
}


/**
 * todo TODO: provide a detailed description for this function.
 */
PUBLIC int kernel_upage_unmap(vaddr_t vaddr)
{
	/* Bad user address. */
	if (vaddr & ~PAGE_MASK)
		return (-EINVAL);

	/* Invalid user address. */
	if (!mm_is_uaddr(vaddr))
		return (-EFAULT);

	return ((upage_unmap(root_pgdir, vaddr) == FRAME_NULL) ? -EAGAIN : 0);
}

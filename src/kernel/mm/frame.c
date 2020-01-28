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

#include <nanvix/hal.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/const.h>
#include <nanvix/hlib.h>
#include <posix/errno.h>

/**
 * @brief Reference count for page frames.
 */
PRIVATE int frames[NUM_UFRAMES] = {0, };

/*============================================================================*
 * frame_is_allocated()                                                       *
 *============================================================================*/

/**
 * The frame_is_allocated() function asserts if the target page frame
 * @p frame is allocated.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int frame_is_allocated(frame_t frame)
{
	/* Invalid page frame. */
	if (!frame_is_valid_num(frame))
		return (0);

	return ((frames[frame_num_to_id(frame)] >= 0));
}

/*============================================================================*
 * frame_alloc()                                                              *
 *============================================================================*/

/**
 * The frame_alloc() function searches sequentially, for a free frame
 * to be allocated. The first free frame is allocated (if any) and its
 * number is returned.
 *
 * @retval FRAME_NULL Cannot allocate another frame.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC frame_t frame_alloc(void)
{
	/* Search for a free frame. */
	for (frame_t i = 0; i < NUM_UFRAMES; i++)
	{
		/* Found it. */
		if (frames[i] == 0)
		{
			frames[i] = 1;
			dcache_invalidate();

			return (frame_id_to_num(i));
		}
	}

	kprintf("[kernel][mm] page frame overflow");

	return (FRAME_NULL);
}

/*============================================================================*
 * frame_free()                                                               *
 *============================================================================*/

/**
 * The frame_free() function frees a previously allocated page frame
 * whose number equals to @p frame.
 *
 * @retval -EINVAL Invalid frame number
 * @retval -EFAULT Page frame not allocated
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int frame_free(frame_t frame)
{
	if (!frame_is_valid_num(frame))
		return (-EINVAL);

	if (frames[frame_num_to_id(frame)] == 0)
	{
		kprintf("[kernel][mm] double free on page frame %x", frame);
		return (-EFAULT);
	}

	frames[frame_num_to_id(frame)]--;
	dcache_invalidate();

	return (0);
}

/*============================================================================*
 * frame_init()                                                               *
 *============================================================================*/

/**
 * The frame_init() function initializes internal structures of the
 * page frame allocator. Additionally, if the kernel is compiled
 * without the @p NDEBUG build option, unit tests on the page frame
 * allocator are launched once its initialization is completed.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void frame_init(void)
{
	kprintf("[kernel][mm] initializing the page frame allocator");

#ifndef __NANVIX_FAST_BOOT
	for (frame_t i = 0; i < NUM_UFRAMES; i++)
		frames[i] = 0;
#endif

	#ifndef __SUPPRESS_TESTS
		kprintf("[kernel][mm] running tests on the page frame allocator");
		frame_test_driver();
	#endif
}

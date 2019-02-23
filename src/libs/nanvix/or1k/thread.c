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

#include <nanvix.h>
#include <errno.h>

/*============================================================================*
 * kthread_self()                                                             *
 *============================================================================*/

/*
 * @see sys_kthread_get_id()
 */
kthread_t kthread_self(void)
{
	return (-1);
}

/*============================================================================*
 * kthread_create()                                                           *
 *============================================================================*/

/*
 * @see sys_kthread_create()
 */
int kthread_create(
	kthread_t *tid,
	void *(*start)(void*),
	void *arg
)
{
	((void) tid);
	((void) start);
	((void) arg);

	return (-1);
}

/*============================================================================*
 * kthread_exit()                                                             *
 *============================================================================*/

/*
 * @see sys_kthread_exit().
 */
int kthread_exit(void *retval)
{
	((void) retval);
	
	return (-1);
}

/*============================================================================*
 * kthread_join()                                                             *
 *============================================================================*/

/*
 * @see sys_kthread_join()
 */
int kthread_join(
	kthread_t tid,
	void **retval
)
{
	((void) tid);
	((void) retval);

	return (-1);
}

/*============================================================================*
 * sleep()                                                                    *
 *============================================================================*/

/*
 * @see sys_sleep()
 */
int sleep(void)
{
	return (-1);
}

/*============================================================================*
 * wakeup()                                                                   *
 *============================================================================*/

/*
 * @see sys_wakeup()
 */
int wakeup(kthread_t tid)
{
	((void) (tid));

	return (-1);
}

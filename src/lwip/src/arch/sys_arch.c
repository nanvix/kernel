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

#if defined(__qemu_x86__) || defined(__qemu_openrisc__)

#include <lwip/opt.h>

#if !NO_SYS
#include <arch/sys_arch.h>
#include <nanvix/thread.h>
#include <lwip/arch.h>
#include <lwip/err.h>

#include <nanvix/hal/hal.h>
#endif

extern int lwip_now;
/**
 * @return the time elapsed since the beginning of the programm in ms (1/30s precision).
 */
u32_t sys_now(void)
{
	/* 30 ticks by second, conversion in ms */
	return lwip_now * (100.0 / 3.0);
}

#if !NO_SYS
/* Work in progress */
PRIVATE spinlock_t prot_lock;

void sys_init(void)
{
	spinlock_init(&prot_lock);
}

/*============================================================================*
 * Semaphores                                                                 *
 *============================================================================*/
void sys_sem_new(sys_sem_t *sem, u8_t count)
{
	LWIP_UNUSED_ARG(sem);
	LWIP_UNUSED_ARG(count);
	// semaphore_init(sem, count);
}

void sys_sem_signal(sys_sem_t *sem)
{
	LWIP_UNUSED_ARG(sem);
	// semaphore_up(sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	LWIP_UNUSED_ARG(sem);
	UNUSED(timeout); /* Is this the right thing to do ? */
	// semaphore_down(sem);
	return 0;
}

void sys_sem_free(sys_sem_t *sem)
{
   	LWIP_UNUSED_ARG(sem);
	/* Do nothing */
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
   	LWIP_UNUSED_ARG(sem);
	// sem = NULL; /* unsure */
}

/*============================================================================*
 * Mutex                                                                      *
 *============================================================================*/
void sys_mutex_new(sys_mutex_t *mutex)
{
	LWIP_UNUSED_ARG(mutex);
	// nanvix_mutex_init(mutex);
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
	LWIP_UNUSED_ARG(mutex);
	// nanvix_mutex_lock(mutex);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
	LWIP_UNUSED_ARG(mutex);
	// nanvix_mutex_unlock(mutex);
}

void sys_mutex_free(sys_mutex_t *mutex)
{
	LWIP_UNUSED_ARG(mutex);
	/* Do nothing */
}

void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
	LWIP_UNUSED_ARG(mutex);
	// mutex = NULL; /* unsure */
}

/*============================================================================*
 * Thread                                                                     *
 *============================================================================*/
sys_thread_t sys_thread_new(char *name, void (*thread)(void *arg),
							void *arg, int stacksize, int prio)
{
	UNUSED(name);
	UNUSED(stacksize);
	UNUSED(prio);
	LWIP_UNUSED_ARG(thread);
	LWIP_UNUSED_ARG(arg);

	// int res = 0;
	// thread_create(&res, thread, arg);
	// return res;
	return 0;
}

/*============================================================================*
 * Mailboxes                                                                  *
 *============================================================================*/

/*   Creates an empty mailbox for maximum "size" elements. Elements stored
  in mailboxes are pointers. You have to define macros "_MBOX_SIZE"
  in your lwipopts.h, or ignore this parameter in your implementation
  and use a default size.
  If the mailbox has been created, ERR_OK should be returned. Returning any
  other error will provide a hint what went wrong, but except for assertions,
  no real error handling is implemented. */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
	LWIP_UNUSED_ARG(mbox);
	LWIP_UNUSED_ARG(size);
	return 0;
}

/*   Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified. */
void sys_mbox_free(sys_mbox_t *mbox)
{
	LWIP_UNUSED_ARG(mbox);
}

/*   Posts the "msg" to the mailbox. This function have to block until
  the "msg" is really posted. */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	LWIP_UNUSED_ARG(mbox);
	LWIP_UNUSED_ARG(msg);
}

/*   Try to post the "msg" to the mailbox. Returns ERR_MEM if this one
  is full, else, ERR_OK if the "msg" is posted. */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	LWIP_UNUSED_ARG(mbox);
	LWIP_UNUSED_ARG(msg);
	return 0;
}

/*   Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). If "timeout" is 0, the thread should
  be blocked until a message arrives. The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped. */
/*   The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.  */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	LWIP_UNUSED_ARG(mbox);
	LWIP_UNUSED_ARG(msg);
	LWIP_UNUSED_ARG(timeout);
	return 0;
}

/*   This is similar to sys_arch_mbox_fetch, however if a message is not
  present in the mailbox, it immediately returns with the code
  SYS_MBOX_EMPTY. On success 0 is returned.

  To allow for efficient implementations, this can be defined as a
  function-like macro in sys_arch.h instead of a normal function. For
  example, a naive implementation could be:
	#define sys_arch_mbox_tryfetch(mbox,msg) \
	  sys_arch_mbox_fetch(mbox,msg,1)
  although this would introduce unnecessary delays. */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	LWIP_UNUSED_ARG(mbox);
	LWIP_UNUSED_ARG(msg);
	return 0;
}

/*   Returns 1 if the mailbox is valid, 0 if it is not valid.
  When using pointers, a simple way is to check the pointer for != NULL.
  When directly using OS structures, implementing this may be more complex.
  This may also be a define, in which case the function is not prototyped. */
/* int sys_mbox_valid(sys_mbox_t *mbox) {} */

/*   Invalidate a mailbox so that sys_mbox_valid() returns 0.
  ATTENTION: This does NOT mean that the mailbox shall be deallocated:
  sys_mbox_free() is always called before calling this function!
  This may also be a define, in which case the function is not prototyped. */
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	LWIP_UNUSED_ARG(mbox);
}

/*============================================================================*
 * Protect                                                                    *
 *============================================================================*/
sys_prot_t sys_arch_protect(void)
{
	// spinlock_lock(&prot_lock); /* Might have to disable interrupt also */
	return ERR_OK;
}

void sys_arch_unprotect(sys_prot_t pval)
{
	LWIP_UNUSED_ARG(pval);
	// spinlock_unlock(&prot_lock); /* Migh have to enable interrupt also */
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
	UNUSED(mbox);
	return 0;
}

int sys_sem_valid(sys_mbox_t *mbox)
{
	UNUSED(mbox);
	return 0;
}

err_t
sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
	UNUSED(q);
	UNUSED(msg);
	return 0;
}
#endif /* !NO_SYS */

#else
#include <lwip/opt.h>
u32_t sys_now(void)
{
	return 0;
}
#endif /* defined(__qemu_x86__) || defined(__qemu_openrisc__) */
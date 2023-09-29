/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum number of kernel threads.
 */
#define KTHREAD_MAX 16

/**
 * @brief Thread quantum.
 */
#define KTHREAD_QUANTUM 1

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Thread table.
 */
static struct thread threads[KTHREAD_MAX];

/**
 * @brief Running thread.
 */
static struct thread *running = NULL;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Releases all resources that are used by a thread.
 *
 * @param thread Target thread.
 */
static void thread_free(struct thread *thread)
{
    thread->tid = 0;
    thread->state = THREAD_NOT_STARTED;
    thread->arg = NULL;
    thread->start = NULL;
    kpage_put(thread->stack);
}

/**
 * @brief Runs the calling thread.
 *
 * @return This function does not return.
 */
noreturn static void thread_run(void)
{
    running->retval = running->start(running->arg);
    thread_exit();
    UNREACHABLE();
}

/**
 * @brief Handles a timer interrupt.
 */
static void do_timer(void)
{
    if (running->quantum++ >= KTHREAD_QUANTUM) {
        thread_yield();
    }
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function returns a pointer to the thread
 * that is running in the underlying core.
 */
struct thread *thread_get_curr(void)
{
    return (running);
}

/**
 * @details Creates a new thread.
 */
tid_t thread_create(void *(*start)(void *), void *arg)
{
    static tid_t next_tid = 0;
    struct thread *thread = NULL;

    // Find a thread control block that is not in use.
    for (int i = 0; i < KTHREAD_MAX; i++) {
        if (threads[i].state == THREAD_NOT_STARTED) {
            thread = &threads[i];
            break;
        }
    }

    // No thread available.
    if (thread == NULL) {
        return (-1);
    }

    // Initializes thread control block.
    thread->tid = ++next_tid;
    thread->age = 1;
    thread->state = THREAD_READY;
    thread->arg = arg;
    thread->start = start;
    thread->stack = kpage_get(true);
    context_create(&thread->ctx, thread->stack, thread_run);

    return (thread->tid);
}

/**
 * @details Yields the CPU.
 */
void thread_yield(void)
{
    running->age = 0;
    running->state = THREAD_READY;

    struct thread *prev = running;
    struct thread *next = prev;

    /* Selects the next thread to run. */
    for (int i = 0; i < KTHREAD_MAX; i++) {
        if (threads[i].state == THREAD_READY) {
            if (threads[i].age++ >= next->age) {
                next = &threads[i];
            }
        }
    }

    running = next;
    running->age = 0;
    running->quantum = 0;
    running->state = THREAD_RUNNING;

    __context_switch(&prev->ctx, &next->ctx);
}

/**
 * @brief Terminates the calling thread.
 */
noreturn void thread_exit(void)
{
    running->state = THREAD_TERMINATED;
    thread_free(running);
    thread_yield();
    UNREACHABLE();
}

/**
 * @details This function atomically puts the calling thread to
 * sleep.  Before sleeping, the spinlock pointed to by @p lock is
 * released.  The calling thread resumes its execution when another
 * thread invokes thread_wakeup() on it. When the thread wakes up, the
 * spinlock @p lock is re-acquired.
 */
void thread_sleep(spinlock_t *lock)
{
    spinlock_unlock(lock);
    thread_yield();
    spinlock_lock(lock);
}
/**
 * @details This function wakes up the thread pointed to by @p thread.
 */
void thread_wakeup(struct thread *t)
{
    t->state = THREAD_READY;
}

/**
 * @details Initializes the thread system.
 */
void thread_init(const void *root_pgdir)
{
    kprintf("[kernel][pm] initializing thread system...");

    // Initializes the table of threads.
    for (int i = 0; i < KTHREAD_MAX; i++) {
        threads[i].age = 0;
        threads[i].state = THREAD_NOT_STARTED;
        threads[i].arg = NULL;
        threads[i].start = NULL;
    }
    threads[0].state = THREAD_RUNNING;

    running = &threads[0];

    interrupt_register(INTERRUPT_TIMER, do_timer);
}

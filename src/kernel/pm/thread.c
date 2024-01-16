/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/mm/kpool.h>
#include <nanvix/kernel/pm/process.h>
#include <nanvix/kernel/pm/thread.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Thread quantum.
 */
#define THREAD_QUANTUM 100

/**
 * @brief Kernel main thread.
 */
#define KERNEL_THREAD 0

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Thread table.
 */
static struct thread threads[THREADS_MAX];

/**
 * @brief Running thread.
 */
static struct thread *running = &threads[KERNEL_THREAD];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @details Checks if there is an entry avaible in the thread table.
 */
static tid_t thread_alloc(void)
{
    tid_t tid;

    for (tid = 0; tid < THREADS_MAX; tid++) {
        if (threads[tid].state == THREAD_AVAILABLE) {
            threads[tid].state = THREAD_STARTED;
            return (tid);
        }
    }

    return (-EAGAIN);
}

/**
 * @details Handles a timer interrupt.
 */
static void do_timer(void)
{
    if (running->quantum++ >= THREAD_QUANTUM) {
        thread_yield();
    }
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Initializes the thread system.
 */
void thread_init(void)
{
    // Initializes the table of threads.
    for (int i = 0; i < THREADS_MAX; i++) {
        threads[i].state = THREAD_AVAILABLE;
        threads[i].pid = -1;
    }

    threads[KERNEL_THREAD].state = THREAD_RUNNING;
    threads[KERNEL_THREAD].quantum = 0;
    threads[KERNEL_THREAD].pid = KERNEL_PROCESS;
    threads[KERNEL_THREAD].age = 1;
    threads[KERNEL_THREAD].stack = NULL;

    interrupt_register(INTERRUPT_TIMER, do_timer);
}

/**
 * @details Creates a new thread.
 */
tid_t thread_create(pid_t pid, bool root)
{
    tid_t tid;
    if ((tid = thread_alloc()) < 0) {
        goto error0;
    }

    threads[tid].tid = tid;
    threads[tid].pid = pid;
    threads[tid].age = 1;
    threads[tid].state = THREAD_READY;
    threads[tid].quantum = 0;
    // TODO: Alloc user stack.
    threads[tid].stack = NULL;

    if (!root) {
        return tid;
    }

    void *kstack = kpage_get(true);
    if (kstack == NULL) {
        goto error1;
    }

    threads[tid].stack = kstack;

    return tid;

error1:
    threads[tid].pid = -1;
    threads[tid].state = THREAD_AVAILABLE;
error0:
    return -1;
}

/**
 * @details Releases a thread entry.
 */
int thread_free(tid_t tid)
{
    if (tid <= KERNEL_THREAD || tid >= THREADS_MAX) {
        return (-EINVAL);
    }

    KASSERT(kpage_put(threads[tid].stack) == 0);
    threads[tid].pid = -1;
    threads[tid].stack = NULL;
    threads[tid].state = THREAD_AVAILABLE;

    return (0);
}

/**
 * @details Releases all threads owned by the target process.
 */
int thread_free_all(pid_t pid)
{
    if (pid == KERNEL_PROCESS || process_is_valid(pid) != 0) {
        return (-EINVAL);
    }

    for (int i = 0; i < THREADS_MAX; i++) {
        if (threads[i].pid == pid) {
            thread_free(threads[i].tid);
        }
    }

    return (0);
}

/**
 * @details Gets the context of the target thread.
 */
struct context *thread_get_ctx(tid_t tid)
{
    if (tid < KERNEL_THREAD || tid >= THREADS_MAX) {
        return (NULL);
    }

    return (&threads[tid].ctx);
}

/**
 * @details Gets the running thread.
 */
tid_t thread_get_curr(void)
{
    return (running->tid);
}

/**
 * @details Gets the Process ID of the target thread.
 */
pid_t thread_get_pid(tid_t tid)
{
    if (tid < KERNEL_THREAD || tid >= THREADS_MAX) {
        return (-EINVAL);
    }

    return (threads[tid].pid);
}

/**
 * @details Gets the stack of the target thread.
 */
byte_t *thread_get_stack(tid_t tid)
{
    if (tid < KERNEL_THREAD || tid >= THREADS_MAX) {
        return (NULL);
    }

    return (threads[tid].stack);
}

/**
 * @details Yields the CPU.
 */
void thread_yield(void)
{
    struct thread *prev = running;
    struct thread *next = &threads[KERNEL_THREAD];

    if (running->state == THREAD_RUNNING) {
        running->state = THREAD_READY;
    }

    /* Selects the next thread to run. */
    for (int i = 0; i < THREADS_MAX; i++) {
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
 * @details This function puts the calling thread to sleep. The calling thread
 * resumes its execution when another thread invokes `thread_wakeup()`.
 * @note The root kernel thread cannot be put to sleep.
 */
void thread_sleep(void)
{
    running->state = THREAD_WAITING;
    thread_yield();
}

/**
 * @details This function wakes up the thread identified by @p tid.
 */
int thread_wakeup(tid_t tid)
{
    if (tid <= KERNEL_THREAD || tid >= THREADS_MAX) {
        return (-EINVAL);
    }

    threads[tid].state = THREAD_READY;

    return (0);
}

/**
 * @details This function puts all threads owned by the calling process to
 * sleep. The calling process resumes its execution when another process invokes
 * `process_wakeup()` which internally invokes `thread_wakeup_all()`.
 * @note The root kernel process cannot be put to sleep.
 */
void thread_sleep_all(void)
{
    for (int i = 0; i < THREADS_MAX; i++) {
        if (threads[i].pid == running->pid) {
            threads[i].state = THREAD_WAITING;
        }
    }

    thread_yield();
}

/**
 * @details This function wakes up all threads owned by the process identified
 * by @p pid.
 */
int thread_wakeup_all(pid_t pid)
{
    if (pid == KERNEL_PROCESS || process_is_valid(pid) != 0) {
        return (-EINVAL);
    }

    for (int i = 0; i < THREADS_MAX; i++) {
        if (threads[i].pid == pid) {
            threads[i].state = THREAD_READY;
        }
    }

    return (0);
}

/**
 * @details This function terminates the calling thread.
 */
noreturn void thread_exit(void)
{
    thread_free(running->tid);
    thread_yield();
    UNREACHABLE();
}

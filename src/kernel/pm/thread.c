/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/log.h>
#include <nanvix/kernel/mm.h>
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
 * Extern Declarations                                                        *
 *============================================================================*/

/**
 * @brief Low-level routine for bootstrapping a new user-created thread.
 */
extern void __start_uthread(void);

/**
 * @brief Low-level routine for bootstrapping a new process.
 */
extern void __do_process_setup(void);

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

/**
 * @brief Releases the memory used by a thread.
 *
 * @param t Target thread.
 */
static void thread_free_memory(struct thread *t)
{
    KASSERT(t != NULL);

    if (kpool_is_kpage(VADDR(t->kstack))) {
        KASSERT(kpage_put(t->kstack) == 0);
    } else if (t->ustack != NULL) {
        struct process *p = process_get(t->pid);

        KASSERT(upage_free((struct pde *)vmem_pgdir_get(p->vmem),
                           (vaddr_t)t->ustack) == 0);

        int pos = ((USER_END_VIRT - (word_t)t->ustack) / PAGE_SIZE) - 1;
        bitmap_clear(p->ustackmap, pos);
    }

    t->kstack = NULL;
    t->ustack = NULL;
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
    threads[KERNEL_THREAD].kstack = NULL;
    threads[KERNEL_THREAD].ustack = NULL;

    interrupt_register(INTERRUPT_TIMER, do_timer);
}

/**
 * @details Creates a new thread.
 */
tid_t thread_create(struct process *p, void *(*start)(), void *args,
                    void (*caller)(void))
{
    tid_t tid;
    struct thread *t;

    if ((p == NULL) || (process_is_valid(p->pid) != 0) || (start == NULL) ||
        (tid = thread_alloc()) < 0) {
        goto error0;
    }

    // Initializes basic TBC info.
    t = &threads[tid];
    t->tid = tid;
    t->pid = p->pid;
    t->age = 1;
    t->state = THREAD_READY;
    t->quantum = 0;
    t->start = start;
    t->args = args;
    t->retval = NULL;
    t->waitmap = 0;

    // Allocates thread's kernel stack.
    void *kstack = kpage_get(true);
    if (kstack == NULL) {
        goto error1;
    }
    t->kstack = kstack;

    // Checks if the owner process has memory avaible for a new thread.
    bitmap_t fbit = bitmap_first_free(&p->ustackmap, 0, sizeof(bitmap_t));
    if (fbit >= THREADS_MAX) {
        goto error2;
    }

    // Calculates the user stack base address.
    bitmap_set(&p->ustackmap, fbit);
    vaddr_t ubp = (vaddr_t)(USER_END_VIRT - ((fbit + 1) * PAGE_SIZE));
    t->ustack = (byte_t *)ubp;

    void *ksp = NULL;
    if ((word_t)t->start == USER_BASE_VIRT) {
        // Forges root thread.
        ksp = interrupt_forge_stack((t->ustack + PAGE_SIZE),
                                    t->kstack,
                                    (void (*)(void))t->start,
                                    __do_process_setup);
    } else {
        // Sets up user-created stack.
        if (upage_alloc(
                (struct pde *)vmem_pgdir_get(p->vmem), ubp, true, false) < 0) {
            goto error3;
        }

        void *usp = uthread_forge_stack(t->ustack, t->args, start);
        KASSERT(usp != NULL);
        ksp = interrupt_forge_stack(usp, t->kstack, caller, __start_uthread);
    }
    KASSERT(ksp != NULL);

    // Creates initial thread context.
    KASSERT(context_create(&t->ctx,
                           vmem_pgdir_get(p->vmem),
                           (const void *)(t->kstack + PAGE_SIZE),
                           ksp) == 0);

    return tid;

error3:
    bitmap_clear(p->ustackmap, fbit);
error2:
    kpage_put(kstack);
error1:
    t->pid = -1;
    t->state = THREAD_AVAILABLE;
    t->start = NULL;
    t->args = NULL;
error0:
    return (-1);
}

/**
 * @details Releases a thread entry.
 */
int thread_free(tid_t tid)
{
    if (tid <= KERNEL_THREAD || tid >= THREADS_MAX) {
        return (-EINVAL);
    }
    struct thread *t = &threads[tid];

    thread_free_memory(t);

    t->pid = -1;
    t->tid = -1;
    t->start = NULL;
    t->args = NULL;
    t->retval = NULL;
    t->age = -1;
    t->quantum = -1;
    t->state = THREAD_AVAILABLE;
    t->waitmap = 0;

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
noreturn void thread_exit(void *retval)
{
    running->retval = retval;
    running->state = THREAD_TERMINATED;
    if (running->detached) {
        thread_free(running->tid);
    } else {
        thread_free_memory(running);
        for (int i = 0; i < THREADS_MAX; i++) {
            if (bitmap_check_bit(&running->waitmap, threads[i].tid)) {
                threads[i].state = THREAD_READY;
                bitmap_clear(&running->waitmap, threads[i].tid);
            }
        }
    }
    thread_yield();
    UNREACHABLE();
}

/**
 * @details This function waits for the target thread to terminate.
 */
int thread_join(tid_t tid, void **retval)
{
    if (tid <= KERNEL_THREAD || tid >= THREADS_MAX) {
        return (-EINVAL);
    }

    if (threads[tid].state == THREAD_AVAILABLE) {
        return (-EAGAIN);
    }

    if (tid == running->tid || threads[tid].pid != running->pid) {
        return (-EINVAL);
    }

    if (threads[tid].detached) {
        return (-EINVAL);
    }

    if (threads[tid].state != THREAD_TERMINATED) {
        running->state = THREAD_WAITING;
        bitmap_set(&threads[tid].waitmap, running->tid);
        thread_yield();
    }

    if (retval != NULL) {
        *retval = threads[tid].retval;
        // Only the first thread to join can get the return value.
        threads[tid].retval = NULL;
    }

    thread_free(tid);

    return (0);
}

/**
 * @details This function detaches the target thread.
 */
int thread_detach(tid_t tid)
{
    if (tid <= KERNEL_THREAD || tid >= THREADS_MAX) {
        return (-EINVAL);
    }

    if (threads[tid].state == THREAD_AVAILABLE) {
        return (-EAGAIN);
    }

    if (threads[tid].pid != running->pid) {
        return (-EINVAL);
    }

    threads[tid].detached = true;
    if (threads[tid].state == THREAD_TERMINATED) {
        thread_free(tid);
    }

    return (0);
}

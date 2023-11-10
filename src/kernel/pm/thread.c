/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <elf.h>
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
#define KTHREAD_QUANTUM 100

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

/**
 * @brief Kernel thread.
 */
static struct thread *kernel = &threads[0];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Low-level routine for bootstrapping a new thread.
 */
extern void __do_thread_setup(void);

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

extern vaddr_t elf32_load(const struct elf32_fhdr *elf);

/**
 * @details This function returns a pointer to the thread
 * that is running in the underlying core.
 */
struct thread *thread_get_curr(void)
{
    return (running);
}

/**
 * @brief Bootstraps a new thread.
 */
void do_thread_setup(void)
{
    const vaddr_t user_fn_addr =
        elf32_load((struct elf32_fhdr *)((vaddr_t)running->start));
    KASSERT(user_fn_addr == USER_BASE_VIRT);

    vaddr_t user_stack_addr = USER_END_VIRT - PAGE_SIZE;

    KASSERT(vmem_attach(running->vmem, user_stack_addr, PAGE_SIZE) == 0);
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
        goto error0;
    }

    // Create a virtual memory space.
    vmem_t vmem = vmem_create();
    if (vmem == VMEM_NULL) {
        goto error0;
    }

    // Create a stack.
    void *kstack = kpage_get(true);
    if (kstack == NULL) {
        goto error1;
    }

    // Initializes thread control block.
    thread->tid = ++next_tid;
    thread->age = 1;
    thread->state = THREAD_READY;
    thread->arg = arg;
    thread->start = start;
    thread->stack = kstack;
    thread->vmem = vmem;

    const void *ksp = interrupt_forge_stack((void *)(USER_END_VIRT),
                                            thread->stack,
                                            (void (*)(void))USER_BASE_VIRT,
                                            __do_thread_setup);

    context_create(&thread->ctx,
                   vmem_pgdir_get(thread->vmem),
                   (const void *)((vaddr_t)thread->stack + PAGE_SIZE),
                   ksp);

    return (thread->tid);

error1:
    vmem_destroy(vmem);
error0:
    return (-1);
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
void thread_init(vmem_t root_vmem)
{
    kprintf("[kernel][pm] initializing thread system...");

    // Initializes the table of threads.
    for (int i = 0; i < KTHREAD_MAX; i++) {
        threads[i].age = 0;
        threads[i].state = THREAD_NOT_STARTED;
        threads[i].arg = NULL;
        threads[i].start = NULL;
    }

    // Initialize.
    kernel->vmem = (vmem_t)root_vmem;
    kernel->state = THREAD_RUNNING;

    running = &threads[0];

    interrupt_register(INTERRUPT_TIMER, do_timer);
}

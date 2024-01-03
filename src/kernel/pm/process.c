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

/**
 * @brief Loads an ELF 32 executable.
 *
 * @param elf Target ELF file.
 *
 * @returns Upon successful completion, the entry point of the ELF file is
 * returned. On failure, zero is returned instead.
 */
extern vaddr_t elf32_load(const struct elf32_fhdr *elf);

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum number of processes.
 */
#define PROCESS_MAX 16

/**
 * @brief Process quantum.
 */
#define PROCESS_QUANTUM 100

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Process table.
 */
static struct process processes[PROCESS_MAX];

/**
 * @brief Running process.
 */
static struct process *running = NULL;

/**
 * @brief Kernel process.
 */
static struct process *kernel = &processes[0];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Low-level routine for bootstrapping a new process.
 */
extern void __do_process_setup(void);

/**
 * @brief Allocates an entry in the process table.
 *
 * @returns Upon successful completion, a pointer to the allocated entry in the
 * process table is returned. Upon failure, NULL is returned instead.
 */
static struct process *process_alloc(void)
{
    // Find a process control block that is not in use.
    for (int i = 0; i < PROCESS_MAX; i++) {
        if (processes[i].state == PROCESS_NOT_STARTED) {
            processes[i].state = PROCESS_STARTED;
            return (&processes[i]);
        }
    }

    return (NULL);
}

/**
 * @brief Releases all resources that are used by a process.
 *
 * @param process Target process.
 */
static void process_free(struct process *process)
{
    process->pid = 0;
    process->state = PROCESS_NOT_STARTED;
    process->image = NULL;
    KASSERT(kpage_put(process->stack) == 0);
}

/**
 * @brief Handles a timer interrupt.
 */
static void do_timer(void)
{
    if (running->quantum++ >= PROCESS_QUANTUM) {
        process_yield();
    }
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function returns a pointer to the process that is running in
 * the underlying core.
 */
struct process *process_get_curr(void)
{
    return (running);
}

/**
 * @brief Bootstraps a new process.
 */
void do_process_setup(void)
{
    const vaddr_t user_fn_addr = elf32_load(running->image);
    KASSERT(user_fn_addr == USER_BASE_VIRT);

    vaddr_t user_stack_addr = USER_END_VIRT - PAGE_SIZE;

    KASSERT(vmem_attach(running->vmem, user_stack_addr, PAGE_SIZE) == 0);
}

/**
 * @details Creates a new process.
 */
pid_t process_create(const void *image)
{
    static pid_t next_pid = 0;
    struct process *process = NULL;

    // Find a process control block that is not in use.
    if ((process = process_alloc()) == NULL) {
        goto error0;
    }

    // Create a virtual memory space.
    vmem_t vmem = vmem_create();
    if (vmem == VMEM_NULL) {
        goto error1;
    }

    // Create a stack.
    void *kstack = kpage_get(true);
    if (kstack == NULL) {
        goto error2;
    }

    // Initializes process control block.
    process->pid = ++next_pid;
    process->age = 1;
    process->state = PROCESS_READY;
    process->image = image;
    process->stack = kstack;
    process->vmem = vmem;

    const void *ksp = interrupt_forge_stack((void *)(USER_END_VIRT),
                                            process->stack,
                                            (void (*)(void))USER_BASE_VIRT,
                                            __do_process_setup);

    context_create(&process->ctx,
                   vmem_pgdir_get(process->vmem),
                   (const void *)((vaddr_t)process->stack + PAGE_SIZE),
                   ksp);

    return (process->pid);

error2:
    vmem_destroy(vmem);
error1:
    process_free(process);
error0:
    return (-1);
}

/**
 * @details Yields the CPU.
 */
void process_yield(void)
{
    running->age = 0;
    running->state = PROCESS_READY;

    struct process *prev = running;
    struct process *next = prev;

    /* Selects the next process to run. */
    for (int i = 0; i < PROCESS_MAX; i++) {
        if (processes[i].state == PROCESS_READY) {
            if (processes[i].age++ >= next->age) {
                next = &processes[i];
            }
        }
    }

    running = next;
    running->age = 0;
    running->quantum = 0;
    running->state = PROCESS_RUNNING;

    __context_switch(&prev->ctx, &next->ctx);
}

/**
 * @brief Terminates the calling process.
 */
noreturn void process_exit(void)
{
    running->state = PROCESS_TERMINATED;
    process_free(running);
    process_yield();
    UNREACHABLE();
}

/**
 * @details This function atomically puts the calling process to sleep. Before
 * sleeping, the spinlock pointed to by @p lock is released.  The calling
 * process resumes its execution when another process invokes process_wakeup()
 * on it. When the process wakes up, the spinlock @p lock is re-acquired.
 */
void process_sleep(spinlock_t *lock)
{
    spinlock_unlock(lock);
    process_yield();
    spinlock_lock(lock);
}
/**
 * @details This function wakes up the process pointed to by @p process.
 */
void process_wakeup(struct process *t)
{
    t->state = PROCESS_READY;
}

/**
 * @details Initializes the process system.
 */
void process_init(vmem_t root_vmem)
{
    kprintf("[kernel][pm] initializing process system...");

    // Initializes the table of processes.
    for (int i = 0; i < PROCESS_MAX; i++) {
        processes[i].age = 0;
        processes[i].state = PROCESS_NOT_STARTED;
        processes[i].image = NULL;
    }

    // Initialize.
    kernel->vmem = (vmem_t)root_vmem;
    kernel->state = PROCESS_RUNNING;

    running = &processes[0];

    interrupt_register(INTERRUPT_TIMER, do_timer);
}

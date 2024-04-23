/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <elf.h>
#include <nanvix/errno.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <nanvix/kernel/pm/semaphore.h>
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
 * @brief Process quantum.
 */
#define PROCESS_QUANTUM 100

/*============================================================================*
 * Public Variables                                                           *
 *============================================================================*/

/**
 * @brief Kernel's semaphore.
 */
int kernel_semaphore = -EINVAL;

/**
 * @brief Kernel's semaphore.
 */
int user_semaphore = -EINVAL;

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Process table.
 */
static struct process processes[PROCESS_MAX];

/**
 * @brief Kernel process.
 */
static struct process *kernel = &processes[KERNEL_PROCESS];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

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
        if (!processes[i].active) {
            processes[i].active = true;
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
    KASSERT(process != kernel);
    process->pid = 0;
    process->active = false;
    process->image = NULL;
    thread_free_all(process->pid);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Checks if a PID refers to a valid process.
 */
int process_is_valid(pid_t pid)
{
    // Check if PID is invalid.
    if (!WITHIN(pid, 0, PROCESS_MAX)) {
        return (-EINVAL);
    }

    // Check if process is not active.
    if (!processes[pid].active) {
        return (-EINVAL);
    }

    return (0);
}

/**
 * @details This function returns a pointer to the process control block of the
 * process whose ID is equal to @p pid.
 */
struct process *process_get(pid_t pid)
{
    // Check if PID is invalid.
    if (!WITHIN(pid, 0, PROCESS_MAX)) {
        return (NULL);
    }

    // Check if process is not active.
    if (!processes[pid].active) {
        return (NULL);
    }

    // Find process.
    for (int i = 0; i < PROCESS_MAX; i++) {
        if (processes[i].pid == pid) {
            return (&processes[i]);
        }
    }

    return (NULL);
}

/**
 * @details This function returns a pointer to the process that is running in
 * the underlying core.
 */
struct process *process_get_curr(void)
{
    return (process_get(thread_get_pid(thread_get_curr())));
}

/**
 * @brief Bootstraps a new process.
 */
void do_process_setup(void)
{
    struct process *running = process_get_curr();
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

    // Allocate a new identity.
    struct identity *id = identity_new(kernel->id);
    if (id == NULL) {
        goto error2;
    }

    // Initializes process control block.
    process->pid = ++next_pid;
    process->id = id;
    process->image = image;
    process->vmem = vmem;
    process->ustackmap = 0;

    // Creates a thread.
    if ((process->tid = thread_create(
             process, (void *(*)())USER_BASE_VIRT, NULL, NULL)) < 0) {
        goto error3;
    }

    return (process->pid);

error3:
    KASSERT(identity_drop(id) == 0);
error2:
    vmem_destroy(vmem);
error1:
    process_free(process);
error0:
    return (-1);
}

/**
 * @brief Terminates the calling process.
 */
noreturn void process_exit(void)
{
    struct process *running = process_get_curr();
    process_free(running);
    thread_yield();
    UNREACHABLE();
}

/**
 * @details This function puts the calling process to sleep.  The calling
 * process resumes its execution when another process invokes `process_wakeup()`
 * on it.
 */
void process_sleep(void)
{
    thread_sleep_all();
}

/**
 * @details This function wakes up the process pointed to by @p process.
 */
void process_wakeup(struct process *p)
{
    thread_wakeup_all(p->pid);
}

/**
 * @details Initializes the process system.
 */
void process_init(vmem_t root_vmem)
{
    log(INFO, "initializing process system...");

    // Sanity check sizes.
    KASSERT_SIZE(sizeof(struct process_info), __SIZEOF_PROCESS_INFO);

    // Initializes the table of processes.
    for (int i = 0; i < PROCESS_MAX; i++) {
        processes[i].active = false;
        processes[i].image = NULL;
    }

    // Initialize.
    kernel->id = (struct identity *)identity_root();
    kernel->vmem = (vmem_t)root_vmem;
    kernel->active = true;
    thread_init();

    // Initialize semaphore table.
    semtable_init();

    // Initialize Kernel's semaphores.
    kernel_semaphore = semaphore_create(0);
    user_semaphore = semaphore_create(1);
}

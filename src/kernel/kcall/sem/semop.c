#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/pm/process.h>
#include <nanvix/kernel/pm/semaphore.h>

// Just a kernel call test.
int kcall_semop(int id, int op)
{
    // TODO
    return id + op;
}

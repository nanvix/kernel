#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/pm/process.h>
#include <nanvix/kernel/pm/semaphore.h>

int kcall_semctl(int id, int cmd, int val)
{
    // TODO
    return id + cmd + val;
}

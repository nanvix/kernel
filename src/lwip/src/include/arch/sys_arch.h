#ifndef _SYS_ARCH_H_
#define _SYS_ARCH_H_

#include <lwip/arch.h>
#include <lwip/opt.h>

typedef u32_t sys_sem_t;
typedef u32_t sys_mutex_t;
typedef u32_t sys_thread_t;
typedef u32_t sys_prot_t;

typedef struct {
    mem_ptr_t buffer[_MBOX_SIZE];
    u32_t head;
    u32_t tail;
    sys_sem_t empty;
    sys_sem_t full;
} sys_mbox_t;

#endif /* _SYS_ARCH_H_ */

#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H
#include <stddef.h>
#include "arch/cpu.h"
#include "arch/syscall.h"
#include "kernel/fatal.h"
#include "kernel/syscall.h"




#define kernel_except_reason(reason)	ARCH_EXCEPT(reason)



#define k_oops()	kernel_except_reason(K_ERR_KERNEL_OOPS)
#define k_panic()	kernel_except_reason(K_ERR_KERNEL_PANIC)

#endif /* _KERNEL_KERNEL_H */
#ifndef _ARCH_ARM64_ERROR_H
#define _ARCH_ARM64_ERROR_H

#include "kernel/syscall.h"
#include "kernel/kernel.h"


#define CODE_UNREACHABLE __builtin_unreachable()

#define ARCH_EXCEPT(reason_p)	do {					\
		if (k_is_user_context()) {				\
			arch_syscall_invoke1(reason_p,			\
                K_SYSCALL_USER_FAULT);			\
        } else {						\
            register uintptr_t _r __asm__("x0") =	\
                (uintptr_t)(reason_p);		\
            register uintptr_t _t __asm__("x1") = 0;	\
            __asm__ volatile("svc #0"			\
                     : : "r"(_r), "r"(_t)		\
                     : "memory");			\
        }							\
        CODE_UNREACHABLE; /* LCOV_EXCL_LINE */			\
    } while (false) 


#endif /* _ARCH_ARM64_ERROR_H */
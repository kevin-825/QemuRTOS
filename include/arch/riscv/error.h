#ifndef _ARCH_RISCV_ERROR_H
#define _ARCH_RISCV_ERROR_H

#include "kernel/syscall.h"
#include "kernel/kernel.h"

#define CODE_UNREACHABLE __builtin_unreachable()
/*
 * In S-mode, ecall (cause=9) is kept in M-mode for SBI so it never reaches
 * the S-mode exception handler.  Kernel-context panics use ebreak (cause=3,
 * breakpoint) which IS delegated to S-mode.  isr.S treats ebreak with
 * t0==RV_ECALL_RUNTIME_EXCEPT (0) as ARCH_EXCEPT and reads the reason from
 * saved a0 — mirroring exactly how M-mode handles ecall with RV_ECALL_RUNTIME_EXCEPT.
 */
#define ARCH_EXCEPT(reason_p)	do {					\
		if (k_is_user_context()) {				\
			arch_syscall_invoke1(reason_p,			\
				K_SYSCALL_USER_FAULT);			\
		} else {						\
			register unsigned long _r __asm__("a0") =	\
				(unsigned long)(reason_p);		\
			register unsigned long _t __asm__("t0") = 0;	\
			__asm__ volatile("ebreak"			\
					 : : "r"(_r), "r"(_t)		\
					 : "memory");			\
		}							\
		CODE_UNREACHABLE; /* LCOV_EXCL_LINE */			\
	} while (false)




#endif /* _ARCH_RISCV_ERROR_H */

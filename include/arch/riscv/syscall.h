#ifndef _ARCH_RISCV_SYSCALL_H
#define _ARCH_RISCV_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Syscall invocation macros. riscv-specific machine constraints used to ensure
 * args land in the proper registers.
 */
static inline uintptr_t arch_syscall_invoke6(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3, uintptr_t arg4,
					     uintptr_t arg5, uintptr_t arg6,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long a3 __asm__ ("a3") = arg4;
	register unsigned long a4 __asm__ ("a4") = arg5;
	register unsigned long a5 __asm__ ("a5") = arg6;
	register unsigned long t0 __asm__ ("t0") = call_id;

	__asm__ volatile ("ecall"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5),
			  "r" (t0)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke5(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3, uintptr_t arg4,
					     uintptr_t arg5,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long a3 __asm__ ("a3") = arg4;
	register unsigned long a4 __asm__ ("a4") = arg5;
	register unsigned long t0 __asm__ ("t0") = call_id;

	__asm__ volatile ("ecall"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (t0)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke4(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3, uintptr_t arg4,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long a3 __asm__ ("a3") = arg4;
	register unsigned long t0 __asm__ ("t0") = call_id;

	__asm__ volatile ("ecall"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (a3), "r" (t0)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke3(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t arg3,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long a2 __asm__ ("a2") = arg3;
	register unsigned long t0 __asm__ ("t0") = call_id;

	__asm__ volatile ("ecall"
			  : "+r" (a0)
			  : "r" (a1), "r" (a2), "r" (t0)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke2(uintptr_t arg1, uintptr_t arg2,
					     uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long a1 __asm__ ("a1") = arg2;
	register unsigned long t0 __asm__ ("t0") = call_id;

	__asm__ volatile ("ecall"
			  : "+r" (a0)
			  : "r" (a1), "r" (t0)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke1(uintptr_t arg1, uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0") = arg1;
	register unsigned long t0 __asm__ ("t0") = call_id;

	__asm__ volatile ("ecall"
			  : "+r" (a0)
			  : "r" (t0)
			  : "memory");
	return a0;
}

static inline uintptr_t arch_syscall_invoke0(uintptr_t call_id)
{
	register unsigned long a0 __asm__ ("a0");
	register unsigned long t0 __asm__ ("t0") = call_id;

	__asm__ volatile ("ecall"
			  : "=r" (a0)
			  : "r" (t0)
			  : "memory");
	return a0;
}


register unsigned long riscv_tp_reg __asm__ ("tp");

static inline bool arch_is_user_context(void)
{
    return false;


	/* don't try accessing TLS variables if tp is not initialized */
	if (riscv_tp_reg == 0) {
		return false;
	}

	/* Defined in arch/riscv/core/thread.c */
	// extern Z_THREAD_LOCAL uint8_t is_user_mode;

	// return is_user_mode != 0;

}


#ifdef __cplusplus
}
#endif

#endif /* _ARCH_RISCV_SYSCALL_H */

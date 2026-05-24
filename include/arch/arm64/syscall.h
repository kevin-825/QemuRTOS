#ifndef _ARCH_ARM64_SYSCALL_H
#define _ARCH_ARM64_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif



static inline uintptr_t arch_syscall_invoke6(uintptr_t arg1, uintptr_t arg2,
                         uintptr_t arg3, uintptr_t arg4,
                         uintptr_t arg5, uintptr_t arg6,
                         uintptr_t call_id)
{
    register unsigned long a0 __asm__ ("x0") = arg1;
    register unsigned long a1 __asm__ ("x1") = arg2;
    register unsigned long a2 __asm__ ("x2") = arg3;
    register unsigned long a3 __asm__ ("x3") = arg4;
    register unsigned long a4 __asm__ ("x4") = arg5;
    register unsigned long a5 __asm__ ("x5") = arg6;
    register unsigned long t0 __asm__ ("x8") = call_id;

    __asm__ volatile ("svc #0"
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
    register unsigned long a0 __asm__ ("x0") = arg1;
    register unsigned long a1 __asm__ ("x1") = arg2;
    register unsigned long a2 __asm__ ("x2") = arg3;
    register unsigned long a3 __asm__ ("x3") = arg4;
    register unsigned long a4 __asm__ ("x4") = arg5;
    register unsigned long t0 __asm__ ("x8") = call_id;

    __asm__ volatile ("svc #0"
              : "+r" (a0)
              : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (t0)
              : "memory");
    return a0;
}

static inline uintptr_t arch_syscall_invoke4(uintptr_t arg1, uintptr_t arg2,
                         uintptr_t arg3, uintptr_t arg4,
                         uintptr_t call_id)
{
    register unsigned long a0 __asm__ ("x0") = arg1;
    register unsigned long a1 __asm__ ("x1") = arg2;
    register unsigned long a2 __asm__ ("x2") = arg3;
    register unsigned long a3 __asm__ ("x3") = arg4;
    register unsigned long t0 __asm__ ("x8") = call_id;

    __asm__ volatile ("svc #0"
              : "+r" (a0)
              : "r" (a1), "r" (a2), "r" (a3), "r" (t0)
              : "memory");
    return a0;
}


static inline uintptr_t arch_syscall_invoke3(uintptr_t arg1, uintptr_t arg2,
                         uintptr_t arg3,
                         uintptr_t call_id)
{
    register unsigned long a0 __asm__ ("x0") = arg1;
    register unsigned long a1 __asm__ ("x1") = arg2;
    register unsigned long a2 __asm__ ("x2") = arg3;
    register unsigned long t0 __asm__ ("x8") = call_id;

    __asm__ volatile ("svc #0"
              : "+r" (a0)
              : "r" (a1), "r" (a2), "r" (t0)
              : "memory");
    return a0;
}


static inline uintptr_t arch_syscall_invoke2(uintptr_t arg1, uintptr_t arg2,
                         uintptr_t call_id)
{
    register unsigned long a0 __asm__ ("x0") = arg1;
    register unsigned long a1 __asm__ ("x1") = arg2;
    register unsigned long t0 __asm__ ("x8") = call_id;

    __asm__ volatile ("svc #0"
              : "+r" (a0)
              : "r" (a1), "r" (t0)
              : "memory");
    return a0;
}

static inline uintptr_t arch_syscall_invoke1(uintptr_t arg1, uintptr_t call_id)
{
    register unsigned long a0 __asm__ ("x0") = arg1;
    register unsigned long t0 __asm__ ("x8") = call_id;

    __asm__ volatile ("svc #0"
              : "+r" (a0)
              : "r" (t0)
              : "memory");
    return a0;
}

/* arm64 thread local*/
register unsigned long  __thread_pointer __asm__("x18");
static inline bool arch_is_user_context(void)
{
    return false;
    return __thread_pointer != 0;
}



#ifdef __cplusplus
}
#endif

#endif /* _ARCH_ARM64_SYSCALL_H */

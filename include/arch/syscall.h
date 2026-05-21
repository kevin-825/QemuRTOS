#ifndef _ARCH_SYSCALL_H
#define _ARCH_SYSCALL_H


#if defined(CONFIG_ARCH_RISCV)
    #include "arch/riscv/syscall.h"
#elif defined(CONFIG_ARCH_ARM)
    #include "arch/arm/syscall.h"
#elif defined(CONFIG_ARCH_ARM64)
    #include "arch/arm64/syscall.h"
#else
    #error "Target architecture not supported or missing from Kconfig!"
#endif


#endif /* _ARCH_SYSCALL_H */
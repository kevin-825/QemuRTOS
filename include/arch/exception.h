#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H




#if defined(CONFIG_ARCH_RISCV)
    #include "arch/riscv/exception.h"
#elif defined(CONFIG_ARCH_ARM)
    #include "arch/arm/exception.h
#elif defined(CONFIG_ARCH_ARM64)
    #include "arch/arm64/exception.h"
#else
    #error "Target architecture not supported or missing from Kconfig!"
#endif


#endif // _ARCH_EXCEPTION_H
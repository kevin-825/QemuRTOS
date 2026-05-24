#ifndef _ARCH_CPU_H
#define _ARCH_CPU_H

#if defined(CONFIG_ARCH_RISCV)
    #include "arch/riscv/arch.h"
#elif defined(CONFIG_ARCH_ARM)
    #include "arch/arm/arch.h"
#elif defined(CONFIG_ARCH_ARM64)
    #include "arch/arm64/arch.h"
#else
    #error "Target architecture not supported or missing from Kconfig!"
#endif



#endif /* _ARCH_CPU_H */

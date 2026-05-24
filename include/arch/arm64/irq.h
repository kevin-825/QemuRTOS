
#ifndef _ARCH_RISCV_EXCEPTION_H
#define _ARCH_RISCV_EXCEPTION_H

#include <stdint.h>
#include <stddef.h>

/* Architecture-specific irq apis for arm64*/
static inline unsigned int arch_irq_lock(void) {
    unsigned int key;
    __asm__ volatile("msr daifset, #2" : "=r" (key) : : "memory");
    return key;
}

static inline void arch_irq_unlock(unsigned int key) {
    __asm__ volatile("msr daifclr, #2" : : : "memory");
}

void arch_halt(void);

#endif /* _ARCH_RISCV_EXCEPTION_H */
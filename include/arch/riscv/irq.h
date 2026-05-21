/* include/arch/riscv/irq.h */
#ifndef _ARCH_RISCV_IRQ_H
#define _ARCH_RISCV_IRQ_H

#define MSTATUS_MIE (1 << 3)

/* *
 * Locks interrupts and returns the previous state of the mstatus register.
 * This executes in 1 clock cycle using the 'csrrc' instruction.
 */
static inline unsigned int arch_irq_lock(void) {
    unsigned int key;
    __asm__ volatile("csrrc %0, mstatus, %1" : "=r" (key) : "r" (MSTATUS_MIE) : "memory");
    return key;
}

/* *
 * Unlocks interrupts ONLY if they were enabled before the lock was called.
 */
static inline void arch_irq_unlock(unsigned int key) {
    __asm__ volatile("csrs mstatus, %0" : : "r" (key & MSTATUS_MIE) : "memory");
}

/* *
 * Forward declaration of the backend hardware halt function.
 * The actual implementation lives in arch/riscv/core/fatal.c
 */
void arch_halt(void);



#endif /* _ARCH_RISCV_IRQ_H */
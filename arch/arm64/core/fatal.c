/* arch/riscv/core/fatal.c */
#include "arch/arm64/exception.h"
#include "arch/arm64/irq.h"

/* *
 * Permanently halts the RISC-V CPU core.
 * This is the end-of-the-line function called by k_panic.
 */
void arch_halt(void) {
    /* Ensure interrupts are absolutely dead before sleeping */
    arch_irq_lock();
    
    while (1) {
        /* wfi: Wait For Interrupt. Puts the silicon to sleep to save power. */
        __asm__ volatile("wfi" : : : "memory");
    }
}
#ifndef _ARCH_ARM64_EXCEPTION_H
#define _ARCH_ARM64_EXCEPTION_H

#include <stdint.h>
#include <stddef.h>

/* Architecture-specific exception state frame for ARM64 AARCH64*/
struct arch_esf {
    uintptr_t x[30];     /* General-purpose registers x0-x30 */
    uint64_t lr;
    uintptr_t elr_el1;   /* Exception Link Register (return address) */
    uintptr_t spsr_el1;  /* Saved Program Status Register (processor state) */
    
};

// Group 1
void do_sync_el1_sp0(struct arch_esf *esf);
void do_irq_el1_sp0(struct arch_esf *esf);
void do_fiq_el1_sp0(struct arch_esf *esf);
void do_serror_el1_sp0(struct arch_esf *esf);

// Group 2
void do_sync_el1_spx(struct arch_esf *esf);
void do_irq_el1_spx(struct arch_esf *esf);
void do_fiq_el1_spx(struct arch_esf *esf);
void do_serror_el1_spx(struct arch_esf *esf);

// Group 3
void do_sync_el0_64(struct arch_esf *esf);
void do_irq_el0_64(struct arch_esf *esf);
void do_fiq_el0_64(struct arch_esf *esf);
void do_serror_el0_64(struct arch_esf *esf);

// Group 4
void do_sync_el0_32(struct arch_esf *esf);
void do_irq_el0_32(struct arch_esf *esf);
void do_fiq_el0_32(struct arch_esf *esf);
void do_serror_el0_32(struct arch_esf *esf);



#endif /* _ARCH_ARM64_EXCEPTION_H */

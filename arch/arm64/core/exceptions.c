#include "arch/arm64/exception.h"
#include "kernel/fatal.h"
#define kernel_fatal_error_arch(esf) kernel_fatal_error(K_ERR_CPU_EXCEPTION, esf)

// Your kernel-level print function (usually hooked to the PL011 UART in QEMU virt)
extern void printk(const char *fmt, ...);

#define read_sysreg(reg) ({ \
    uint64_t _val; \
    __asm__ volatile("mrs %0, " #reg : "=r" (_val)); \
    _val; \
})

// Shared helper to dump detailed Synchronous exception state
static void dump_sync_state(const char *source, struct arch_esf *esf) {
    uint64_t esr = read_sysreg(esr_el1);
    uint64_t far = read_sysreg(far_el1);
    uint32_t ec = (esr >> 26) & 0x3F;

    printk("\n=== SYNCHRONOUS EXCEPTION ===\n");
    printk("Source:  %s\n", source);
    printk("ELR:     0x%016lx\n", esf->elr_el1);
    printk("SPSR:    0x%016lx\n", esf->spsr_el1);
    printk("ESR:     0x%016lx (Class: 0x%02x)\n", esr, ec);
    printk("FAR:     0x%016lx\n", far);

    switch (ec) {
        case 0x15: printk("Reason: SVC (System Call) in AArch64\n"); break;
        case 0x24: printk("Reason: Data Abort (Lower EL)\n"); break;
        case 0x25: printk("Reason: Data Abort (Current EL)\n"); break;
        case 0x20: printk("Reason: Instruction Abort (Lower EL)\n"); break;
        case 0x21: printk("Reason: Instruction Abort (Current EL)\n"); break;
        case 0x26: printk("Reason: SP Alignment Fault\n"); break;
        case 0x3C: printk("Reason: BRK Instruction (AArch64)\n"); break;
        default:   printk("Reason: Unknown (Check ARM Architecture Reference Manual)\n"); break;
    }
}

// -----------------------------------------------------------------------------
// Group 1: Current EL using SP0
// -----------------------------------------------------------------------------
void do_sync_el1_sp0(struct arch_esf *esf) {
    dump_sync_state("EL1_SP0", esf);
    kernel_fatal_error_arch(esf);
}
void do_irq_el1_sp0(struct arch_esf *esf) {
    printk("IRQ triggered from EL1 using SP0\n");
    // Handle IRQ logic here
}
void do_fiq_el1_sp0(struct arch_esf *esf) {
    printk("FIQ triggered from EL1 using SP0\n");
    while(1);
}
void do_serror_el1_sp0(struct arch_esf *esf) {
    printk("System Error (SError) from EL1 using SP0\n");
    kernel_fatal_error_arch(esf);
}

// -----------------------------------------------------------------------------
// Group 2: Current EL using SPx (Primary Kernel Execution State)
// -----------------------------------------------------------------------------
void do_sync_el1_spx(struct arch_esf *esf) {
    dump_sync_state("EL1_SPx (Kernel Bug)", esf);
    kernel_fatal_error_arch(esf);
}
void do_irq_el1_spx(struct arch_esf *esf) {
    // printk("IRQ triggered from Kernel (EL1_SPx)\n");
    // e.g., gic_handle_interrupt();
}
void do_fiq_el1_spx(struct arch_esf *esf) {
    printk("FIQ triggered from Kernel (EL1_SPx)\n");
    while(1);
}
void do_serror_el1_spx(struct arch_esf *esf) {
    printk("System Error (SError) from Kernel (EL1_SPx)\n");
    kernel_fatal_error_arch(esf);
}

// -----------------------------------------------------------------------------
// Group 3: Lower EL AArch64 (User Space Executing in 64-bit)
// -----------------------------------------------------------------------------
void do_sync_el0_64(struct arch_esf *esf) {
    dump_sync_state("EL0_64 (User Space Trap/Syscall)", esf);
    // If it's a syscall (SVC), you extract the syscall number from x8 here
    // and route to your syscall table. Otherwise, kill the user process.
    kernel_fatal_error_arch(esf);
}
void do_irq_el0_64(struct arch_esf *esf) {
    // printk("IRQ triggered while in User Space (EL0_64)\n");
    // e.g., gic_handle_interrupt();
}
void do_fiq_el0_64(struct arch_esf *esf) {
    printk("FIQ triggered while in User Space (EL0_64)\n");
    // Terminate or handle
}
void do_serror_el0_64(struct arch_esf *esf) {
    dump_sync_state("EL0_64 (System Error)", esf);
    // Terminate process
    kernel_fatal_error_arch(esf);
}

// -----------------------------------------------------------------------------
// Group 4: Lower EL AArch32 (User Space Executing in 32-bit)
// -----------------------------------------------------------------------------
void do_sync_el0_32(struct arch_esf *esf) {
    dump_sync_state("EL0_32", esf);
    kernel_fatal_error_arch(esf);
}
void do_irq_el0_32(struct arch_esf *esf) {
    printk("IRQ triggered from 32-bit EL0\n");
}
void do_fiq_el0_32(struct arch_esf *esf) {
    printk("FIQ triggered from 32-bit EL0\n");
    while(1);
}
void do_serror_el0_32(struct arch_esf *esf) {
    printk("System Error (SError) from 32-bit EL0\n");
    kernel_fatal_error_arch(esf);
}
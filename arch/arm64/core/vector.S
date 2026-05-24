.global vector_table

// Context Save Macro
.macro kernel_entry
    sub sp, sp, #288
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    
    mrs x21, elr_el1
    mrs x22, spsr_el1
    stp x30, x21, [sp, #16 * 15] 
    str x22, [sp, #16 * 16]      
.endm

// Context Restore Macro
.macro kernel_exit
    ldr x22, [sp, #16 * 16]
    ldp x30, x21, [sp, #16 * 15]
    msr spsr_el1, x22
    msr elr_el1, x21

    ldp x28, x29, [sp, #16 * 14]
    ldp x26, x27, [sp, #16 * 13]
    ldp x24, x25, [sp, #16 * 12]
    ldp x22, x23, [sp, #16 * 11]
    ldp x20, x21, [sp, #16 * 10]
    ldp x18, x19, [sp, #16 * 9]
    ldp x16, x17, [sp, #16 * 8]
    ldp x14, x15, [sp, #16 * 7]
    ldp x12, x13, [sp, #16 * 6]
    ldp x10, x11, [sp, #16 * 5]
    ldp x8, x9, [sp, #16 * 4]
    ldp x6, x7, [sp, #16 * 3]
    ldp x4, x5, [sp, #16 * 2]
    ldp x2, x3, [sp, #16 * 1]
    ldp x0, x1, [sp, #16 * 0]
    add sp, sp, #288
    eret
.endm

// Vector Table Entry Macro
.macro ventry label
    .align 7
    b \label
.endm

// -----------------------------------------------------------------------------
// The 16 Explicit Assembly Handlers
// -----------------------------------------------------------------------------

// Group 1: Current EL with SP0
handler_sync_el1_sp0:
    kernel_entry; mov x0, sp; bl do_sync_el1_sp0; kernel_exit
handler_irq_el1_sp0:
    kernel_entry; mov x0, sp; bl do_irq_el1_sp0; kernel_exit
handler_fiq_el1_sp0:
    kernel_entry; mov x0, sp; bl do_fiq_el1_sp0; kernel_exit
handler_serror_el1_sp0:
    kernel_entry; mov x0, sp; bl do_serror_el1_sp0; kernel_exit

// Group 2: Current EL with SPx
handler_sync_el1_spx:
    kernel_entry; mov x0, sp; bl do_sync_el1_spx; kernel_exit
handler_irq_el1_spx:
    kernel_entry; mov x0, sp; bl do_irq_el1_spx; kernel_exit
handler_fiq_el1_spx:
    kernel_entry; mov x0, sp; bl do_fiq_el1_spx; kernel_exit
handler_serror_el1_spx:
    kernel_entry; mov x0, sp; bl do_serror_el1_spx; kernel_exit

// Group 3: Lower EL AArch64
handler_sync_el0_64:
    kernel_entry; mov x0, sp; bl do_sync_el0_64; kernel_exit
handler_irq_el0_64:
    kernel_entry; mov x0, sp; bl do_irq_el0_64; kernel_exit
handler_fiq_el0_64:
    kernel_entry; mov x0, sp; bl do_fiq_el0_64; kernel_exit
handler_serror_el0_64:
    kernel_entry; mov x0, sp; bl do_serror_el0_64; kernel_exit

// Group 4: Lower EL AArch32
handler_sync_el0_32:
    kernel_entry; mov x0, sp; bl do_sync_el0_32; kernel_exit
handler_irq_el0_32:
    kernel_entry; mov x0, sp; bl do_irq_el0_32; kernel_exit
handler_fiq_el0_32:
    kernel_entry; mov x0, sp; bl do_fiq_el0_32; kernel_exit
handler_serror_el0_32:
    kernel_entry; mov x0, sp; bl do_serror_el0_32; kernel_exit

// -----------------------------------------------------------------------------
// The Vector Table
// -----------------------------------------------------------------------------
.align 11
.global vector_table_el1
vector_table_el1:
    // Group 1
    ventry handler_sync_el1_sp0
    ventry handler_irq_el1_sp0
    ventry handler_fiq_el1_sp0
    ventry handler_serror_el1_sp0

    // Group 2
    ventry handler_sync_el1_spx
    ventry handler_irq_el1_spx
    ventry handler_fiq_el1_spx
    ventry handler_serror_el1_spx

    // Group 3
    ventry handler_sync_el0_64
    ventry handler_irq_el0_64
    ventry handler_fiq_el0_64
    ventry handler_serror_el0_64

    // Group 4
    ventry handler_sync_el0_32
    ventry handler_irq_el0_32
    ventry handler_fiq_el0_32
    ventry handler_serror_el0_32
.extern vector_table_el1
.global _start
.section .text.boot
/* =======================================================
 * FAKE LINUX IMAGE HEADER (64 Bytes)
 * Tricks QEMU into passing the DTB in x0
 * ======================================================= */
_head:
    b _start                /* 0x00: Branch over this header */
    .word 0                 /* 0x04: Reserved */
    .quad 0                 /* 0x08: text_offset */
    .quad 0                 /* 0x10: image_size */
    .quad 8                 /* 0x18: flags (Little Endian, 4k pages) */
    .quad 0                 /* 0x20: reserved */
    .quad 0                 /* 0x28: reserved */
    .quad 0                 /* 0x30: reserved */
    .word 0x644D5241        /* 0x38: Magic number ("ARM\x64") */
    .word 0                 /* 0x3C: reserved */
/* ======================================================= */
_start:
    /* 1. Check which CPU core this is (MPIDR_EL1 register) */
    mrs x1, mpidr_el1
    and x1, x1, #0xFF      /* Extract core ID */
    cbz x1, primary_core   /* If core ID == 0, jump to primary_core */

    /* Secondary cores go to sleep */
park:
    wfe                    /* Wait For Event */
    b park

primary_core:
    /* Note: QEMU passes the DTB pointer in x0. 
     * We MUST NOT overwrite x0 during our EL switch! Use x1, x2, x3. */

    /* 2. Check Current Exception Level */
    mrs x1, CurrentEL
    lsr x1, x1, #2         /* Shift bits [3:2] down to read EL */
    
    cmp x1, #3
    b.eq drop_el3_to_el1   /* If EL == 3, drop down */
    cmp x1, #2
    b.eq drop_el2_to_el1   /* If EL == 2, drop down */
    
    /* If already EL1, jump straight to setup */
    b el1_entry

drop_el3_to_el1:
    /* Configure SCR_EL3 (Secure Configuration Register) */
    /* Bit 10 (RW) = 1: Next EL is AArch64. Bit 0 (NS) = 1: Non-Secure */
    mov x1, #(1 << 10) | (1 << 0)
    msr scr_el3, x1

    /* Configure SPSR_EL3 (Saved Program Status Register) */
    /* 0x3C5: Mask D,A,I,F interrupts (0x3C0) + Target EL1h (0x05) */
    mov x1, #0x3C5
    msr spsr_el3, x1

    /* Set the return address to our el1_entry label */
    adr x1, el1_entry
    msr elr_el3, x1
    
    eret                   /* Perform the exception return to EL1 */

drop_el2_to_el1:
    /* Configure HCR_EL2 (Hypervisor Configuration Register) */
    /* Bit 31 (RW) = 1: EL1 is AArch64 */
    mov x1, #(1 << 31)
    msr hcr_el2, x1

    /* Configure SPSR_EL2 */
    /* 0x3C5: Mask D,A,I,F interrupts + Target EL1h */
    mov x1, #0x3C5
    msr spsr_el2, x1

    /* Set the return address to our el1_entry label */
    adr x1, el1_entry
    msr elr_el2, x1

    eret                   /* Perform the exception return to EL1 */

el1_entry:
    /* ==========================================
     * WE ARE NOW OFFICIALLY IN EL1
     * ========================================== */

    /* 3. Save the DTB pointer passed by QEMU in x0 */
    ldr x1, =g_dtb_ptr     /* Load address of global C variable */
    str x0, [x1]           /* Store the DTB pointer there */

    /* 4. Setup the Stack (This now configures sp_el1!) */
    /* AArch64 requires the stack to be 16-byte aligned */
    ldr x1, =_stack_top
    mov sp, x1

    /* install vector_table_el1 into vbar_el1 */
// Load the address of the vector table into x0
    ldr x1, =vector_table_el1
    
    // Write x1 into the VBAR_EL1 register
    msr vbar_el1, x1

    // Ensure the system register write is complete before continuing
    isb

    /* 
     * 5. Clear the BSS section.
     */
    ldr x1, =__bss_start
    ldr x2, =__bss_end
    cbz x2, end_clear_bss  /* Skip if BSS is empty */
clear_bss:
    str xzr, [x1], #8      /* Store 64-bit 0 (xzr) and increment x1 by 8 */
    cmp x1, x2
    b.lo clear_bss
end_clear_bss:

    /* 6. Jump to your C Kernel */
    bl main                /* Branch with Link (calls your main function) */

    /* Fallback if kernel returns */
halt:
    wfe
    b halt
    
/* Define the global variable for C to access */
.section .data
.global g_dtb_ptr
g_dtb_ptr: .dword 0        /* 64-bit variable */

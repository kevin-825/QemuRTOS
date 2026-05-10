/* arch/aarch64/core/startup.S */

.global _start
.section .text.boot

_start:
    /* 1. Check which CPU core this is (MPIDR_EL1 register) */
    mrs x1, mpidr_el1
    and x1, x1, #0xFF      /* Extract core ID */
    cbz x1, primary_core   /* If core ID == 0, jump to primary_core */

    /* Secondary cores go to sleep */
park:
    wfe                    /* Wait For Event (ARM's version of WFI) */
    b park

primary_core:
    /* 2. Save the DTB pointer passed by QEMU in x0 */
    ldr x1, =g_dtb_ptr     /* Load address of global C variable */
    str x0, [x1]           /* Store the DTB pointer there */

    /* 3. Setup the Stack */
    /* AArch64 requires the stack to be 16-byte aligned! */
    ldr x1, =_stack_top
    mov sp, x1

    /* 
     * 4. Clear the BSS section.
     * Note: We do NOT need to copy the .data segment because QEMU 
     * loaded our entire ELF directly into RAM for us. 
     */
    ldr x1, =__bss_start
    ldr x2, =__bss_end
    cbz x2, end_clear_bss  /* Skip if BSS is empty */
clear_bss:
    str xzr, [x1], #8      /* Store 64-bit 0 (xzr) and increment x1 by 8 */
    cmp x1, x2
    b.lo clear_bss
end_clear_bss:

    /* 5. Jump to your C Kernel */
    bl main         /* Branch with Link (calls your main function) */

    /* Fallback if kernel returns */
halt:
    wfe
    b halt

/* Define the global variable for C to access */
.section .data
.global g_dtb_ptr
g_dtb_ptr: .dword 0        /* 64-bit variable */

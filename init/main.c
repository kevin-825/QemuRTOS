/* kernel/init.c */
#include "device.h"
#include "kernel/printk.h"
extern unsigned int g_dtb_ptr;
void main(void) {
    device_init_all((void*)g_dtb_ptr);
    console_init();

    pr_info("======================================\n");
    pr_info("  QemuRTOS Boot Sequence Initiated\n");
    pr_info("======================================\n");

    pr_debug("Initializing memory manager...\n"); /* This is hidden by default! */
    
    int cpu_id = 0;
    pr_info("CPU Hart ID: %d\n", cpu_id);
    
    const struct device *uart = device_get_binding("uart0");
    if (!uart) {
        pr_fatal("Failed to bind UART0!\n");
        while(1); /* Halt system */
    }

    pr_info("System Ready.\n");

    while (1) {
        asm volatile("wfi");
    }
}

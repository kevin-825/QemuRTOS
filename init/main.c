/* kernel/init.c */
#include "kernel/device.h"
#include "kernel/printk.h"
#include "kernel/assert.h"
#include "kernel/mm/simple_mm.h"
#include "test.h"

extern unsigned long g_dtb_ptr;

extern uint8_t _heap[];
extern uint8_t _eheap[];

void main(void) {
    simple_init(_heap, _eheap - _heap);

    platform_bus_enumerate((void*)g_dtb_ptr);
    platform_bus_match_drivers((void*)g_dtb_ptr);

    int ret = console_init((void*)g_dtb_ptr);
    if (ret < 0) {
        k_panic();
        /* If console initialization fails, we have no way to print an error message.
         * In a real OS, we might blink an LED or halt the CPU here. For now, we just return. */
        return;
    }

    pr_info("======================================\n");
    pr_info("  QemuRTOS Boot Sequence Initiated\n");
    pr_info("======================================\n");

    
    int cpu_id = 0;
    pr_info("CPU Hart ID: %d\n", cpu_id);

    pr_info("System Ready.\n");

    run_mm_tests();

    run_printk_tests();

    while (1) {
        asm volatile("wfi");
    }
}

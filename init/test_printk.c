#include "test.h"
#include "kernel/printk.h"
#include "kernel/mm/kmalloc.h"



void run_printk_tests(void)
{
    /* Prints a perfectly aligned table, regardless of string length! */
    printk("%-12s | %-8s\n", "DRIVER", "STATUS");
    printk("%-12s | %-8s\n", "uart_pl011", "OK");
    printk("%-12s | %-8s\n", "virtio_blk", "FAILED");

    /* Automatically attaches '0x' and perfectly pads to 8 characters */
    printk("Register dumped at: %#08x\n", 0x4F); // Outputs: 0x00004f


    /* Forces numbers to align perfectly by managing the minus sign */
    printk("Temp offset: %+d\n", 15);  // Outputs: +15
    printk("Temp offset: % d\n", 15);  // Outputs:  15
    printk("Temp offset: % d\n", -15); // Outputs: -15


    /* Imagine this string is corrupted and missing its \0 terminator */
    char bad_string[] = {'I', 'N', 'I', 'T', 'X', 'Y', 'Z'}; 

    /* Stop printing after exactly 4 characters to prevent a crash! */
    printk("Subsystem: %.4s\n", bad_string); // Outputs: INIT

    void *p = (void *)0x8000;
    printk("%#zx\n", (size_t)p);

    printk("0x%zx\n", (size_t)p);
    printk("printing pointer: %p\n", p);
    printk("% p\n", p);
}
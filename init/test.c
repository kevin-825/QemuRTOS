#include "kernel/printk.h"
#include "kernel/mm/simple_mm.h"

void test_simple_mm(void) {
    printk("\n========================================\n");
    printk("       STARTING MEMORY MANAGER TEST       \n");
    printk("========================================\n");

    /* 1. Allocate 3 blocks */
    printk("\n[Test] Allocating 3 blocks (p1=64, p2=128, p3=256)...\n");
    void *p1 = simple_malloc(64);
    void *p2 = simple_malloc(128);
    void *p3 = simple_malloc(256);
    
    printk("p1 allocated at: %p\n", p1);
    printk("p2 allocated at: %p\n", p2);
    printk("p3 allocated at: %p\n", p3);
    
    printk("\n--- Heap State After Allocations ---\n");
    simple_mm_dump();

    /* 2. Free the middle block (Creates an isolated free block) */
    printk("\n[Test] Freeing middle block (p2)...\n");
    simple_free(p2);
    
    printk("\n--- Heap State After Freeing p2 ---\n");
    simple_mm_dump();

    /* 3. Free the bottom block (Triggers FORWARD coalescing: p2 + p3) */
    printk("\n[Test] Freeing bottom block (p3)...\n");
    simple_free(p3);
    
    printk("\n--- Heap State After Freeing p3 (Forward Coalesce) ---\n");
    simple_mm_dump();

    /* 4. Free the top block (Triggers BACKWARD coalescing: p1 + p2/p3) */
    printk("\n[Test] Freeing top block (p1)...\n");
    simple_free(p1);
    
    printk("\n--- Heap State After Freeing p1 (Total Coalesce) ---\n");
    simple_mm_dump();
    
    printk("\n========================================\n");
    printk("       MEMORY MANAGER TEST COMPLETE       \n");
    printk("========================================\n");
}
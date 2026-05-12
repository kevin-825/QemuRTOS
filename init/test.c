#include <stddef.h>
#include <stdint.h>
#include "kernel/printk.h"
#include "kernel/mm/kmalloc.h"
#include "kernel/mm/simple_mm.h" /* Included purely for simple_mm_dump() */

#define STRESS_TEST_ITERATIONS 50
#define MAX_ALLOC_SIZE 1024
#define MIN_ALLOC_SIZE 8

/* -------------------------------------------------------------------
 * Bare-Metal Pseudo-Random Number Generator (Xorshift32)
 * Used because standard <stdlib.h> rand() is not available in early boot.
 * ------------------------------------------------------------------- */
static uint32_t rand_state = 0xDEADBEEF;

static uint32_t bare_rand(void) {
    rand_state ^= rand_state << 13;
    rand_state ^= rand_state >> 17;
    rand_state ^= rand_state << 5;
    return rand_state;
}

/* -------------------------------------------------------------------
 * Phase 1: The Deterministic Coalescing Test
 * Verifies the 3-way merge logic perfectly before adding chaos.
 * ------------------------------------------------------------------- */
static void test_deterministic_coalesce(void) {
    printk("\n--- PHASE 1: Deterministic Coalescing Test ---\n");
    
    void *p1 = kmalloc(64);
    void *p2 = kmalloc(128);
    void *p3 = kmalloc(256);
    void *p4 = kmalloc(512); // Guard block so p3 isn't the physical end of heap

    printk("[+] Allocated 4 contiguous blocks: p1=%p, p2=%p, p3=%p, p4=%p\n", p1, p2, p3, p4);

    printk("\n[~] Freeing p2 (Creates isolated free block)...\n");
    kfree(p2);

    printk("[~] Freeing p3 (Forces FORWARD coalesce: p2 + p3)...\n");
    kfree(p3);

    printk("[~] Freeing p1 (Forces BACKWARD coalesce: p1 + p2/p3)...\n");
    kfree(p1);

    printk("[~] Freeing guard block p4...\n");
    kfree(p4);

    printk("Phase 1 Complete. Heap should show consolidated free space.\n");
    simple_mm_dump();
}

/* -------------------------------------------------------------------
 * Phase 2: The "Swiss Cheese" Stress Test
 * Allocates highly variable sizes, then frees them out of order.
 * ------------------------------------------------------------------- */
static void test_swiss_cheese_stress(void) {
    printk("\n--- PHASE 2: Swiss Cheese Stress Test ---\n");
    
    void *ptrs[STRESS_TEST_ITERATIONS];
    size_t allocated_sizes[STRESS_TEST_ITERATIONS];
    uint32_t successful_allocs = 0;

    printk("[+] Allocating %d random-sized blocks...\n", STRESS_TEST_ITERATIONS);
    
    /* 1. Allocate a massive array of random sizes */
    for (int i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        /* Generate random size between MIN and MAX */
        size_t size = (bare_rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE + 1)) + MIN_ALLOC_SIZE;
        
        ptrs[i] = kmalloc(size);
        allocated_sizes[i] = size;

        if (ptrs[i] != NULL) {
            successful_allocs++;
        } else {
            printk("[!] OOM hit at iteration %d (Requested %zu bytes)\n", i, size);
            break;
        }
    }
    
    printk("[+] Successfully allocated %u blocks.\n", successful_allocs);
    
    /* 2. Free Phase A: Evens (Creates massive fragmentation) */
    printk("[~] Freeing all EVEN indexed blocks (Creating fragmentation)...\n");
    for (int i = 0; i < successful_allocs; i += 2) {
        if (ptrs[i]) {
            kfree(ptrs[i]);
            ptrs[i] = NULL;
        }
    }

    /* 3. Free Phase B: Odds (Forces total bidirectional coalescing) */
    printk("[~] Freeing all ODD indexed blocks (Triggering massive coalescing)...\n");
    for (int i = 1; i < successful_allocs; i += 2) {
        if (ptrs[i]) {
            kfree(ptrs[i]);
            ptrs[i] = NULL;
        }
    }

    printk("Phase 2 Complete. Heap must be perfectly unified.\n");
    simple_mm_dump();
}

/* -------------------------------------------------------------------
 * Main Public Entry Point
 * Call this from init/main.c right after simple_init()
 * ------------------------------------------------------------------- */
void run_mm_tests(void) {
    printk("\n======================================================\n");
    printk("           STARTING KERNEL MEMORY STRESS TESTS          \n");
    printk("======================================================\n");

    test_deterministic_coalesce();
    test_swiss_cheese_stress();

    printk("\n======================================================\n");
    printk("           MEMORY STRESS TESTS COMPLETED                \n");
    printk("======================================================\n");
}
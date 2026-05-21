/* Simple Memory Manager Implementation simple_mm.c*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "kernel/printk.h"
#include "kernel/assert.h"
#include "kernel/fatal.h"
#include "kernel/mm/simple_mm.h"
#include "string.h"

struct list_node free_mem_list_head;


void print_mm_blk(struct simple_mm_block *block)
{
    printk(" mem block at=0x%zx  size=0x%zx, flags=0x%04x magic=0x%08x\n",\
         (size_t)block, EXTRACT_SIZE(block->size_flag), EXTRACT_FLAG(block->size_flag), block->magic);
}

static inline size_t align_requested_size(size_t size)
{
    if (size < MIN_BLOCK_SIZE) {
        printk("[simple_mm] Warning: Requested size %zu is below minimum block size %zu. Rounding up.\n", size, MIN_BLOCK_SIZE);
        size = MIN_BLOCK_SIZE;
    } else if (size % MIN_BLOCK_SIZE != 0) {
        size_t aligned_size = (size + MIN_BLOCK_SIZE - 1) & (~(MIN_BLOCK_SIZE - 1));
        printk("[simple_mm] Warning: Requested size %zu is not aligned to minimum block size %zu. Rounding up to %zu.\n", size, MIN_BLOCK_SIZE, aligned_size);
        size = aligned_size;
    }
    return size;
}

bool simple_init(void *base, size_t size)
{
    printk("[simple_init] base=%p and size=%zu bytes\n", base, size);

    printk("[simple_init] block_head_struct_size=%zu\n", BLOCK_HEAD_SIZE);
    if (size < BLOCK_HEAD_SIZE) {
        printk("[simple_init] Not enough memory to initialize\n");
        return false; // Not enough memory to initialize
    }
    if (base == NULL) {
        printk("[simple_init] Invalid base pointer \n");
        return false; // Invalid base pointer
    }
    
    if(size % MIN_BLOCK_SIZE != 0) {
        printk("[simple_init] Warning: Provided size %zu is not aligned to minimum block size %zu. Rounding down.\n", size, MIN_BLOCK_SIZE);
        size &= SIZE_MASK;
    }

    struct simple_mm_block *block = (struct simple_mm_block *)base;
    block->magic = SIMPLE_MM_MAGIC;

    size -= BLOCK_HEAD_SIZE; // make room for the end block head
    // Mark the entire region as one big free block, as FREE PREV not free not poisoned.
    block->size_flag = (size | FLAG_FREE);
    block->size_flag &= ~(FLAG_PREV_FREE_MASK | FLAG_POISONED_MASK); // Mark as not having a free previous block and not poisoned
#ifdef CONFIG_MM_DEBUG
    memset((void *)(block + 1), SIMPLE_MM_POISON, size - BLOCK_HEAD_SIZE ); // Fill free memory with 0xAA for debugging
    block->size_flag |= FLAG_POISONED; // Mark the block as poisoned for debugging
#endif
    // write a size 0 head at the end of heap ram
    struct simple_mm_block *end_block = (struct simple_mm_block *)((uint8_t *)base + size);
    end_block->magic = SIMPLE_MM_MAGIC;
    end_block->size_flag = 0 | FLAG_PREV_FREE; // Size 0, not free, not poisoned
    list_init(&free_mem_list_head);
    list_add(&block->mm_node, &free_mem_list_head);
    simple_mm_dump();
    return true;
}


void *simple_malloc(size_t size)
{
    void *alloced_ptr = NULL;

    struct list_node *iterator;
    size_t free_block_size=0;
    struct simple_mm_block *block_head=NULL;
    
    size_t aligned_size = align_requested_size(size);
    list_for_each(iterator, &free_mem_list_head) {
        block_head = container_of(iterator, struct simple_mm_block, mm_node);
        free_block_size = EXTRACT_SIZE(block_head->size_flag);
        if (IS_BLOCK_FREE(block_head->size_flag) && free_block_size >= (aligned_size + BLOCK_HEAD_SIZE)) {
            // Found a suitable block
            // allocate the block, split if necessary, and update the free list
            alloced_ptr = (void *)(block_head + 1);
            CLR_MASK(block_head->size_flag, FLAG_FREE); // Clear free flag,Mark as used
            list_del(&block_head->mm_node);
            struct simple_mm_block *new_block_head=NULL;
            size_t remaining_size = free_block_size - BLOCK_HEAD_SIZE - aligned_size;
            if (remaining_size > (BLOCK_HEAD_SIZE + MIN_BLOCK_SIZE)) {
                
                // Split the block
                new_block_head = (struct simple_mm_block *)( (uint8_t *)(block_head + 1) + aligned_size);
                new_block_head->magic = SIMPLE_MM_MAGIC;
                // write the footer
                SIZE_MEMBER_TYPE *ptr_footer = (SIZE_MEMBER_TYPE *)((uint8_t *)new_block_head + remaining_size - SIZE_MEMBER_TYPE_SIZE);
                *ptr_footer = remaining_size;
                new_block_head->size_flag = remaining_size;
                SET_MASK(new_block_head->size_flag, FLAG_FREE); // Mark the new block as free
                CLR_MASK(new_block_head->size_flag, FLAG_PREV_FREE | FLAG_POISONED); // Mark
                
                list_add(&new_block_head->mm_node, &free_mem_list_head); 

                // Update the allocated block's size and flags
                size_t old_flags = EXTRACT_FLAG(block_head->size_flag);
                block_head->size_flag = ((aligned_size + BLOCK_HEAD_SIZE) & SIZE_MASK) | old_flags; 
                CLR_MASK(block_head->size_flag, FLAG_FREE); // Mark the allocated block as not having a free previous block and not poisoned
            } else {
                // Not enough space to split,  entire block allocated, notfiy next block the previous block is not free
                new_block_head = (struct simple_mm_block *)((uint8_t *)block_head + free_block_size);
                if (new_block_head->magic != SIMPLE_MM_MAGIC) {
                    pr_err(" memory corruption at %p\n", alloced_ptr);
                    return NULL; // Next block is corrupted, possible memory corruption
                } else {
                     // Mark the next block's PREV_FREE flag as not free
                    CLR_MASK(new_block_head->size_flag, FLAG_PREV_FREE); // Mark the next block's PREV_FREE flag as not free
                }
                
            }
#ifdef CONFIG_MM_DEBUG
            printk("[simple_malloc] Allocated block at %p with size %zu bytes (requested %zu bytes)\n",\
                 alloced_ptr, EXTRACT_SIZE(new_block_head->size_flag), size);
#endif
            break;
        }
    }
    return alloced_ptr;
}

void ensure_prev_block_coalescing(struct simple_mm_block *block_to_free)
{
    struct simple_mm_block *prev_block = NULL;
    
    // Check previous block
    SIZE_MEMBER_TYPE *ptr_prev_block_size_flag=NULL;
    size_t size_of_the_member = SIZE_MEMBER_TYPE_SIZE;
    ptr_prev_block_size_flag = (SIZE_MEMBER_TYPE *) ((uint8_t *)block_to_free - size_of_the_member);
    size_t prev_block_size = EXTRACT_SIZE(*ptr_prev_block_size_flag);
    
    prev_block = (struct simple_mm_block *)((uint8_t *)block_to_free - prev_block_size);
    if (prev_block->magic != SIMPLE_MM_MAGIC){
        pr_err("Previous block corruption detected at %p\n", prev_block);
        k_panic();
    } else if(!IS_BLOCK_FREE(prev_block->size_flag)) {
        // previos block is supposed to be free, but it's not marked as free, possible corruption
        pr_err("Previous block at %p is supposed to be free. Possible corruption.\n", prev_block);
        k_panic();
    } else {
        // previos block is free, Coalesce with previous block
        size_t prev_flag = EXTRACT_FLAG(prev_block->size_flag);
        size_t prev_size = EXTRACT_SIZE(prev_block->size_flag);
        size_t new_size = prev_size + EXTRACT_SIZE(block_to_free->size_flag);
        prev_block->size_flag = (new_size | prev_flag);
        list_del(&block_to_free->mm_node);

        // write the footer
        SIZE_MEMBER_TYPE *ptr_footer = (SIZE_MEMBER_TYPE *)((uint8_t *)prev_block + new_size - SIZE_MEMBER_TYPE_SIZE);
        *ptr_footer = new_size;
    }
}

void simple_free(void *ptr)
{
#ifdef CONFIG_MM_DEBUG
    printk("[simple_free] Attempting to free block at %p\n", ptr);
#endif
    __ASSERT(ptr != NULL, "[simple_free] Error: Attempt to free a NULL pointer");

    struct simple_mm_block *block_to_free = (struct simple_mm_block *)ptr - 1; // Get the block header
    if (block_to_free->magic != SIMPLE_MM_MAGIC) {
        printk("[simple_free] Error: Invalid block magic number. Possible double free or corruption at %p\n", ptr);
        k_panic();
    }
    if (IS_BLOCK_FREE(block_to_free->size_flag)) {
        printk("[simple_free] Warning: Block at %p is already free. Possible double free\n", ptr);
        k_panic();
    }

    block_to_free->size_flag |= FLAG_FREE; // Mark the block as free
    SIZE_MEMBER_TYPE size_to_free=EXTRACT_SIZE(block_to_free->size_flag);
    list_add(&block_to_free->mm_node, &free_mem_list_head);

    // Check next block for coalescing
    struct simple_mm_block *next_block = (struct simple_mm_block *)((uint8_t *)block_to_free + size_to_free);
    if ((next_block->magic != SIMPLE_MM_MAGIC)) {
        pr_err(" memory corruption at %p\n", ptr);
        k_panic();
    } else if (!IS_BLOCK_FREE(next_block->size_flag)) {
        // Next block is not free,
        next_block->size_flag |= FLAG_PREV_FREE; // Mark the next block's PREV_FREE flag
        // write the footer
        SIZE_MEMBER_TYPE *ptr_footer = (SIZE_MEMBER_TYPE *)((uint8_t *)block_to_free + size_to_free - SIZE_MEMBER_TYPE_SIZE);
        *ptr_footer = size_to_free;
        
    } else {
        // Next block is free, merge with current block to free
        size_t new_size = EXTRACT_SIZE(block_to_free->size_flag) + EXTRACT_SIZE(next_block->size_flag);
        size_t flag = EXTRACT_FLAG(block_to_free->size_flag);
        block_to_free->size_flag = (new_size | flag);
        list_del(&next_block->mm_node); // Remove the next block from the free list
        SIZE_MEMBER_TYPE *ptr_footer = (SIZE_MEMBER_TYPE *)((uint8_t *)block_to_free + new_size - SIZE_MEMBER_TYPE_SIZE);
        *ptr_footer = new_size;

    }

    if (IS_PREV_BLOCK_FREE(block_to_free->size_flag)) {
        ensure_prev_block_coalescing(block_to_free);
    }
}

void *simple_calloc(size_t num, size_t size)
{
    /* 1. Trap Integer Overflow */
    if (num != 0 && size > (size_t)-1 / num) {
        printk("[simple_calloc] Error: Integer overflow detected!\n");
        return NULL;
    }

    size_t total_size = num * size;

    /* 2. Allocate the memory using your engine */
    void *ptr = simple_malloc(total_size);
    
    /* 3. Zero out the payload if allocation succeeded */
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void *simple_realloc(void *ptr, size_t new_size)
{
    if (ptr == NULL) {
        return simple_malloc(new_size); // realloc with NULL ptr should behave like malloc
    }
    if(new_size == 0) {
        simple_free(ptr); // realloc with size 0 should free the block
        return NULL;
    }


    struct simple_mm_block *old_block_head = (struct simple_mm_block *)ptr - 1; // Get the block header

    if (old_block_head->magic != SIMPLE_MM_MAGIC) {
        printk("[simple_realloc] Error: Invalid block magic number. Possible corruption at %p\n", ptr);
        return NULL; // Invalid block, possible corruption
    }

    size_t old_size = EXTRACT_SIZE(old_block_head->size_flag);

    if (new_size <= old_size) {
        // If the new size is smaller or equal, we can just return the same pointer
        return ptr;
    }
    
    void *new_ptr = simple_malloc(new_size);
    if (new_ptr == NULL) {
        return NULL; // Allocation failed
    }
    memcpy(new_ptr, ptr, old_size); // Copy old data to new block
    simple_free(ptr); // Free the old block
    return new_ptr;
}


void simple_mm_dump(void)
{
    struct list_node *iterator;
    struct simple_mm_block *block;
    printk("Simple MM Dump:\n");
    list_for_each(iterator, &free_mem_list_head) {
        block = container_of(iterator, struct simple_mm_block, mm_node);
        print_mm_blk(block);
    }

}

size_t simple_mm_get_free_memory(void)
{
    struct list_node *iterator;
    struct simple_mm_block *block;
    size_t free_memory = 0;
    list_for_each(iterator, &free_mem_list_head) {
        block = container_of(iterator, struct simple_mm_block, mm_node);
        free_memory += EXTRACT_SIZE(block->size_flag);
        print_mm_blk(block);
    }
    return free_memory;
}


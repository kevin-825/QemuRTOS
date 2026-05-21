#ifndef _SIMPLE_MM_H
#define _SIMPLE_MM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mylib/list/list.h"
#include "mylib/bitops.h"

#define SIMPLE_MM_MAGIC  0xDEADBEEF
#define SIMPLE_MM_POISON 0xAAAAAAAAAAAAAAAA

#define SIMPLE_MM_FLAG_WIDTH 3
#define MIN_BLOCK_SIZE (1UL << SIMPLE_MM_FLAG_WIDTH)
#define FLAG_MASK          ((1UL << SIMPLE_MM_FLAG_WIDTH) - 1) 
#define SIZE_MASK          (~FLAG_MASK)

#define FLAG_FREE_BIT       0
#define FLAG_PREV_FREE_BIT  1
#define FLAG_POISONED_BIT   2

 
#define FLAG_FREE_MASK      BIT2MASK(FLAG_FREE_BIT)
#define FLAG_PREV_FREE_MASK BIT2MASK(FLAG_PREV_FREE_BIT)
#define FLAG_POISONED_MASK  BIT2MASK(FLAG_POISONED_BIT)

#define FLAG_FREE           FLAG_FREE_MASK
#define FLAG_PREV_FREE      FLAG_PREV_FREE_MASK
#define FLAG_POISONED       FLAG_POISONED_MASK



#define IS_BLOCK_FREE(size_flag)       (GET_BIT(size_flag, FLAG_FREE_BIT) != 0)
#define IS_PREV_BLOCK_FREE(size_flag)  (GET_BIT(size_flag, FLAG_PREV_FREE_BIT) != 0)
#define IS_BLOCK_POISONED(size_flag)   (GET_BIT(size_flag, FLAG_POISONED_BIT) != 0)

#define EXTRACT_SIZE(size_flag)    GET_MASK(size_flag, SIZE_MASK)
#define EXTRACT_FLAG(size_flag)    GET_MASK(size_flag, FLAG_MASK)


// this may lover memory performance but make the code simpler
// struct __attribute__((packed)) simple_mm_block

struct simple_mm_block {
    uint32_t magic; // Magic number to identify the block
    uint32_t size_flag; // Size of the block
    struct list_node mm_node; // List node for linking blocks
};


#define BLOCK_HEAD_SIZE sizeof(struct simple_mm_block)
#define SIZE_MEMBER_TYPE typeof( ((struct simple_mm_block *)0)->size_flag )
#define SIZE_MEMBER_TYPE_SIZE sizeof( ((struct simple_mm_block *)0)->size_flag )


// Function prototypes for simple memory management
bool simple_init(void *base, size_t size);
void *simple_malloc(size_t size);
void simple_free(void *ptr);

void *simple_calloc(size_t num, size_t size);
void *simple_realloc(void *ptr, size_t new_size);


void simple_mm_dump(void);
size_t simple_mm_get_free_memory(void);

#endif /* _SIMPLE_MM_H */

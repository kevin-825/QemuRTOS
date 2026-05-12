#ifndef _KERNEL_MM_KMALLOC_H
#define _KERNEL_MM_KMALLOC_H

#include <stddef.h>

/* -------------------------------------------------------------------
 * Generic Kernel Memory Allocation API
 * ------------------------------------------------------------------- */
void *kmalloc(size_t size);
void kfree(void *ptr);
void *kcalloc(size_t num, size_t size);
void *krealloc(void *ptr, size_t new_size);

#endif /* _KERNEL_MM_KMALLOC_H */
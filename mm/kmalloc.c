#include "kernel/mm/kmalloc.h"

/* Include the specific allocator headers based on Kconfig selection */
#ifdef CONFIG_MM_SIMPLE
#include "kernel/mm/simple_mm.h"
#endif

/* * If you add TLSF or SLUB later in Kconfig, you include them here:
 * #ifdef CONFIG_MM_TLSF
 * #include "kernel/mm/tlsf.h"
 * #endif
 */

void *kmalloc(size_t size)
{
#ifdef CONFIG_MM_SIMPLE
    return simple_malloc(size);
#elif defined(CONFIG_MM_TLSF)
    /* return tlsf_malloc(size); */
    return NULL;
#else
    /* Fallback if no allocator is configured */
    return NULL; 
#endif
}

void kfree(void *ptr)
{
#ifdef CONFIG_MM_SIMPLE
    simple_free(ptr);
#elif defined(CONFIG_MM_TLSF)
    /* tlsf_free(ptr); */
#endif
}

void *kcalloc(size_t num, size_t size)
{
#ifdef CONFIG_MM_SIMPLE
    return simple_calloc(num, size);
#else
    return NULL;
#endif
}

void *krealloc(void *ptr, size_t new_size)
{
#ifdef CONFIG_MM_SIMPLE
    return simple_realloc(ptr, new_size);
#else
    return NULL;
#endif
}
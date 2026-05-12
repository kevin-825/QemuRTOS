#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

/* 
 * We use compiler built-ins to ensure size_t matches 
 * the target architecture (32-bit vs 64-bit) automatically.
 */


/* Unsigned type for sizes (sizeof operator) */
typedef __SIZE_TYPE__ size_t;

/* Signed type for sizes (returns -1 on error) */
typedef signed __SIZE_TYPE__ ssize_t;

/* Signed type for pointer subtraction */
typedef __PTRDIFF_TYPE__ ptrdiff_t;

/* Unsigned type for memory addresses */
typedef __UINTPTR_TYPE__ uintptr_t;

/* Signed type for pointer storage */
typedef __INTPTR_TYPE__ intptr_t;



// For RISC-V 64-bit
// #ifdef __riscv_xlen == 64
//     typedef unsigned long size_t;
//     typedef long ssize_t;
// #else
//     // For RISC-V 32-bit
//     typedef unsigned int size_t;
//     typedef int ssize_t;
// #endif

/* 
 * We use the signed attribute to create the signed version 
 * of the size_t type automatically. 
 */



#endif
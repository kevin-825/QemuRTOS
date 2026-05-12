#ifndef _MYLIB_BITOPS_H
#define _MYLIB_BITOPS_H

/* -------------------------------------------------------------------
 * Converts a bit index (0, 1, 2) into a mask (0x1, 0x2, 0x4).
 * Uses '1UL' to guarantee 64-bit safety on your RISC-V architecture.
 * ------------------------------------------------------------------- */
#define BIT2MASK(n)             (1UL << (n))

/* -------------------------------------------------------------------
 * Index-Based Operations 
 * (Provide the raw bit number: 0, 1, 2...)
 * ------------------------------------------------------------------- */
#define SET_BIT(value, bit)     ((value) |= BIT2MASK(bit))
#define GET_BIT(value, bit)     (((value) >> (bit)) & 1UL)
#define CLEAR_BIT(value, bit)   ((value) &= ~BIT2MASK(bit))
#define TOGGLE_BIT(value, bit)  ((value) ^= BIT2MASK(bit))
#define TEST_BIT(value, bit)    (((value) & BIT2MASK(bit)) != 0)

/* -------------------------------------------------------------------
 * Mask-Based Operations 
 * (Provide the pre-calculated mask: 0x1, 0xFF, FLAG_FREE, etc.)
 * ------------------------------------------------------------------- */
#define SET_MASK(value, mask)   ((value) |= (mask))
#define GET_MASK(value, mask)   ((value) & (mask))
#define CLR_MASK(value, mask) ((value) &= ~(mask))
#define TOGGLE_MASK(value, mask) ((value) ^= (mask))
#define TEST_MASK(value, mask)  (((value) & (mask)) == (mask))
#define TEST_ANY_MASK(value, mask) (((value) & (mask)) != 0)

#define CLEAR_MASK CLR_MASK


/* -------------------------------------------------------------------
 * Extracts a multi-bit value from the middle of a register.
 * Requires the mask, and the index of the lowest bit in that mask.
 * ------------------------------------------------------------------- */
#define EXTRACT_FIELD(value, mask, shift) (((value) & (mask)) >> (shift))

/* -------------------------------------------------------------------
 * Snaps a 'size' up to the nearest multiple of 'alignment'.
 * WARNING: 'alignment' MUST be a power of 2 (e.g., 8, 16, 32)!
 * ------------------------------------------------------------------- */
#define ALIGN_UP(size, alignment) (((size) + (alignment) - 1) & ~((alignment) - 1))

#endif /* _MYLIB_BITOPS_H */

/* include/kernel/irq.h */
#ifndef _KERNEL_IRQ_H
#define _KERNEL_IRQ_H

extern volatile int interrupt_nesting_count;
static inline bool is_in_isr()
{
    return interrupt_nesting_count > 0;
}
/* ==========================================================================
 * 1. ARCHITECTURE ROUTING
 * Pull in the blazing-fast 'static inline' assembly for the active silicon.
 * ========================================================================== */
#if defined(CONFIG_ARCH_RISCV)
    #include "arch/riscv/irq.h"
#elif defined(CONFIG_ARCH_ARM)
    #include "arch/arm/irq.h"
#elif defined(CONFIG_ARCH_ARM64)
    #include "arch/arm64/irq.h"
#else
    #error "Target architecture not supported or missing from Kconfig!"
#endif

/* ==========================================================================
 * 2. GLOBAL CPU INTERRUPT LOCKS
 * Used to protect critical sections and spinlocks. These disable the CPU's
 * ability to hear ANY interrupts.
 * ========================================================================== */

/* *
 * @brief Lock all interrupts on the current CPU.
 * @return An architecture-specific key representing the previous interrupt state.
 */
#define irq_lock() arch_irq_lock()

/* *
 * @brief Unlock interrupts using the previously saved state.
 * @param key The state key returned by irq_lock().
 */
#define irq_unlock(key) arch_irq_unlock(key)

/* ==========================================================================
 * 3. PERIPHERAL INTERRUPT CONTROLLER (PLIC/GIC) LINE ROUTING
 * Used by device drivers to unmute specific hardware lines (e.g., UART, Timer).
 * ========================================================================== */

/* *
 * @brief Enable a specific hardware interrupt line.
 * @param irq The interrupt line number (e.g., PLIC trap number).
 */
#define irq_enable(irq) arch_irq_enable(irq)

/* *
 * @brief Disable a specific hardware interrupt line.
 * @param irq The interrupt line number.
 */
#define irq_disable(irq) arch_irq_disable(irq)

/* *
 * @brief Check if a specific hardware interrupt line is currently enabled.
 * @param irq The interrupt line number.
 * @return true if enabled, false otherwise.
 */
#define irq_is_enabled(irq) arch_irq_is_enabled(irq)

/* ==========================================================================
 * 4. FATAL ERROR ROUTING
 * ========================================================================== */

/* *
 * @brief Permanently halt the CPU core (Wait For Interrupt).
 * implemented in arch/<arch>/core/fatal.c
 */
void arch_halt(void);


#endif /* _KERNEL_IRQ_H */
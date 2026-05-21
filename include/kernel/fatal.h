#ifndef _KERNEL_FATAL_H
#define _KERNEL_FATAL_H

#include "arch/exception.h"


enum k_fatal_error_reason {
	/** Generic CPU exception, not covered by other codes */
	K_ERR_CPU_EXCEPTION,

	/** Unhandled hardware interrupt */
	K_ERR_SPURIOUS_IRQ,

	/** Faulting context overflowed its stack buffer */
	K_ERR_STACK_CHK_FAIL,

	/** Moderate severity software error */
	K_ERR_KERNEL_OOPS,

	/** High severity software error */
	K_ERR_KERNEL_PANIC,

	/** Arch specific fatal errors */
	K_ERR_ARCH_START = 16
};

void kernel_fatal_error(unsigned int reason, const struct arch_esf *esf);

#endif // _KERNEL_FATAL_H   
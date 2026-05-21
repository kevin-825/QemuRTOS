#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#define K_SYSCALL_YIELD          0
#define K_SYSCALL_UART_WRITE     1

#define K_SYSCALL_USER_FAULT     0xff   /* <-- Your user-mode panic ID */

typedef uintptr_t (*_k_syscall_handler_t)(uintptr_t arg1, uintptr_t arg2,
					  uintptr_t arg3, uintptr_t arg4,
					  uintptr_t arg5, uintptr_t arg6,
					  void *ssf);

static inline bool k_is_user_context(void)
{
	return arch_is_user_context();
}



#endif /* _KERNEL_SYSCALL_H */

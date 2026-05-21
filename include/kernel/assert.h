#ifndef _KERNEL_ASSERT_H
#define _KERNEL_ASSERT_H

#include "kernel/fatal.h"
#include "kernel/kernel.h"

#define Z_STRINGIFY(x) #x
#define STRINGIFY(s) Z_STRINGIFY(s)

#ifdef CONFIG_ASSERT

#define __ASSERT_PRINT(fmt, ...) printk(fmt, ##__VA_ARGS__)
#define __ASSERT_MSG_INFO(fmt, ...) __ASSERT_PRINT("\t" fmt "\n", ##__VA_ARGS__)

#define __ASSERT_LOC(test)                              \
	__ASSERT_PRINT("ASSERTION FAIL [%s] @ %s:%d\n", \
		Z_STRINGIFY(test),                      \
		__FILE__, __LINE__)

#define __ASSERT_POST_ACTION() \
    do { \
        k_panic(); \
    } while (0)

#define __ASSERT_UNREACHABLE do { __builtin_unreachable(); } while (0)

#define __ASSERT(test, fmt, ...) \
    do { \
        if (!(test)) { \
            __ASSERT_LOC(test);                               \
			__ASSERT_MSG_INFO(fmt, ##__VA_ARGS__);            \
			__ASSERT_POST_ACTION();                           \
			__ASSERT_UNREACHABLE;                             \
        } \
    } while (0)

#define __ASSERT_NO_MSG(test) \
    do { \
        if (!(test)) { \
            __ASSERT_LOC(test); \
            __ASSERT_POST_ACTION(); \
            __ASSERT_UNREACHABLE; \
        } \
    } while (0)

#else

/* * Zero-cost abstractions for production builds. 
 * (void)(test) prevents "unused variable" compiler warnings when asserts are disabled!
 */

#define __ASSERT(test, fmt, ...) { }
#define __ASSERT_NO_MSG(test) { }
#define __ASSERT_POST_ACTION() { }
#define __ASSERT_UNREACHABLE { }

#endif /* CONFIG_ASSERT */

#endif /* _KERNEL_ASSERT_H */



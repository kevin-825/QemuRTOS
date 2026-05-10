#ifndef INIT_H
#define INIT_H

#include <stdint.h>

/* include/init.h */
typedef void (*initcall_t)(void);

/* This macro marks the function pointer for the linker */
#define __initcall(fn) \
    static initcall_t __initcall_##fn \
    __attribute__((__used__, __section__(".initcalls"))) = fn

/* Linux-style aliases */
#define device_initcall(fn)      __initcall(fn)
#define subsys_initcall(fn)      __initcall(fn)

#endif /* INIT_H */

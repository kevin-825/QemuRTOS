# lib/lib.mk

# Only compile libfdt if the user explicitly chose the Dynamic strategy
ifeq ($(CONFIG_DTB_DYNAMIC),y)

    # 1. Export the libfdt headers globally
    CFLAGS += -Ilib/libfdt

    # 2. Add the libfdt source files to the build
    SRCS += lib/libfdt/fdt.c
    SRCS += lib/libfdt/fdt_ro.c
    SRCS += lib/libfdt/fdt_wip.c
    SRCS += lib/libfdt/fdt_sw.c
    SRCS += lib/libfdt/fdt_rw.c
    SRCS += lib/libfdt/fdt_strerror.c
    SRCS += lib/libfdt/fdt_empty_tree.c
    SRCS += lib/libfdt/fdt_addresses.c
    SRCS += lib/libfdt/fdt_overlay.c

endif

CFLAGS += -Ilib/string

libdir := $(dir $(lastword $(MAKEFILE_LIST)))

SRCS += $(wildcard $(libdir)string/*.c)
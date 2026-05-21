# lib/lib.mk

# Only compile libfdt if the user explicitly chose the Dynamic strategy
ifeq ($(CONFIG_DTB_DYNAMIC),y)

    # 1. Export the libfdt headers globally
    CFLAGS += -Ilib/libfdt

    # 2. Add the libfdt source files to the build
    obj-y += lib/libfdt/fdt.o
    obj-y += lib/libfdt/fdt_ro.o
    obj-y += lib/libfdt/fdt_wip.o
    obj-y += lib/libfdt/fdt_sw.o
    obj-y += lib/libfdt/fdt_rw.o
    obj-y += lib/libfdt/fdt_strerror.o
    obj-y += lib/libfdt/fdt_empty_tree.o
    obj-y += lib/libfdt/fdt_addresses.o
    obj-y += lib/libfdt/fdt_overlay.o

endif

CFLAGS += -Ilib

include lib/mylib/mylib.mk
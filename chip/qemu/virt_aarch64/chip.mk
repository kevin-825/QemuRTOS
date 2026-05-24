
LDFLAGS += -T chip/$(CHIP_DIR)/linker.ld
CFLAGS += -mgeneral-regs-only -mstrict-align

obj-y += chip/$(CHIP_DIR)/startup.o


QEMU_RUN_CMD_PREFIX := \
qemu-system-aarch64 -M virt,virtualization=on,secure=on \
    -cpu cortex-a76 -m 1024M -nographic \
    -chardev stdio,id=con,mux=on \
	-serial chardev:con \
	-mon chardev=con,mode=readline

# CFLAGS += -Ichip/$(CHIP_DIR)/include

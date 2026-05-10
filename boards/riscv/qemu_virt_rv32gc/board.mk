BOARD_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

QEMU_RUN_CMD_PREFIX := \
qemu-system-riscv32 -nographic -machine virt -net none \
-chardev stdio,id=con,mux=on \
-serial chardev:con \
-mon chardev=con,mode=readline \
-bios none -smp 4

BOARD_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

QEMU_RUN_CMD_PREFIX := \
qemu-system-arm -M virt,virtualization=on,secure=on \
-cpu cortex-a15 -m 1024M -nographic


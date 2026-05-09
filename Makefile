# QemuRTOS / Top-Level Makefile

# 1. Load the Kconfig generated configuration
# The '-' prevents make from crashing if .config doesn't exist yet
-include .config

# 2. Determine Architecture and Toolchain Prefix

# Strip the double quotes that Kconfiglib puts around strings

# Strip the double quotes that Kconfiglib puts around strings
CROSS_COMPILE    ?= $(patsubst "%",%,$(CONFIG_CROSS_COMPILE))
ARCH             ?= $(patsubst "%",%,$(CONFIG_ARCH_NAME))
ARCH_SIZE        ?= $(if $(CONFIG_64BIT),64,32)

# 3. Define the Toolchain
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
GDB     = $(CROSS_COMPILE)gdb

# 4. Base Compiler Flags (Baremetal RTOS standard)
CFLAGS = -Wall -Wextra -O0 -g3 \
         -ffreestanding -nostdlib -nostartfiles \
         -Iinclude

# 5. Build Targets
.PHONY: all menuconfig clean check-config

all: check-config qemurtos.elf

# Ensure the user has run menuconfig before trying to build
check-config:
	@if [ ! -f .config ]; then \
		echo "==============================================================="; \
		echo " [!] ERROR: .config not found."; \
		echo "     Please run 'make menuconfig' to select a board first."; \
		echo "==============================================================="; \
		exit 1; \
	fi

# This will eventually link all the compiled .o files together
qemurtos.elf:
	@echo "========================================"
	@echo " Building QemuRTOS"
	@echo " Architecture : $(ARCH)"
	@echo " Arch Size    : $(ARCH_SIZE)"
	@echo " Compiler     : $(CC)"
	@echo "========================================"
	@echo " [SUCCESS] Toolchain routing works!"

# Kconfig tools from pip install kconfiglib
menuconfig:
	menuconfig Kconfig

clean:
	rm -f *.elf *.o
	rm -f .config .config.old
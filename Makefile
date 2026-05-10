# QemuRTOS / Top-Level Makefile

# 1. Load the Kconfig generated configuration
# The '-' prevents make from crashing if .config doesn't exist yet
-include .config
output_dir := /mnt/wsl/ramdisk5/qemurtos
PROJECT_ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))



# 2. Determine Architecture and Toolchain Prefix
# Strip the double quotes that Kconfiglib puts around strings
CROSS_COMPILE ?= $(patsubst "%",%,$(CONFIG_CROSS_COMPILE))
ARCH          ?= $(patsubst "%",%,$(CONFIG_ARCH_NAME))
CHIP_DIR      ?= $(patsubst "%",%,$(CONFIG_CHIP_DIR))
BOARD_DIR     ?= $(patsubst "%",%,$(CONFIG_BOARD_DIR))
ARCH_SIZE     ?= $(if $(CONFIG_64BIT),64,32)
QEMU_RUN_CMD_PREFIX ?= $(patsubst "%",%,$(CONFIG_QEMU_RUN_CMD_PREFIX)) 


TARGET_DIR := $(output_dir)/$(ARCH)
TARGET     := $(TARGET_DIR)/qemurtos.elf
# 3. Define the Toolchain
CC      = $(CROSS_COMPILE)gcc
AS      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
SIZE    = $(CROSS_COMPILE)size
OBJDUMP = $(CROSS_COMPILE)objdump
GDB     = $(CROSS_COMPILE)gdb
STRIP   = $(CROSS_COMPILE)strip
OBJCPY  = $(CROSS_COMPILE)objcopy
QEMU_RUN= $(QEMU_RUN_CMD_PREFIX) --kernel $(TARGET) -s 

# 4. Base Compiler Flags (Baremetal RTOS standard)
CFLAGS = -Wall -Wextra -O0 -g3 \
         -ffreestanding -nostdlib -nostartfiles \
         -Iinclude 

LDFLAGS = -nostdlib -nostartfiles

# 5. Modular Flag Injection
# We use -include so Make doesn't crash if a folder doesn't have a flags.mk
# 5. Modular Build Scripts (Flags + Sources)
SRCS := 

# Include the explicitly named modular scripts
-include arch/$(ARCH)/arch.mk
-include chip/$(CHIP_DIR)/chip.mk
-include board/$(BOARD_DIR)/board.mk
-include kernel/kernel.mk
-include init/init.mk
-include drivers/drivers.mk
-include subsys/subsys.mk
-include lib/lib.mk
-include include/include.mk

OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)
OBJS := $(OBJS:.s=.o)
OBJS := $(addprefix $(TARGET_DIR)/, $(OBJS))
#$(info  OBJS:$(OBJS))
DEPS := $(OBJS:.o=.d)
CFLAGS += -MMD -MP

.PHONY: all menuconfig clean check-config

all: check-config $(TARGET)

# Ensure the user has run menuconfig before trying to build
check-config:
	@if [ ! -f .config ]; then \
		echo "==============================================================="; \
		echo " [!] ERROR: .config not found."; \
		echo "     Please run 'make menuconfig' to select a board first."; \
		echo "==============================================================="; \
		exit 1; \
	fi

$(TARGET_DIR)/%: 
	@mkdir -p $(@D)

	
# This will eventually link all the compiled .o files together
$(TARGET):$(OBJS)
	@echo "========================================"
	@echo " Building QemuRTOS"
	@echo " Architecture : $(ARCH)"
	@echo " Arch Size    : $(ARCH_SIZE)"
	@echo " Board        : $(BOARD_DIR)"
	@echo " Chip         : $(CHIP_DIR)"
	@echo " Compiler     : $(CC)"
	@echo "========================================"
	@echo " [SUCCESS] Toolchain routing works! src files:"
	@echo "$(SRCS)" | tr ' ' '\n' | sort |sed 's/ /\n/g' 
	@echo "\nCFLAGS: $(CFLAGS)\n "
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "======================================="
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET_DIR)/qemurtos.elf
	$(SIZE) $(TARGET_DIR)/qemurtos.elf
	@sh -c '$(OBJDUMP) -d -S $(TARGET_DIR)/qemurtos.elf > $(TARGET_DIR)/qemurtos.elf.s'
	@echo " [SUCCESS] Build complete! Output at $(TARGET_DIR)/qemurtos.elf"
	@echo "======================================="
	@echo "QEMU_RUN_CMD_PREFIX: $(QEMU_RUN_CMD_PREFIX) --kernel $(TARGET)"
	./scripts/auto_gdbinit.sh $(PROJECT_ROOT) $(TARGET_DIR) $(CROSS_COMPILE) $(TARGET) $(ARCH) $(ARCH_SIZE) $(BOARD_DIR)
	eval "$(QEMU_RUN)"


# Build rules
$(TARGET_DIR)/%.o: %.c | $(TARGET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET_DIR)/%.o: %.S | $(TARGET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET_DIR)/%.o: %.s | $(TARGET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Kconfig tools from pip install kconfiglib
menuconfig:
	menuconfig Kconfig

distclean: clean
	rm -rf $(output_dir)
	rm -f *.elf *.o
	rm -f .config .config.old
clean:
	rm -f *.elf *.o
	rm -rf $(TARGET_DIR)
run:
	eval "$(QEMU_RUN)"
debug:
	./scripts/auto_gdbinit.sh $(PROJECT_ROOT) $(TARGET_DIR) $(CROSS_COMPILE) $(TARGET) $(ARCH) $(ARCH_SIZE) $(BOARD_DIR)
	$(GDB) -x $(PROJECT_ROOT)/scripts/gdbinit

# Include generated dependency files so headers trigger rebuilds
-include $(DEPS)

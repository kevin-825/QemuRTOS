# QemuRTOS / Top-Level Makefile

# Kconfig tools from pip install kconfiglib

.PHONY: all menuconfig clean distclean qemu-run qemu-dbg debug


NON_BUILD_TARGETS := menuconfig save defconfig %_defconfig save_%

#Gloval variables
output_dir := /mnt/wsl/ramdisk5/qemurtos
PROJECT_ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
QEMU_RUN_CMD_PREFIX :=

# ==============================================================================
# PHASE 1: PARSE TIME GUARDRAILS & CONFIGURATION
# If the user's command is NOT in the non-build list, we prepare to build!
# ==============================================================================

ifeq ($(filter $(NON_BUILD_TARGETS),$(MAKECMDGOALS)),)    
    ifeq ($(wildcard .config),)
        $(error FATAL: .config not found. Please run 'make menuconfig' first.)
    endif    
    include .config    
    CROSS_COMPILE ?= $(patsubst "%",%,$(CONFIG_CROSS_COMPILE))
    ARCH          ?= $(patsubst "%",%,$(CONFIG_ARCH_NAME))
    CHIP_DIR      ?= $(patsubst "%",%,$(CONFIG_CHIP_DIR))
    BOARD_DIR     ?= $(patsubst "%",%,$(CONFIG_BOARD_DIR))
    ARCH_SIZE     ?= $(if $(CONFIG_64BIT),64,32)
    TARGET_DIR := $(output_dir)/$(ARCH)
    TARGET     := $(TARGET_DIR)/qemurtos.elf    
    CC      = $(CROSS_COMPILE)gcc
    AS      = $(CROSS_COMPILE)gcc
    LD      = $(CROSS_COMPILE)ld
    AR      = $(CROSS_COMPILE)ar
    SIZE    = $(CROSS_COMPILE)size
    OBJDUMP = $(CROSS_COMPILE)objdump
    GDB     = $(CROSS_COMPILE)gdb
    STRIP   = $(CROSS_COMPILE)strip
    OBJCPY  = $(CROSS_COMPILE)objcopy    
    CFLAGS = -Wall -Wextra -O0 -g3 \
             -ffreestanding -nostdlib -nostartfiles \
             -Iinclude     
    LDFLAGS = -nostdlib -nostartfiles    
    # 5. STRICT Modular Flag Injection
    SRCS := 
    
    include arch/$(ARCH)/arch.mk
    include chip/$(CHIP_DIR)/chip.mk
    include boards/$(BOARD_DIR)/board.mk
    include kernel/kernel.mk
    include init/init.mk
    include drivers/drivers.mk
    include subsys/subsys.mk
    include lib/lib.mk
    include include/include.mk    
    OBJS := $(SRCS:.c=.o)
    OBJS := $(OBJS:.S=.o)
    OBJS := $(OBJS:.s=.o)
    OBJS := $(addprefix $(TARGET_DIR)/, $(OBJS))
    
    DEPS := $(OBJS:.o=.d)
    CFLAGS += -MMD -MP
    $(info "========================================")
    $(info " QemuRTOS Build Configuration:")
    $(info " Board        : $(BOARD_DIR)")
    $(info " Chip         : $(CHIP_DIR)")
    $(info " Architecture : $(ARCH)")
    $(info " Arch Size    : $(ARCH_SIZE) bit")
    $(info " Compiler     : $(CC)")
    $(info "========================================")
    # Include generated dependency files so headers trigger rebuilds
    -include $(DEPS)

endif
# ==============================================================================
# PHASE 2: TARGET EXECUTION 
# ==============================================================================


all: $(TARGET)

# Safer directory creation rule
$(TARGET_DIR): 
	@mkdir -p $(@D)

# This will link all the compiled .o files together
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	@echo " [Building QemuRTOS] src files:"
	@echo "$(SRCS)" | tr ' ' '\n' | sort | sed 's/ /\n/g' 
	@echo "\nCFLAGS: $(CFLAGS)\n "
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "======================================="
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET_DIR)/qemurtos.elf
	$(SIZE) $(TARGET_DIR)/qemurtos.elf
	@echo "======================================="
	@echo " [SUCCESS] Build complete! Output at $(TARGET_DIR)/qemurtos.elf"
	@sh -c '$(OBJDUMP) -d -S $(TARGET_DIR)/qemurtos.elf > $(TARGET_DIR)/qemurtos.elf.s'
	@echo "======================================="
	./scripts/auto_gdbinit.sh $(PROJECT_ROOT) $(TARGET_DIR) $(CROSS_COMPILE) $(TARGET) $(ARCH) $(ARCH_SIZE) $(BOARD_DIR)
	eval "$(QEMU_RUN_CMD_PREFIX) --kernel $(TARGET) -s"


# Build rules
$(TARGET_DIR)/%.o: %.c | $(TARGET_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET_DIR)/%.o: %.S | $(TARGET_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET_DIR)/%.o: %.s | $(TARGET_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

distclean:
	rm -rf $(output_dir)
	rm -f *.elf *.o
	rm -f .config .config.old
clean:
	rm -f *.elf *.o
	rm -rf $(TARGET_DIR)

qemu-run:
	eval "$(QEMU_RUN_CMD_PREFIX) --kernel $(TARGET) -s"
qemu-dbg:
	eval "$(QEMU_RUN_CMD_PREFIX) --kernel $(TARGET) -s -S"

debug:
	./scripts/auto_gdbinit.sh $(PROJECT_ROOT) $(TARGET_DIR) $(CROSS_COMPILE) $(TARGET) $(ARCH) $(ARCH_SIZE) $(BOARD_DIR)
	$(GDB) -x $(PROJECT_ROOT)/scripts/gdbinit

menuconfig:
	menuconfig Kconfig

# 1. Save the current .config into a minimal 'defconfig' file
save:
	@savedefconfig --kconfig Kconfig --out defconfig
	@echo " [SUCCESS] Saved minimal configuration to 'defconfig'"

# Alternate shortcut: make save_<name>
save_%:
	@mkdir -p configs
	@savedefconfig --kconfig Kconfig --out configs/$*_defconfig
	@echo " [SUCCESS] Saved configuration to 'configs/$*_defconfig'"

defconfig:
	@defconfig --kconfig Kconfig
	@echo " [SUCCESS] Loaded default configuration"

# 3. Load a specific named defconfig (e.g., make my_board_defconfig)
# This rule will search the root directory, and optionally a 'configs/' folder.
%_defconfig:
	@if [ -f "$@" ]; then \
		defconfig --kconfig Kconfig "$@"; \
	elif [ -f "configs/$@" ]; then \
		defconfig --kconfig Kconfig "configs/$@"; \
	else \
		echo " [!] ERROR: Could not find configuration file '$@'"; \
		exit 1; \
	fi
	@echo " [SUCCESS] Loaded configuration from $@"

print-%:
	@echo "$* = $($*)" | tr ' ' '\n' 
pr-%:
	@echo "$* = $($*)" | tr ' ' '\n' 


LDFLAGS += -T chip/$(CHIP_DIR)/linker.ld


obj-y += chip/$(CHIP_DIR)/startup.S


# CFLAGS += -Ichip/$(CHIP_DIR)/include

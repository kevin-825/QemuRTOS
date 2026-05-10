# drivers/serial/serial.mk

drv_path := $(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(CONFIG_SERIAL_NS16550),y)
    SRCS += $(drv_path)ns16550.c
endif

ifeq ($(CONFIG_SERIAL_PL011),y)
    SRCS += $(drv_path)pl011.c
endif

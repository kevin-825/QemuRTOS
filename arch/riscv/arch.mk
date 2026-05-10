d := $(dir $(lastword $(MAKEFILE_LIST)))

# 1. RISC-V Specific Compiler Flags
ifeq ($(ARCH_SIZE),32)
    CFLAGS += -march=rv32imafdc -mabi=ilp32d
    LDFLAGS += -march=rv32imafdc -mabi=ilp32d
else
    CFLAGS += -march=rv64imafdc -mabi=lp64d
    LDFLAGS += -march=rv64imafdc -mabi=lp64d
endif
CFLAGS += -mcmodel=medany -Iarch/$(ARCH)/include -Iarch/$(ARCH)/core

SRCS += $(wildcard $(d)core/*.c)
SRCS += $(wildcard $(d)core/*.S)
SRCS += $(wildcard $(d)core/*.s)

SRCS += $(wildcard $(d)*.c)
SRCS += $(wildcard $(d)*.S)
SRCS += $(wildcard $(d)*.s)

#sources := device.c
#SRCS += $(addprefix $(d), $(sources))

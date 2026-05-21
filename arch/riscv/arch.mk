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


mySrc :=
mySrc += $(wildcard $(d)core/*.c)
mySrc += $(wildcard $(d)core/*.S)
mySrc += $(wildcard $(d)core/*.s)
myobj :=
myobj :=$(mySrc:.c=.o)
myobj :=$(myobj:.S=.o)
myobj :=$(myobj:.s=.o)

obj-y += $(myobj)

#sources := device.c
#SRCS += $(addprefix $(d), $(sources))

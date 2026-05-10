

d := $(dir $(lastword $(MAKEFILE_LIST)))

SRCS += $(wildcard $(d)core/*.c)
SRCS += $(wildcard $(d)core/*.S)
SRCS += $(wildcard $(d)core/*.s)

SRCS += $(wildcard $(d)*.c)
SRCS += $(wildcard $(d)*.S)
SRCS += $(wildcard $(d)*.s)

#sources := device.c
#SRCS += $(addprefix $(d), $(sources))


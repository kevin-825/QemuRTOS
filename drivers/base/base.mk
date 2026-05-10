d := $(dir $(lastword $(MAKEFILE_LIST)))

SRCS += $(wildcard $(d)*.c)

#sources := device.c
#SRCS += $(addprefix $(d), $(sources))

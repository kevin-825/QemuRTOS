init_dir := $(dir $(lastword $(MAKEFILE_LIST)))

SRCS += $(wildcard $(init_dir)*.c)

#sources := device.c
#SRCS += $(addprefix $(init_dir), $(sources))
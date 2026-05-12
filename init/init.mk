init_dir := $(dir $(lastword $(MAKEFILE_LIST)))

SRCS += $(wildcard $(init_dir)*.c)

CFLAGS += -Iinit
#sources := device.c
#SRCS += $(addprefix $(init_dir), $(sources))
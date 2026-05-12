mm_dir := $(dir $(lastword $(MAKEFILE_LIST)))

SRCS += $(wildcard $(mm_dir)*.c)

#sources := device.c
#SRCS += $(addprefix $(mm_dir), $(sources))
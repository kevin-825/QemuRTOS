

d := $(dir $(lastword $(MAKEFILE_LIST)))

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

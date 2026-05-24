
CFLAGS += -Ilib/mylib

mylibdir := $(dir $(lastword $(MAKEFILE_LIST)))

mySrc :=
mySrc += $(wildcard $(mylibdir)list/*.c)
mySrc += $(wildcard $(mylibdir)string/*.c)
mySrc += $(wildcard $(mylibdir)tree/*.c)
myobj :=
myobj :=$(mySrc:.c=.o)
myobj :=$(myobj:.S=.o)
myobj :=$(myobj:.s=.o)

obj-y += $(myobj)


CFLAGS += -Ilib/mylib

mylibdir := $(dir $(lastword $(MAKEFILE_LIST)))

SRCS += $(wildcard $(mylibdir)list/*.c)
SRCS += $(wildcard $(mylibdir)string/*.c)
SRCS += $(wildcard $(mylibdir)tree/*.c)
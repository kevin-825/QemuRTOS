
# Only compile ns16550.o if CONFIG_SERIAL_NS16550 is 'y'
obj-$(CONFIG_SERIAL_NS16550) += drivers/serial/ns16550.o

# Only compile pl011.o if CONFIG_SERIAL_PL011 is 'y'
obj-$(CONFIG_SERIAL_PL011) += drivers/serial/pl011.o

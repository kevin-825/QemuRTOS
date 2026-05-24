/* drivers/serial/pl011.c */
#include "kernel/device.h"
#include "drivers/uart.h"
#include "kernel/mm/kmalloc.h" /* Your kmalloc/kfree header */
#include <libfdt.h>

struct pl011_data {
    uintptr_t base_addr;
    uint32_t irq_type;
    uint32_t irq_num;
    uint32_t irq_flags;
};

#define UART_DR    0x000 
#define UART_FR    0x018 
#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)

static void pl011_poll_out(struct platform_device *dev, unsigned char c) {
    struct pl011_data *data = (struct pl011_data *)dev->private_data;
    volatile uint32_t *base = (uint32_t *)data->base_addr;
    
    if (c == '\n') pl011_poll_out(dev, '\r');
    while (base[UART_FR / 4] & UART_FR_TXFF);
    base[UART_DR / 4] = c;
}

static void pl011_put_char(struct platform_device *dev, unsigned char c) {
    pl011_poll_out(dev, c);
}

static int pl011_probe(struct platform_device *dev) {
    /* --- DYNAMIC RAM ALLOCATION --- */
    struct pl011_data *data = kcalloc(1, sizeof(struct pl011_data));
    if (!data) {
        return -1; /* Heap allocation failed */
    }
    
    int len;
    const fdt32_t *prop;
    extern void *g_dtb_ptr; 

    /* Extract Base Address */
    prop = fdt_getprop(g_dtb_ptr, dev->dt_node_offset, "reg", &len);
    if (!prop) {
        kfree(data); /* Clean up on failure! */
        return -1;
    }
    
    uint64_t addr = ((uint64_t)fdt32_to_cpu(prop[0]) << 32) | fdt32_to_cpu(prop[1]);
    data->base_addr = (uintptr_t)addr;

    /* Extract IRQs */
    prop = fdt_getprop(g_dtb_ptr, dev->dt_node_offset, "interrupts", &len);
    if (prop && len >= (int)(3 * sizeof(fdt32_t))) {
        data->irq_type  = fdt32_to_cpu(prop[0]);
        data->irq_num   = fdt32_to_cpu(prop[1]);
        data->irq_flags = fdt32_to_cpu(prop[2]);
    }

    /* Bind the dynamically allocated memory back to the device */
    dev->private_data = data;
    
    return 0; /* Success! */
}

/* Optional: Clean up memory if the device is hot-unplugged or errors out */
static void pl011_remove(struct platform_device *dev) {
    if (dev->private_data) {
        kfree(dev->private_data);
        dev->private_data = NULL;
    }
}

static const struct uart_driver_api pl011_api = {
    .poll_out = pl011_poll_out,
    .put_char = pl011_put_char,
};

/* Tell the Linker to pack this driver into the .driver_list array */
DRIVER_DEFINE(pl011, "arm,pl011", pl011_probe, &pl011_api, POST_KERNEL);
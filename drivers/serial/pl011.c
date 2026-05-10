/* drivers/serial/pl011.c */
#include "device.h"
#include "uart.h"
#include <libfdt.h>

struct pl011_data {
    uintptr_t base_addr;
    /* ARM GIC Interrupt parameters */
    uint32_t irq_type;   /* 0 = SPI, 1 = PPI */
    uint32_t irq_num;    /* The raw interrupt number */
    uint32_t irq_flags;  /* Level/Edge trigger flags */
};

/* ARM PL011 Register Offsets */
#define UART_DR    0x000 /* Data Register */
#define UART_FR    0x018 /* Flag Register */
#define UART_FR_TXFF (1 << 5) /* Transmit FIFO Full */
#define UART_FR_RXFE (1 << 4) /* Receive FIFO Empty */

static void pl011_poll_out(const struct device *dev, unsigned char c) {
    const struct pl011_data *data = dev->data;
    volatile uint32_t *base = (uint32_t *)data->base_addr;
    
    if (c == '\n') {
        pl011_poll_out(dev, '\r');
    }
    
    while (base[UART_FR / 4] & UART_FR_TXFF);
    base[UART_DR / 4] = c;
}
static void pl011_put_char(const struct device *dev, unsigned char c) {
    pl011_poll_out(dev, (unsigned char)c);
}

static int pl011_poll_in(const struct device *dev, unsigned char *c) {
    const struct pl011_data *data = dev->data;
    volatile uint32_t *base = (uint32_t *)data->base_addr;
    
    if ((base[UART_FR / 4] & UART_FR_RXFE) == 0) {
        *c = base[UART_DR / 4] & 0xFF;
        return 0;
    }
    return -1;
}
static char pl011_get_char(const struct device *dev) {
    unsigned char c;
    while (pl011_poll_in(dev, &c) != 0) {
        /* Wait for data */
    }
    return (char)c;
}

int pl011_init(const struct device *dev, void *dtb, int dt_node) {
    struct pl011_data *data = (struct pl011_data *)dev->data;
    int len;
    const fdt32_t *prop;

    /* --- 1. EXTRACT THE BASE ADDRESS --- */
    prop = fdt_getprop(dtb, dt_node, "reg", &len);
    if (!prop) {
        return -1; 
    }
    
    uint64_t addr_high = fdt32_to_cpu(prop[0]);
    uint64_t addr_low  = fdt32_to_cpu(prop[1]);
    data->base_addr = (uintptr_t)((addr_high << 32) | addr_low);

    /* --- 2. EXTRACT ALL 3 IRQ CELLS --- */
    prop = fdt_getprop(dtb, dt_node, "interrupts", &len);
    
    /* Ensure the property actually contains at least 3 cells (12 bytes) */
    if (prop && len >= (int)(3 * sizeof(fdt32_t))) {
        data->irq_type  = fdt32_to_cpu(prop[0]);
        data->irq_num   = fdt32_to_cpu(prop[1]);
        data->irq_flags = fdt32_to_cpu(prop[2]);
    } else {
        data->irq_type  = 0;
        data->irq_num   = 0;
        data->irq_flags = 0;
    }

    return 0; 
}

/* API Structure */
static const struct uart_driver_api pl011_api = {
    .poll_out = pl011_poll_out,
    .poll_in  = pl011_poll_in,
    .put_char = pl011_put_char,
    .get_char = pl011_get_char
};

/* Initialize the data struct with 0s */
static struct pl011_data uart0_data = { 
    .base_addr = 0, 
    .irq_type = 0, 
    .irq_num = 0, 
    .irq_flags = 0 
};
static struct device_state uart0_state = { .init_state = DEVICE_STATE_UNINIT };

/* THE ZEPHYR MAGIC */
DEVICE_DEFINE(uart0_pl011, "arm,pl011", pl011_init, NULL, &uart0_data, &uart0_state, &pl011_api, POST_KERNEL);

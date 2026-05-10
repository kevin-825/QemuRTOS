/* drivers/serial/ns16550.c */
#include "device.h"
#include "drivers/uart.h"
#include <libfdt.h>

/* 1. ROM Configuration */
struct ns16550_config {
    uint32_t placeholder; /* Placeholder for any future config parameters */
};
/* The private data for this specific UART instance */
struct ns16550_data {
    uintptr_t base_addr;
    uint32_t irq;
};



/* 3. The Implementation */
static void ns16550_poll_out(const struct device *dev, unsigned char c) {
    const struct ns16550_data *data = dev->data;
    volatile uint8_t *base = (uint8_t *)data->base_addr;
    
    if (c == '\n') {
        ns16550_poll_out(dev, '\r'); /* Terminal newline fix */
    }
    
    while (!(base[5] & 0x20)); /* Wait for TX ready */
    base[0] = c;
}

static int ns16550_poll_in(const struct device *dev, unsigned char *c) {
    const struct ns16550_data *data = dev->data;
    volatile uint8_t *base = (uint8_t *)data->base_addr;
    
    if (base[5] & 0x01) { /* Data ready? */
        *c = base[0];
        return 0;
    }
    return -1;
}

int ns16550_init(const struct device *dev, void *dtb, int dt_node) {
    /* Cast dev->data to our local struct type */
    struct ns16550_data *data = (struct ns16550_data *)dev->data;
    
    int len;
    const fdt32_t *prop;

    /* --- 1. EXTRACT THE BASE ADDRESS --- */
    /* "reg" property usually contains [address, size] */
    prop = fdt_getprop(dtb, dt_node, "reg", &len);
    if (!prop) {
        return -1; /* Failed: No memory address found! */
    }
    
    /* CRITICAL: Device tree values are ALWAYS Big-Endian. 
       We must convert them to RISC-V Little-Endian. */
    //data->base_addr = (uintptr_t)fdt32_to_cpu(prop[0]);
    /* Extract 64-bit address from the two 32-bit cells */
    uint64_t addr = ((uint64_t)fdt32_to_cpu(prop[0]) << 32) | fdt32_to_cpu(prop[1]);
    data->base_addr = (uintptr_t)addr;
    
    /* (Note: prop[1] would contain the size of the memory region, e.g., 0x100) */


    /* --- 2. EXTRACT THE IRQ --- */
    prop = fdt_getprop(dtb, dt_node, "interrupts", &len);
    if (!prop) {
        return -1; /* Failed: No IRQ found! */
    }
    uint32_t interrupt_type = fdt32_to_cpu(prop[0]);
    uint32_t interrupt_num = fdt32_to_cpu(prop[1]);
    uint32_t interrupt_flags = fdt32_to_cpu(prop[2]);
    data->irq = fdt32_to_cpu(prop[0]);


    /* --- 3. ACTUALLY INITIALIZE THE HARDWARE --- */
    /* Now that we know where it is, we can configure the registers */
    uint8_t *uart_base = (uint8_t *)data->base_addr;
    
    /* Disable all UART interrupts for now */
    uart_base[1] = 0x00; 
    
    /* Set word length to 8 bits, no parity, 1 stop bit (8N1) */
    uart_base[3] = 0x03;
    
    /* Enable FIFO, clear them */
    uart_base[2] = 0x07; 

    return 0; /* Success! */
}

/* The API implementation for writing a character */
static void ns16550_put_char(const struct device *dev, char c) {
    /* 1. Retrieve the saved data from the device struct */
    struct ns16550_data *data = (struct ns16550_data *)dev->data;
    
    /* 2. Cast the saved base_addr to a volatile pointer */
    volatile uint8_t *uart = (volatile uint8_t *)data->base_addr;
    
    /* 3. Hardware interaction (Wait for TX buffer to be empty, then write) */
    while ((uart[5] & 0x20) == 0) {
        /* Wait */
    }
    uart[0] = c;
}

/* The API implementation for reading a character */
static char ns16550_get_char(const struct device *dev) {
    /* 1. Retrieve the saved data again */
    struct ns16550_data *data = (struct ns16550_data *)dev->data;
    volatile uint8_t *uart = (volatile uint8_t *)data->base_addr;

    /* 2. Wait for data to be ready, then read */
    while ((uart[5] & 0x01) == 0) {
        /* Wait */
    }
    return uart[0];
}

/* 4. Link it all together */
static const struct uart_driver_api ns16550_api = {
    .poll_out = ns16550_poll_out,
    .poll_in  = ns16550_poll_in,
    .put_char = ns16550_put_char,
    .get_char = ns16550_get_char
};

static const struct ns16550_config uart0_cfg;

static struct ns16550_data uart0_data= {
    .base_addr = 0, /* Will be set during init from device tree */
    .irq = 0       /* Will be set during init from device tree */
};
static struct device_state uart0_state = { .init_state = DEVICE_STATE_UNINIT };


DEVICE_DEFINE(uart0, "ns16550a", ns16550_init, &uart0_cfg, &uart0_data, &uart0_state, &ns16550_api, POST_KERNEL);

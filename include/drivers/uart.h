/* include/drivers/uart.h */
#ifndef QEMURTOS_UART_H
#define QEMURTOS_UART_H

#include "kernel/device.h"

/* 1. The Condensed API Struct */
struct uart_driver_api {
    /* Polling API */
    void (*poll_out)(struct platform_device *dev, unsigned char out_char);
    int  (*poll_in)(struct platform_device *dev, unsigned char *p_char);
    void (*put_char)(struct platform_device *dev, unsigned char c);
    char (*get_char)(struct platform_device *dev);

    /* We can add Interrupt API here later when we write the RISC-V Trap handler! */
};

/* 2. The Zephyr-Style Inline Wrappers */
/* 2. The Zephyr-Style Inline Wrappers */
static inline void uart_poll_out(struct platform_device *dev, unsigned char out_char) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->driver->api;
    api->poll_out(dev, out_char);
}

static inline int uart_poll_in(struct platform_device *dev, unsigned char *p_char) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->driver->api;
    return api->poll_in(dev, p_char);
}

static inline void uart_put_char(struct platform_device *dev, char c) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->driver->api;
    api->put_char(dev, c);
}

static inline char uart_get_char(struct platform_device *dev) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->driver->api;
    return api->get_char(dev);
}

#endif /* QEMURTOS_UART_H */

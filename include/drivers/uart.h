/* include/drivers/uart.h */
#ifndef QEMURTOS_UART_H
#define QEMURTOS_UART_H

#include "kernel/device.h"

/* 1. The Condensed API Struct */
struct uart_driver_api {
    /* Polling API */
    void (*poll_out)(const struct device *dev, unsigned char out_char);
    int  (*poll_in)(const struct device *dev, unsigned char *p_char);
    void (*put_char)(const struct device *dev, unsigned char c);
    char (*get_char)(const struct device *dev);

    /* We can add Interrupt API here later when we write the RISC-V Trap handler! */
};

/* 2. The Zephyr-Style Inline Wrappers */
/* 2. The Zephyr-Style Inline Wrappers */
static inline void uart_poll_out(const struct device *dev, unsigned char out_char) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->api;
    api->poll_out(dev, out_char);
}

static inline int uart_poll_in(const struct device *dev, unsigned char *p_char) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->api;
    return api->poll_in(dev, p_char);
}

static inline void uart_put_char(const struct device *dev, char c) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->api;
    api->put_char(dev, c);
}

static inline char uart_get_char(const struct device *dev) {
    const struct uart_driver_api *api = (const struct uart_driver_api *)dev->api;
    return api->get_char(dev);
}

#endif

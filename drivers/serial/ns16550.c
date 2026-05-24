/* drivers/serial/ns16550.c
 *
 * Platform-bus style NS16550 driver that matches your updated platform model.
 * - Uses byte-sized volatile MMIO accesses (compatible with legacy UARTs)
 * - Parses 32/64-bit 'reg' encodings from the DTB using dev->dt_node_offset
 * - Allocates per-device private_data in probe() and frees it in remove()
 * - Sets device state via dev->state.init_state
 *
 * Expects these headers to exist in your tree (as in your examples):
 *   kernel/device.h
 *   drivers/uart.h
 *   kernel/mm/kmalloc.h
 *   libfdt.h
 *
 * This driver is written to plug into your platform bus:
 *   - platform_bus_enumerate() creates platform_device entries with dt_node_offset
 *   - platform_bus_match_drivers() finds this driver by compatible string and calls probe()
 *
 * Notes:
 *  - This driver intentionally does NOT ioremap; it dereferences the physical base
 *    directly (as your working init did). If your environment requires virtual mapping,
 *    change the access helpers to use the mapped base.
 *  - The probe performs a safe read test before writes; if that read traps, the platform
 *    environment must ensure the device region is accessible at probe time.
 */

#include "kernel/device.h"
#include "drivers/uart.h"
#include "kernel/mm/kmalloc.h"
#include <libfdt.h>
#include <stdint.h>

/* Per-device runtime data */
struct ns16550_data {
    uintptr_t base_addr;   /* physical base (as your environment uses) */
    uintptr_t vbase;       /* reserved if you later add ioremap */
    uint32_t irq;
};

/* 16550 byte offsets */
#define NS16550_RBR_THR_DLL   0x00
#define NS16550_IER_DLM       0x01
#define NS16550_IIR_FCR       0x02
#define NS16550_LCR           0x03
#define NS16550_MCR           0x04
#define NS16550_LSR           0x05
#define NS16550_MSR           0x06
#define NS16550_SCR           0x07

/* LSR bits */
#define NS16550_LSR_DR   (1 << 0)
#define NS16550_LSR_THRE (1 << 5)

/* Helper: byte pointer into MMIO region (no ioremap) */
static inline volatile uint8_t *ns16550_bptr(uintptr_t base, uint32_t off)
{
    return (volatile uint8_t *)(base + off);
}

/* UART API functions (match your uart_driver_api) */
static void ns16550_poll_out(struct platform_device *pdev, unsigned char c)
{
    struct ns16550_data *data = (struct ns16550_data *)pdev->private_data;
    volatile uint8_t *base = ns16550_bptr(data->base_addr, 0);

    if (c == '\n') {
        ns16550_poll_out(pdev, '\r');
    }

    while (!(base[NS16550_LSR] & NS16550_LSR_THRE)) {
        /* spin */
    }
    base[NS16550_RBR_THR_DLL] = (uint8_t)c;
}

static int ns16550_poll_in(struct platform_device *pdev, unsigned char *c)
{
    struct ns16550_data *data = (struct ns16550_data *)pdev->private_data;
    volatile uint8_t *base = ns16550_bptr(data->base_addr, 0);

    if (base[NS16550_LSR] & NS16550_LSR_DR) {
        *c = base[NS16550_RBR_THR_DLL];
        return 0;
    }
    return -1;
}

static void ns16550_put_char(struct platform_device *pdev, unsigned char c)
{
    struct ns16550_data *data = (struct ns16550_data *)pdev->private_data;
    volatile uint8_t *base = ns16550_bptr(data->base_addr, 0);

    while ((base[NS16550_LSR] & NS16550_LSR_THRE) == 0) {
        /* wait */
    }
    base[NS16550_RBR_THR_DLL] = (uint8_t)c;
}

static char ns16550_get_char(struct platform_device *pdev)
{
    struct ns16550_data *data = (struct ns16550_data *)pdev->private_data;
    volatile uint8_t *base = ns16550_bptr(data->base_addr, 0);

    while ((base[NS16550_LSR] & NS16550_LSR_DR) == 0) {
        /* wait */
    }
    return (char)base[NS16550_RBR_THR_DLL];
}

/* Probe: called by platform_bus_match_drivers() */
static int ns16550_probe(struct platform_device *pdev)
{
    int len;
    const fdt32_t *prop;
    extern void *g_dtb_ptr; /* your global DTB pointer used elsewhere */

    /* Allocate private data */
    struct ns16550_data *data = kcalloc(1, sizeof(*data));
    if (!data) {
        return -1;
    }

    /* Parse 'reg' property from DTB using the node offset stored in pdev */
    prop = fdt_getprop(g_dtb_ptr, pdev->dt_node_offset, "reg", &len);
    if (!prop || len < (int)sizeof(fdt32_t)) {
        kfree(data);
        return -1;
    }

    /* Support both 1-cell (32-bit) and 2-cell (64-bit) encodings */
    if (len >= (int)(2 * sizeof(fdt32_t))) {
        uint64_t hi = (uint64_t)fdt32_to_cpu(prop[0]);
        uint64_t lo = (uint64_t)fdt32_to_cpu(prop[1]);
        uint64_t addr = (hi << 32) | lo;
        data->base_addr = (uintptr_t)addr;
    } else {
        uint32_t addr32 = fdt32_to_cpu(prop[0]);
        data->base_addr = (uintptr_t)addr32;
    }

    /* Parse 'interrupts' if present (optional) */
    prop = fdt_getprop(g_dtb_ptr, pdev->dt_node_offset, "interrupts", &len);
    if (prop && len >= (int)sizeof(fdt32_t)) {
        data->irq = fdt32_to_cpu(prop[0]);
    } else {
        data->irq = 0;
    }

    /* Safe read test: perform a non-destructive read to detect inaccessible regions early.
     * If this read traps, the platform must ensure the region is accessible at probe time.
     */
    volatile uint8_t *base = ns16550_bptr(data->base_addr, 0);
    volatile uint8_t probe_val = base[NS16550_RBR_THR_DLL];
    (void)probe_val;

    /* Minimal UART init: disable interrupts, 8N1, enable FIFO (typical) */
    base[NS16550_IER_DLM] = 0x00; /* disable interrupts */
    base[NS16550_LCR]     = 0x03; /* 8 bits, no parity, 1 stop */
    base[NS16550_IIR_FCR] = 0x07; /* enable FIFO, clear RX/TX FIFOs */

    /* Bind private data to platform_device */
    pdev->private_data = data;
    pdev->state.init_state = DEVICE_STATE_READY;

    return 0;
}

/* Remove: free private data */
static void ns16550_remove(struct platform_device *pdev)
{
    if (!pdev) return;
    if (pdev->private_data) {
        kfree(pdev->private_data);
        pdev->private_data = NULL;
    }
    pdev->state.init_state = DEVICE_STATE_UNINIT;
}

/* Expose a small uart API wrapper that the rest of the kernel can use.
 * Your platform's uart subsystem may expect a different prototype; adapt if needed.
 */
static const struct uart_driver_api ns16550_api = {
    .poll_out = (void (*)(struct platform_device *, unsigned char))ns16550_poll_out,
    .poll_in  = (int  (*)(struct platform_device *, unsigned char *))ns16550_poll_in,
    .put_char = (void (*)(struct platform_device *, unsigned char))ns16550_put_char,
    .get_char = (char (*)(struct platform_device *))ns16550_get_char,
};

/* Register the driver in the linker array so platform_bus_match_drivers() can find it.
 * Use the same compatible string you put in the DTB: "ns16550a"
 */
DRIVER_DEFINE(ns16550, "ns16550a", ns16550_probe, &ns16550_api, POST_KERNEL);

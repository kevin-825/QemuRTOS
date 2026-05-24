//device.c - Core device management for QemuRTOS

/* kernel/device.c */
#include "kernel/device.h"
#include "kernel/mm/kmalloc.h"  /* Your kmalloc/kfree header */
#include "kernel/printk.h"
#include <libfdt.h>
#include "string.h"

extern struct device_driver _driver_list_start[];
extern struct device_driver _driver_list_end[];

/* Global linked list head for the Platform Bus */
static struct platform_device *platform_bus = NULL;

void platform_bus_enumerate(void *dtb) {
    int node_offset = 0;
    int depth = 0;
    
    for (node_offset = fdt_next_node(dtb, -1, &depth);
         node_offset >= 0;
         node_offset = fdt_next_node(dtb, node_offset, &depth)) {
        
        const char *compat = fdt_getprop(dtb, node_offset, "compatible", NULL);
        if (!compat) continue;

        const char *status = fdt_getprop(dtb, node_offset, "status", NULL);
        if (status && strcmp(status, "disabled") == 0) continue; 

        /* --- THE HEAP ALLOCATION --- */
        /* kcalloc is perfect here because it ensures dev->driver, dev->next, 
         * and dev->private_data are safely initialized to NULL */
        struct platform_device *dev = kcalloc(1, sizeof(struct platform_device));
        if (!dev) {
            /* Out of memory condition, stop enumerating */
            break; 
        }

        const char *node_name = fdt_get_name(dtb, node_offset, NULL);
        strncpy(dev->name, node_name, sizeof(dev->name) - 1);
        dev->name[sizeof(dev->name) - 1] = '\0';
        
        dev->compatible = compat;
        dev->dt_node_offset = node_offset;
        dev->state.init_state = DEVICE_STATE_UNINIT;

        /* Add to the head of the global linked list */
        dev->next = platform_bus;
        platform_bus = dev;
    }
}

int platform_bus_match_drivers(void *dtb) {
    struct platform_device *dev;
    struct device_driver *drv;
    int matched_count = 0;

    for (dev = platform_bus; dev != NULL; dev = dev->next) {
        if (dev->state.init_state != DEVICE_STATE_UNINIT) continue;

        for (drv = _driver_list_start; drv < _driver_list_end; drv++) {
            printk("Matching device '%s' against driver '%s'...\n", dev->name, drv->name);
            if (fdt_node_check_compatible(dtb, dev->dt_node_offset, drv->compatible) == 0) {
                
                dev->driver = drv;
                dev->state.init_state = DEVICE_STATE_INITIALIZING;
                
                if (drv->probe && drv->probe(dev) == 0) {
                    dev->state.init_state = DEVICE_STATE_READY;
                    matched_count++;
                } else {
                    dev->state.init_state = DEVICE_STATE_ERROR;
                    /* If probe fails, it's the driver's responsibility to free 
                     * dev->private_data, but the bus keeps the dev struct around 
                     * to flag the hardware failure. */
                }
                break; 
            }
        }
    }
    return matched_count;
}

struct platform_device* platform_bus_get_device(const char *name) {
    struct platform_device *dev;
    for (dev = platform_bus; dev != NULL; dev = dev->next) {
        if (strcmp(dev->name, name) == 0) {
            return dev;
        }
    }
    return NULL;
}

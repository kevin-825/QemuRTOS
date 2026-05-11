//device.c - Core device management for QemuRTOS

#include "kernel/device.h"
#include <string.h>
#include <stddef.h>
#include <libfdt.h>  /* Added for Auto-Probing */

/* Import symbols from linker.ld */
extern const struct device _device_PRE_KERNEL_start;
extern const struct device _device_PRE_KERNEL_end;
extern const struct device _device_POST_KERNEL_start;
extern const struct device _device_POST_KERNEL_end;
extern const struct device _device_init_start;
extern const struct device _device_init_end;

/* Updated to accept the DTB pointer */
static void init_tier(const struct device *start, const struct device *end, void *dtb_ptr) {
    for (const struct device *dev = start; dev < end; dev++) {
        if (dev->state->init_state != DEVICE_STATE_UNINIT) {
            continue;
        }

        int dt_node = -1;

        /* AUTO-PROBE LOGIC: If this driver is looking for specific hardware */
        if (dev->compatible) {
            if (!dtb_ptr) {
                dev->state->init_state = DEVICE_STATE_ERROR;
                continue; /* No DTB available to probe */
            }

            /* Search the DTB for the compatible string */
            dt_node = fdt_node_offset_by_compatible(dtb_ptr, -1, dev->compatible);

            if (dt_node < 0) {
                /* Hardware not found in the DTB. 
                 * Skip initialization entirely, leave state as UNINIT. */
                continue; 
            }
        }

        /* Hardware found (or it's a software-only virtual device without a compatible string) */
        dev->state->init_state = DEVICE_STATE_INITIALIZING;

        if (dev->init) {
            /* Pass the DTB pointer and the exact node offset to the driver */
            int err = dev->init(dev, dtb_ptr, dt_node);
            if (err == 0) {
                dev->state->init_state = DEVICE_STATE_READY;
            } else {
                dev->state->init_state = DEVICE_STATE_ERROR;
            }
        } else {
            /* No init function needed, just mark ready */
            dev->state->init_state = DEVICE_STATE_READY;
        }
    }
}

/* Boot sequence passes the DTB from start.S into here */
void device_init_all(void *dtb_ptr) {
    /* Optional sanity check: Ensure the DTB isn't corrupted */
    if (dtb_ptr && fdt_check_header(dtb_ptr) != 0) {
        dtb_ptr = NULL; /* Force fail on physical devices if DTB is bad */
    }

    init_tier(&_device_PRE_KERNEL_start, &_device_PRE_KERNEL_end, dtb_ptr);
    init_tier(&_device_POST_KERNEL_start, &_device_POST_KERNEL_end, dtb_ptr);
}

const struct device* device_get_binding(const char *name) {
    for (const struct device *dev = &_device_init_start; dev < &_device_init_end; dev++) {
        if (strcmp(dev->name, name) == 0) {
            if (dev->state->init_state == DEVICE_STATE_READY) {
                return dev;
            }
            return NULL;
        }
    }
    return NULL;
}

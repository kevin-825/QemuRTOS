#ifndef _KERNEL_DEVICE_H
#define _KERNEL_DEVICE_H

#include <stdint.h>
/* include/device.h */
/* Device Initialization States */
#define DEVICE_STATE_UNINIT       0x00  /* Has not been probed yet */
#define DEVICE_STATE_INITIALIZING 0x01  /* Currently booting */
#define DEVICE_STATE_READY        0x02  /* Booted and active */
#define DEVICE_STATE_SUSPENDED    0x03  /* Asleep (Power Management) */
#define DEVICE_STATE_ERROR        0xFF  /* Hardware failure */

/* Device Capability Flags */
#define DEVICE_FLAG_DYNAMIC       (1 << 0) /* Was this hot-plugged? */
#define DEVICE_FLAG_PM_SUPPORTED  (1 << 1) /* Can this device sleep? */
#define DEVICE_FLAG_SHARED_IRQ    (1 << 2) /* Does it share an interrupt? */

typedef int16_t device_handle_t;
struct device_state {
    uint8_t init_state; /* One of the DEVICE_STATE_* values */
    uint8_t flags;      /* Bitfield of DEVICE_FLAG_* values */
};

struct device {
    const char *name;
    const char *compatible;
    const void *config;
    const void *api;
    struct device_state *state;
    void *data;
    int (*init)(const struct device *dev, void *dtb_ptr, int dt_node);
    
    /* THE MISSING DEPENDENCY MEMBER */
    /* A pointer to a ROM array of integers representing dependent devices */
    const device_handle_t *handles; 
};


void device_init_all(void *dtb_ptr);
const struct device* device_get_binding(const char *name);

/* include/drivers/device.h */

/* Helper macro to handle the compiler attributes */
#define __device_init_section(_level) \
    __attribute__((__section__("._device_" #_level))) __attribute__((__used__))

/**
 * @brief Macro to define a device and add it to the linker section.
 * 
 * @param _dev_name   The instance name (e.g., uart0)
 * @param _compat     The Device Tree compatible string (e.g., "ns16550a")
 * @param _init_fn    The initialization function pointer
 * @param _config     Pointer to the ROM configuration struct
 * @param _data       Pointer to the RAM data struct
 * @param _state      Pointer to the state struct
 * @param _api        Pointer to the API struct
 * @param _level      The initialization tier (PRE_KERNEL, POST_KERNEL)
 */
#define DEVICE_DEFINE(_dev_name, _compat, _init_fn, _config, _data, _state, _api, _level) \
    const struct device _device_##_dev_name __device_init_section(_level) = { \
        .name = #_dev_name, \
        .compatible = _compat, \
        .config = (_config), \
        .api = (_api), \
        .state = (_state), \
        .data = (_data), \
        .init = (_init_fn), \
        .handles = NULL \
    }

#endif
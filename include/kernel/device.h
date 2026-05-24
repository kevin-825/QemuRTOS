#ifndef _KERNEL_DEVICE_H
#define _KERNEL_DEVICE_H

#include <stdint.h>
#include <stddef.h>

/* ==========================================================================
 * 1. STATE & CAPABILITY FLAGS
 * ========================================================================== */

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

/* Forward declarations to resolve circular pointer dependencies */
struct platform_device;
struct device_driver;

/* ==========================================================================
 * 2. THE DRIVER (SOFTWARE DEFINITION)
 * ========================================================================== */

struct device_driver {
    const char *name;
    const char *compatible; /* e.g., "arm,pl011" or "ns16550a" */
    
    /* The lifecycle functions called by the Platform Bus */
    int (*probe)(struct platform_device *dev); 
    void (*remove)(struct platform_device *dev);
    
    /* Function pointers for the generic subsystem API (UART, SPI, etc.) */
    const void *api; 
};

/* Helper macro to instruct the Linker Script where to pack the drivers */
#define __device_driver_init_section(_level) \
    __attribute__((__section__("._driver_list_" #_level))) __attribute__((__used__)) __attribute__((packed))

/* * Macro used at the bottom of driver .c files to statically compile 
 * the driver into the kernel's read-only linker array.
 */
#define DRIVER_DEFINE(_name, _compat, _probe, _api, _level) \
    const struct device_driver _driver_##_name __device_driver_init_section(_level) = { \
        .name = #_name, \
        .compatible = _compat, \
        .probe = _probe, \
        .remove = NULL, \
        .api = _api \
    }

/* ==========================================================================
 * 3. THE DEVICE (HARDWARE INSTANCE)
 * ========================================================================== */

struct platform_device {
    char name[32];           /* e.g., "serial@9000000" */
    const char *compatible;  /* Extracted from DTB to match with drivers */
    
    /* CRITICAL: Where this device lives in the flattened device tree blob.
     * The probe() function uses this to extract reg and interrupt data. */
    int dt_node_offset;      
    
    const void *config;      /* Optional ROM configuration */
    void *private_data;      /* RAM allocated by the driver during probe() */
    
    struct device_state state;    /* Current runtime state */
    
    /* The software logic bound to this hardware instance */
    const struct device_driver *driver; 
    
    /* Linked list pointer to attach this hardware to the global Platform Bus */
    struct platform_device *next; 
};

/* ==========================================================================
 * 4. PLATFORM BUS CORE API
 * ========================================================================== */

/**
 * @brief Scans the Device Tree Blob (DTB), dynamically allocates 
 * a platform_device for every compatible node, and adds it to the bus.
 */
void platform_bus_enumerate(void *dtb);

/**
 * @brief Iterates over the global platform_device linked list, checks the 
 * linker array for a driver with a matching 'compatible' string, 
 * and executes the driver's probe() function.
 */
int platform_bus_match_drivers(void *dtb);

/**
 * @brief Fetches a fully initialized device from the bus by its node name.
 * Useful for console routing.
 */
struct platform_device* platform_bus_get_device(const char *name);

#endif /* _KERNEL_DEVICE_H */
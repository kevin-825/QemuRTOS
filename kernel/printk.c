/* kernel/printk.c */
#include "kernel/printk.h"
#include "device.h"
#include "drivers/uart.h"
#include <stdarg.h>

#include <stdarg.h>
#define NULL ((void *)0)

/* Store the console device pointer globally so we don't have to look it up every time */
static const struct device *console_dev = NULL;

void console_init(void) {
    /* Grab the UART from the Device Subsystem we just built! */
    console_dev = device_get_binding("uart0");
}

/* Internal helper: Print a single character to the console */
static void print_char(char c) {
    if (!console_dev) return; /* Drop characters if console isn't ready */
    
    /* Auto-inject carriage return for serial terminals */
    if (c == '\n') {
        uart_poll_out(console_dev, '\r');
    }
    uart_poll_out(console_dev, c);
}

/* kernel/printk.c */
/* Move this above printk */
static void print_string(const char *s) {
    if (!s) s = "(null)";
    while (*s) {
        print_char(*s++);
    }
}

static void print_int(int num, int base, int width, char pad_char, int is_signed) {
    char buf[32];
    int i = 0;
    unsigned int unum;

    if (is_signed && num < 0) {
        print_char('-');
        unum = (unsigned int)(-num);
    } else {
        unum = (unsigned int)num;
    }

    if (unum == 0) {
        buf[i++] = '0';
    } else {
        while (unum > 0) {
            int rem = unum % base;
            buf[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
            unum /= base;
        }
    }

    /* Here is the new padding logic! */
    while (i < width && i < 32) {
        buf[i++] = pad_char;
    }

    /* Print in reverse */
    while (i > 0) {
        print_char(buf[--i]);
    }
}

void printk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {
        if (*p != '%') {
            print_char(*p);
            continue;
        }

        p++; /* Skip the '%' */

        /* 1. Check for padding character (usually '0') */
        char pad_char = ' ';
        if (*p == '0') {
            pad_char = '0';
            p++;
        }

        /* 2. Check for width (e.g., the '8' in %08x) */
        int width = 0;
        while (*p >= '0' && *p <= '9') {
            width = (width * 10) + (*p - '0');
            p++;
        }

        /* 3. Handle the format specifier */
        switch (*p) {
            case 'd': /* Signed decimal */
                print_int(va_arg(args, int), 10, width, pad_char, 1);
                break;
            case 'u': /* Unsigned decimal */
                print_int(va_arg(args, unsigned int), 10, width, pad_char, 0);
                break;
            case 'x': /* Hexadecimal */
                print_int(va_arg(args, unsigned int), 16, width, pad_char, 0);
                break;
            case 'p': /* Pointer */
                print_string("0x");
                print_int(va_arg(args, unsigned int), 16, 8, '0', 0); /* Force 8-char zero padding */
                break;
            case 's': /* String */
                char *s = va_arg(args, char *);
                print_string(s ? s : "(null)");
                break;
            case 'c': /* Character */
                print_char((char)va_arg(args, int));
                break;
            default:
                print_char('%');
                print_char(*p);
                break;
        }
    }
    va_end(args);
}
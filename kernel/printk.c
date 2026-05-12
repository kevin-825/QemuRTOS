/* kernel/printk.c */
#include "kernel/printk.h"
#include "kernel/device.h"
#include "drivers/uart.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define NULL ((void *)0)

static const struct device *console_dev;

void console_init(void) {
    console_dev = device_get_binding("uart0");
}

static void print_char(char c) {
    if (!console_dev) return; 
    
    if (c == '\n') {
        uart_poll_out(console_dev, '\r');
    }
    uart_poll_out(console_dev, c);
}

/* -------------------------------------------------------------------
 * The String Engine (Supports Width, Precision, and Left-Justification)
 * ------------------------------------------------------------------- */
static void print_string_fmt(const char *s, int width, int precision, int flags_left) {
    if (!s) s = "(null)";
    
    int len = 0;
    /* Calculate length, but stop early if we hit the Precision limit */
    while (s[len] && (precision == -1 || len < precision)) {
        len++;
    }

    int pad = width - len;

    /* If right-justified, print spaces FIRST */
    if (!flags_left) {
        while (pad-- > 0) print_char(' ');
    }

    /* Print the actual string */
    for (int i = 0; i < len; i++) {
        print_char(s[i]);
    }

    /* If left-justified, print spaces LAST */
    if (flags_left) {
        while (pad-- > 0) print_char(' ');
    }
}

/* -------------------------------------------------------------------
 * The Core Number Engine (ISO C Compliant)
 * Handles all combinations of +, -, space, #, 0, width, and precision.
 * ------------------------------------------------------------------- */
static void print_number(unsigned long long num, int base, int width, int precision,
                         int flags_left, int flags_plus, int flags_space, int flags_hash, int flags_zero,
                         int is_signed, int is_upper) {
    char buf[64];
    int i = 0;
    char prefix[4] = {0};
    int prefix_len = 0;

    /* 1. Handle Sign and Positional Prefixes */
    if (is_signed) {
        long long snum = (long long)num;
        if (snum < 0) {
            prefix[prefix_len++] = '-';
            num = (unsigned long long)(-snum);
        } else if (flags_plus) {
            prefix[prefix_len++] = '+';
        } else if (flags_space) {
            prefix[prefix_len++] = ' ';
        }
    }

    /* 2. Generate the raw number string (backwards) */
    if (num == 0) {
        buf[i++] = '0';
    } else {
        while (num > 0) {
            int rem = num % base;
            char hex_base = is_upper ? 'A' : 'a';
            buf[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + hex_base);
            num /= base;
        }
    }

    /* 3. Alternate Form Prefix (0x for hex) */
    if (flags_hash && num != 0 && base == 16) {
        prefix[prefix_len++] = '0';
        prefix[prefix_len++] = is_upper ? 'X' : 'x';
    }

    /* 4. ISO C Rule: If Precision is specified, Zero-Padding is IGNORED */
    if (precision >= 0) {
        flags_zero = 0;
    }

    /* 5. Calculate Zeros needed for Precision */
    int zeros = 0;
    if (precision > i) {
        zeros = precision - i;
    }

    /* 6. Calculate Spaces needed for Width */
    int pad = width - (i + zeros + prefix_len);

    /* 7. If Zero Padding is active (and not left justified), convert spaces to zeros */
    if (flags_zero && !flags_left && pad > 0) {
        zeros += pad;
        pad = 0;
    }

    /* --- THE FINAL RENDER ORDER --- */
    
    /* A. Right Justification Spaces */
    if (!flags_left) {
        while (pad-- > 0) print_char(' ');
    }

    /* B. The Prefix (+, -, or 0x) */
    for (int j = 0; j < prefix_len; j++) print_char(prefix[j]);

    /* C. The Leading Zeros */
    while (zeros-- > 0) print_char('0');

    /* D. The Actual Number */
    while (i > 0) print_char(buf[--i]);

    /* E. Left Justification Spaces */
    if (flags_left) {
        while (pad-- > 0) print_char(' ');
    }
}

/* -------------------------------------------------------------------
 * The Architecture-Aware Format Parser State Machine
 * ------------------------------------------------------------------- */
void printk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {
        if (*p != '%') {
            print_char(*p);
            continue;
        }
        p++; /* Skip the '%' */

        /* State 1: Parse Flags */
        int flags_left = 0, flags_plus = 0, flags_space = 0, flags_hash = 0, flags_zero = 0;
        while (1) {
            if (*p == '-') flags_left = 1;
            else if (*p == '+') flags_plus = 1;
            else if (*p == ' ') flags_space = 1;
            else if (*p == '#') flags_hash = 1;
            else if (*p == '0') flags_zero = 1;
            else break;
            p++;
        }

        /* State 2: Parse Width */
        int width = 0;
        while (*p >= '0' && *p <= '9') {
            width = (width * 10) + (*p - '0');
            p++;
        }

        /* State 3: Parse Precision */
        int precision = -1; /* -1 means unspecified */
        if (*p == '.') {
            p++;
            precision = 0;
            while (*p >= '0' && *p <= '9') {
                precision = (precision * 10) + (*p - '0');
                p++;
            }
        }

        /* State 4: Parse Size Modifiers (z, l, ll) */
        int is_long = 0;
        int is_size_t = 0;
        while (*p == 'l' || *p == 'z') {
            if (*p == 'l') is_long++;
            else if (*p == 'z') is_size_t = 1;
            p++;
        }

        /* State 5: The Format Specifier */
        unsigned long long val = 0;

        switch (*p) {
            case 'd':
            case 'i':
                if (is_size_t) val = (unsigned long long)va_arg(args, size_t);
                else if (is_long >= 2) val = va_arg(args, long long);
                else if (is_long == 1) val = va_arg(args, long);
                else val = va_arg(args, int);
                print_number(val, 10, width, precision, flags_left, flags_plus, flags_space, flags_hash, flags_zero, 1, 0);
                break;

            case 'u':
            case 'x':
            case 'X':
                if (is_size_t) val = va_arg(args, size_t);
                else if (is_long >= 2) val = va_arg(args, unsigned long long);
                else if (is_long == 1) val = va_arg(args, unsigned long);
                else val = va_arg(args, unsigned int);
                
                int base = (*p == 'u') ? 10 : 16;
                int upper = (*p == 'X') ? 1 : 0;
                print_number(val, base, width, precision, flags_left, flags_plus, flags_space, flags_hash, flags_zero, 0, upper);
                break;

            case 'p':
                /* Force 0x prefix and zero-pad to exactly the width of the CPU architecture */
                val = (unsigned long long)(uintptr_t)va_arg(args, void *);
                int ptr_width = sizeof(void *) * 2; /* 16 chars for 64-bit, 8 chars for 32-bit */
                
                /* Auto-inject the '#' and '0' flags to mimic standard %p behavior */
                print_number(val, 16, ptr_width + 2, -1, flags_left, 0, 0, 1, 1, 0, 0);
                break;

            case 's':
                print_string_fmt(va_arg(args, char *), width, precision, flags_left);
                break;

            case 'c':
                print_char((char)va_arg(args, int));
                break;

            case '%':
                print_char('%');
                break;

            default:
                print_char('%');
                print_char(*p);
                break;
        }
    }
    va_end(args);
}
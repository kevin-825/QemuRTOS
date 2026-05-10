/* include/kernel/printk.h */
#ifndef PRINTK_H
#define PRINTK_H

/* Initialize the console routing */
void console_init(void);

/* The main kernel print function */
void printk(const char *fmt, ...);

/* 1. Define the severity levels */
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_FATAL 1
#define LOG_LEVEL_ERR   2
#define LOG_LEVEL_WARN  3
#define LOG_LEVEL_INFO  4
#define LOG_LEVEL_DEBUG 5   

/* 2. Set the global compile-time log level. 
 * In a real OS, this comes from menuconfig (e.g., CONFIG_LOG_DEFAULT_LEVEL).
 * For now, we hardcode it to INFO (hides DEBUG messages). 
 * LOG_LEVEL_DEBUG means print all log messages, 
 * LOG_LEVEL_INFO means print INFO and above, etc.
 */
#ifndef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

/* 3. The GCC Macro Magic 
 * We use 'do { ... } while(0)' to make these macros perfectly safe inside if/else statements.
 */

#if CONFIG_LOG_LEVEL >= LOG_LEVEL_FATAL
#define pr_fatal(fmt, ...) \
    do { printk("[FATAL] " fmt, ##__VA_ARGS__); } while(0)
#else
#define pr_fatal(fmt, ...) do { } while(0)
#endif

#if CONFIG_LOG_LEVEL >= LOG_LEVEL_ERR
#define pr_err(fmt, ...) \
    do { printk("[ERR]   " fmt, ##__VA_ARGS__); } while(0)
#else
#define pr_err(fmt, ...) do { } while(0)
#endif

#if CONFIG_LOG_LEVEL >= LOG_LEVEL_WARN
#define pr_warn(fmt, ...) \
    do { printk("[WARN]  " fmt, ##__VA_ARGS__); } while(0)
#else
#define pr_warn(fmt, ...) do { } while(0)
#endif

#if CONFIG_LOG_LEVEL >= LOG_LEVEL_INFO
#define pr_info(fmt, ...) \
    do { printk("[INFO]  " fmt, ##__VA_ARGS__); } while(0)
#else
#define pr_info(fmt, ...) do { } while(0)
#endif

#if CONFIG_LOG_LEVEL >= LOG_LEVEL_DEBUG
#define pr_debug(fmt, ...) \
    do { printk("[DEBUG] " fmt, ##__VA_ARGS__); } while(0)
#else
#define pr_debug(fmt, ...) do { } while(0)
#endif

#endif
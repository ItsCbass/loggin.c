// logger.h — Simple C Logging Library Header
//
// repo   : https://github.com/ItsCbass/loggin.c
// docs   : https://github.com/ItsCbass/loggin.c
// author : https://github.com/ItsCbass
//
// Developed with ❤️ by Sebastian Rivera.

#ifndef LOGGIN_H
#define LOGGIN_H

// ╔══════════════════════════════════════ PACK ══════════════════════════════════════╗

    #include <stdio.h>
    #include <stdarg.h>
    #include <stdbool.h>
    #include <time.h>

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════ INIT ══════════════════════════════════════╗

    #define LOGGER_VERSION "1.0.0"

    /* Log levels */
    typedef enum {
        LOG_LEVEL_TRACE = 0,
        LOG_LEVEL_DEBUG = 1,
        LOG_LEVEL_INFO  = 2,
        LOG_LEVEL_WARN  = 3,
        LOG_LEVEL_ERROR = 4,
        LOG_LEVEL_FATAL = 5
    } log_level_t;

    /* Log event structure */
    typedef struct {
        va_list ap;
        const char *fmt;
        const char *file;
        const char *function;
        struct tm *time;
        void *user_data;
        int line;
        log_level_t level;
    } log_event_t;

    /* Function pointer types */
    typedef void (*log_output_fn_t)(log_event_t *event);
    typedef void (*log_lock_fn_t)(bool lock, void *user_data);

    /* Configuration structure */
    typedef struct {
        log_level_t level;
        bool quiet;
        bool use_colors;
        bool show_file_line;
        bool show_function;
        log_lock_fn_t lock_fn;
        void *lock_data;
    } log_config_t;

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════ CORE ══════════════════════════════════════╗

    /* Convenience macros */
    #define log_trace(...) logger_log(LOG_LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define log_debug(...) logger_log(LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define log_info(...)  logger_log(LOG_LEVEL_INFO,  __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define log_warn(...)  logger_log(LOG_LEVEL_WARN,  __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define log_error(...) logger_log(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define log_fatal(...) logger_log(LOG_LEVEL_FATAL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

    /* Core API functions */
    void logger_init(void);
    void logger_cleanup(void);

    /* Configuration functions */
    void logger_set_level(log_level_t level);
    void logger_set_quiet(bool quiet);
    void logger_set_colors(bool use_colors);
    void logger_set_show_file_line(bool show);
    void logger_set_show_function(bool show);
    void logger_set_lock(log_lock_fn_t fn, void *user_data);

    /* Output functions */
    int logger_add_console_output(log_level_t level);
    int logger_add_file_output(FILE *file, log_level_t level);
    int logger_add_custom_output(log_output_fn_t output_fn, void *user_data, log_level_t level);

    /* Utility functions */
    const char* logger_level_to_string(log_level_t level);
    log_level_t logger_string_to_level(const char *str);
    void logger_log(log_level_t level, const char *file, const char *function, int line, const char *fmt, ...);

    /* Built-in output functions */
    void logger_console_output(log_event_t *event);
    void logger_file_output(log_event_t *event);

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

#endif /* LOGGIN_H */

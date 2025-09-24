// logger.c — Simple C Logging Library Implementation
//
// repo   : https://github.com/ItsCbass/loggin.c
// docs   : https://github.com/ItsCbass/loggin.c
// author : https://github.com/ItsCbass
//
// Developed with ❤️ by Sebastian Rivera.

#include "../loggin.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// ╔══════════════════════════════════════ INIT ══════════════════════════════════════╗

    #define MAX_OUTPUTS 16

    /* Output handler structure */
    typedef struct {
        log_output_fn_t output_fn;
        void *user_data;
        log_level_t min_level;
        bool active;
    } output_handler_t;

    /* Global logger state */
    static struct {
        log_config_t config;
        output_handler_t outputs[MAX_OUTPUTS];
        bool initialized;
    } logger_state = {0};

    /* Level strings */
    static const char *level_strings[] = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };

    /* Level colors (ANSI escape codes) */
    static const char *level_colors[] = {
        "\x1b[94m",  /* TRACE - Blue */
        "\x1b[36m",  /* DEBUG - Cyan */
        "\x1b[32m",  /* INFO - Green */
        "\x1b[33m",  /* WARN - Yellow */
        "\x1b[31m",  /* ERROR - Red */
        "\x1b[35m"   /* FATAL - Magenta */
    };

    /* Reset color */
    static const char *color_reset = "\x1b[0m";

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════ CORE ══════════════════════════════════════╗

    // ┌──────────────────────────── THREAD SAFETY ────────────────────────────┐

        /* Thread safety functions */
        static void lock_logger(void) {
            if (logger_state.config.lock_fn) {
                logger_state.config.lock_fn(true, logger_state.config.lock_data);
            }
        }

        static void unlock_logger(void) {
            if (logger_state.config.lock_fn) {
                logger_state.config.lock_fn(false, logger_state.config.lock_data);
            }
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── INITIALIZATION ────────────────────────────┐

        /// Initialize logger with default configuration.
        ///
        /// Sets up the logger with sensible defaults including console output
        /// and INFO level logging. Safe to call multiple times.
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_init(void) {
            if (logger_state.initialized) {
                return;
            }
            
            memset(&logger_state, 0, sizeof(logger_state));
            
            /* Set default configuration */
            logger_state.config.level = LOG_LEVEL_INFO;
            logger_state.config.quiet = false;
            logger_state.config.use_colors = true;
            logger_state.config.show_file_line = true;
            logger_state.config.show_function = false;
            logger_state.config.lock_fn = NULL;
            logger_state.config.lock_data = NULL;
            
            /* Add default console output */
            logger_add_console_output(LOG_LEVEL_TRACE);
            
            logger_state.initialized = true;
        }

        /// Cleanup logger resources.
        ///
        /// Closes any open file handles and resets the logger state.
        /// Should be called before program termination.
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_cleanup(void) {
            if (!logger_state.initialized) {
                return;
            }
            
            /* Close any file outputs */
            for (int i = 0; i < MAX_OUTPUTS; i++) {
                if (logger_state.outputs[i].active && 
                    logger_state.outputs[i].output_fn == logger_file_output) {
                    FILE *file = (FILE*)logger_state.outputs[i].user_data;
                    if (file && file != stdout && file != stderr) {
                        fclose(file);
                    }
                }
            }
            
            memset(&logger_state, 0, sizeof(logger_state));
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── CONFIGURATION ────────────────────────────┐

        /// Set the minimum log level.
        ///
        /// Messages below this level will not be processed or output.
        ///
        /// __Parameters__
        ///
        /// - `level`: Minimum log level (LOG_LEVEL_TRACE to LOG_LEVEL_FATAL)
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_set_level(log_level_t level) {
            lock_logger();
            logger_state.config.level = level;
            unlock_logger();
        }

        /// Enable or disable all logging output.
        ///
        /// When quiet mode is enabled, no messages will be output regardless
        /// of their level.
        ///
        /// __Parameters__
        ///
        /// - `quiet`: true to disable output, false to enable
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_set_quiet(bool quiet) {
            lock_logger();
            logger_state.config.quiet = quiet;
            unlock_logger();
        }

        /// Enable or disable color output for console.
        ///
        /// Colors are only applied to console output, not file output.
        ///
        /// __Parameters__
        ///
        /// - `use_colors`: true to enable colors, false to disable
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_set_colors(bool use_colors) {
            lock_logger();
            logger_state.config.use_colors = use_colors;
            unlock_logger();
        }

        /// Show or hide file and line number information.
        ///
        /// When enabled, log messages will include the source file and line
        /// number where the log call was made.
        ///
        /// __Parameters__
        ///
        /// - `show`: true to show file:line, false to hide
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_set_show_file_line(bool show) {
            lock_logger();
            logger_state.config.show_file_line = show;
            unlock_logger();
        }

        /// Show or hide function name information.
        ///
        /// When enabled, log messages will include the function name where
        /// the log call was made.
        ///
        /// __Parameters__
        ///
        /// - `show`: true to show function name, false to hide
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_set_show_function(bool show) {
            lock_logger();
            logger_state.config.show_function = show;
            unlock_logger();
        }

        /// Set thread safety lock function.
        ///
        /// Provides a way to make the logger thread-safe by providing
        /// custom locking mechanisms.
        ///
        /// __Parameters__
        ///
        /// - `lock_fn`: Function to call for locking/unlocking
        /// - `user_data`: User data to pass to the lock function
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_set_lock(log_lock_fn_t lock_fn, void *user_data) {
            lock_logger();
            logger_state.config.lock_fn = lock_fn;
            logger_state.config.lock_data = user_data;
            unlock_logger();
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── OUTPUT MANAGEMENT ────────────────────────────┐

        /// Add console output handler.
        ///
        /// Adds stderr as an output destination with the specified minimum level.
        ///
        /// __Parameters__
        ///
        /// - `level`: Minimum log level for this output
        ///
        /// __Return__
        ///
        /// - 0 on success, -1 on failure (no free slots)
        int logger_add_console_output(log_level_t level) {
            return logger_add_custom_output(logger_console_output, stderr, level);
        }

        /// Add file output handler.
        ///
        /// Adds a file as an output destination with the specified minimum level.
        ///
        /// __Parameters__
        ///
        /// - `file`: File pointer to write to
        /// - `level`: Minimum log level for this output
        ///
        /// __Return__
        ///
        /// - 0 on success, -1 on failure (no free slots or invalid file)
        int logger_add_file_output(FILE *file, log_level_t level) {
            if (!file) {
                return -1;
            }
            return logger_add_custom_output(logger_file_output, file, level);
        }

        /// Add custom output handler.
        ///
        /// Adds a custom output function with the specified minimum level.
        ///
        /// __Parameters__
        ///
        /// - `output_fn`: Custom output function
        /// - `user_data`: User data to pass to the output function
        /// - `level`: Minimum log level for this output
        ///
        /// __Return__
        ///
        /// - 0 on success, -1 on failure (no free slots or invalid function)
        int logger_add_custom_output(log_output_fn_t output_fn, void *user_data, log_level_t level) {
            if (!output_fn) {
                return -1;
            }
            
            lock_logger();
            
            for (int i = 0; i < MAX_OUTPUTS; i++) {
                if (!logger_state.outputs[i].active) {
                    logger_state.outputs[i].output_fn = output_fn;
                    logger_state.outputs[i].user_data = user_data;
                    logger_state.outputs[i].min_level = level;
                    logger_state.outputs[i].active = true;
                    unlock_logger();
                    return 0;
                }
            }
            
            unlock_logger();
            return -1; /* No free slots */
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── UTILITY FUNCTIONS ────────────────────────────┐

        /// Convert log level to string representation.
        ///
        /// Returns a human-readable string for the given log level.
        ///
        /// __Parameters__
        ///
        /// - `level`: Log level to convert
        ///
        /// __Return__
        ///
        /// - String representation of the level, or "UNKNOWN" for invalid levels
        const char* logger_level_to_string(log_level_t level) {
            if (level >= 0 && level < 6) {
                return level_strings[level];
            }
            return "UNKNOWN";
        }

        /// Convert string to log level.
        ///
        /// Parses a string representation of a log level (case-insensitive).
        ///
        /// __Parameters__
        ///
        /// - `str`: String to parse (e.g., "INFO", "error", "WARN")
        ///
        /// __Return__
        ///
        /// - Corresponding log level, or LOG_LEVEL_INFO for invalid strings
        log_level_t logger_string_to_level(const char *str) {
            if (!str) return LOG_LEVEL_INFO;
            
            for (int i = 0; i < 6; i++) {
                if (strcasecmp(str, level_strings[i]) == 0) {
                    return (log_level_t)i;
                }
            }
            return LOG_LEVEL_INFO;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── MAIN LOGGING ────────────────────────────┐

        /* Initialize event with current time */
        static void init_event(log_event_t *event, void *user_data) {
            if (!event->time) {
                time_t t = time(NULL);
                event->time = localtime(&t);
            }
            event->user_data = user_data;
        }

        /// Main logging function.
        ///
        /// Processes a log message and sends it to all appropriate output handlers.
        /// This is the core function that all logging macros ultimately call.
        ///
        /// __Parameters__
        ///
        /// - `level`: Log level of the message
        /// - `file`: Source file name (usually __FILE__)
        /// - `function`: Function name (usually __FUNCTION__)
        /// - `line`: Line number (usually __LINE__)
        /// - `fmt`: printf-style format string
        /// - `...`: Variable arguments for the format string
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_log(log_level_t level, const char *file, const char *function, int line, const char *fmt, ...) {
            if (!logger_state.initialized) {
                logger_init();
            }
            
            log_event_t event = {
                .fmt = fmt,
                .file = file,
                .function = function,
                .line = line,
                .level = level,
                .time = NULL,
                .user_data = NULL
            };
            
            lock_logger();
            
            /* Check if we should log this level */
            if (level < logger_state.config.level) {
                unlock_logger();
                return;
            }
            
            /* Process all active outputs */
            for (int i = 0; i < MAX_OUTPUTS; i++) {
                if (logger_state.outputs[i].active && 
                    level >= logger_state.outputs[i].min_level) {
                    
                    init_event(&event, logger_state.outputs[i].user_data);
                    va_start(event.ap, fmt);
                    logger_state.outputs[i].output_fn(&event);
                    va_end(event.ap);
                }
            }
            
            unlock_logger();
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── BUILT-IN OUTPUTS ────────────────────────────┐

        /// Built-in console output function.
        ///
        /// Formats and outputs log messages to console with colors and
        /// customizable formatting options.
        ///
        /// __Parameters__
        ///
        /// - `event`: Log event to output
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_console_output(log_event_t *event) {
            FILE *stream = (FILE*)event->user_data;
            char time_buf[32];
            
            /* Format timestamp */
            strftime(time_buf, sizeof(time_buf), "%H:%M:%S", event->time);
            
            /* Print timestamp and level */
            if (logger_state.config.use_colors) {
                fprintf(stream, "%s %s%-5s%s ", 
                        time_buf, 
                        level_colors[event->level], 
                        level_strings[event->level], 
                        color_reset);
            } else {
                fprintf(stream, "%s %-5s ", time_buf, level_strings[event->level]);
            }
            
            /* Print file and line if enabled */
            if (logger_state.config.show_file_line) {
                if (logger_state.config.use_colors) {
                    fprintf(stream, "%s%s:%d:%s ", 
                            color_reset, event->file, event->line, color_reset);
                } else {
                    fprintf(stream, "%s:%d: ", event->file, event->line);
                }
            }
            
            /* Print function if enabled */
            if (logger_state.config.show_function) {
                if (logger_state.config.use_colors) {
                    fprintf(stream, "%s[%s]%s ", 
                            color_reset, event->function, color_reset);
                } else {
                    fprintf(stream, "[%s] ", event->function);
                }
            }
            
            /* Print the actual message */
            vfprintf(stream, event->fmt, event->ap);
            fprintf(stream, "\n");
            fflush(stream);
        }

        /// Built-in file output function.
        ///
        /// Formats and outputs log messages to files with detailed timestamps
        /// and no color codes.
        ///
        /// __Parameters__
        ///
        /// - `event`: Log event to output
        ///
        /// __Return__
        ///
        /// - No return value
        void logger_file_output(log_event_t *event) {
            FILE *file = (FILE*)event->user_data;
            char time_buf[64];
            
            /* Format timestamp with date */
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", event->time);
            
            /* Print to file */
            fprintf(file, "%s %-5s %s:%d", 
                    time_buf, 
                    level_strings[event->level], 
                    event->file, 
                    event->line);
            
            if (logger_state.config.show_function) {
                fprintf(file, " [%s]", event->function);
            }
            
            fprintf(file, ": ");
            vfprintf(file, event->fmt, event->ap);
            fprintf(file, "\n");
            fflush(file);
        }

    // └────────────────────────────────────────────────────────────────────┘

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

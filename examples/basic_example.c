// basic_example.c — Basic Logging Example
//
// repo   : https://github.com/ItsCbass/loggin.c
// docs   : https://github.com/ItsCbass/loggin.c
// author : https://github.com/ItsCbass
//
// Developed with ❤️ by Sebastian Rivera.

#include "../lib/loggin.h"
#include <unistd.h>

// ╔══════════════════════════════════════ INIT ══════════════════════════════════════╗

    /* Example function for demonstrating logging in different contexts */
    void test_function(void) {
        log_info("This is an info message from test_function");
        log_debug("Debug message with variable: %d", 42);
        log_warn("Warning: This is just a test warning");
    }

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════ CORE ══════════════════════════════════════╗

    int main(void) {
        // ┌──────────────────────────── INITIALIZATION ────────────────────────────┐

            /* Initialize the logger */
            logger_init();
            
            printf("=== Basic Logging Example ===\n\n");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── BASIC LOGGING ────────────────────────────┐

            /* Test all log levels */
            log_trace("This is a trace message");
            log_debug("This is a debug message");
            log_info("This is an info message");
            log_warn("This is a warning message");
            log_error("This is an error message");
            log_fatal("This is a fatal message");
            
            printf("\n=== Function Call Example ===\n\n");
            test_function();

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── LOG LEVEL CONTROL ────────────────────────────┐

            printf("\n=== Different Log Levels ===\n\n");
            
            /* Change log level to only show warnings and above */
            logger_set_level(LOG_LEVEL_WARN);
            printf("Log level set to WARN - only warnings and above will show:\n");
            log_trace("This trace message should not appear");
            log_debug("This debug message should not appear");
            log_info("This info message should not appear");
            log_warn("This warning message should appear");
            log_error("This error message should appear");
            log_fatal("This fatal message should appear");
            
            /* Reset to show all levels */
            logger_set_level(LOG_LEVEL_TRACE);
            printf("\nLog level reset to TRACE - all messages will show:\n");
            log_info("All levels are now visible again");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── QUIET MODE ────────────────────────────┐

            printf("\n=== Quiet Mode Test ===\n\n");
            logger_set_quiet(true);
            printf("Quiet mode enabled - no messages should appear:\n");
            log_info("This message should not appear");
            log_error("This error should not appear either");
            
            logger_set_quiet(false);
            printf("Quiet mode disabled - messages should appear again:\n");
            log_info("Messages are visible again");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── CLEANUP ────────────────────────────┐

            /* Cleanup */
            logger_cleanup();
            
            printf("\n=== Example Complete ===\n");
            return 0;

        // └────────────────────────────────────────────────────────────────────┘
    }

// ╚═════════════════════════════════════════════════════════════════════════════════════╝
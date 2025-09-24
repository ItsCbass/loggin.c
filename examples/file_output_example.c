// file_output_example.c — File Output Example
//
// repo   : https://github.com/ItsCbass/loggin.c
// docs   : https://github.com/ItsCbass/loggin.c
// author : https://github.com/ItsCbass
//
// Developed with ❤️ by Sebastian Rivera.

#include "../lib/loggin.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// ╔══════════════════════════════════════ CORE ══════════════════════════════════════╗

    int main(void) {
        // ┌──────────────────────────── INITIALIZATION ────────────────────────────┐

            FILE *log_file;
            
            /* Initialize the logger */
            logger_init();
            
            printf("=== File Output Example ===\n\n");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── FILE SETUP ────────────────────────────┐

            /* Create a log file */
            log_file = fopen("example.log", "w");
            if (!log_file) {
                log_error("Failed to open log file: %s", strerror(errno));
                return 1;
            }
            
            /* Add file output to the logger */
            if (logger_add_file_output(log_file, LOG_LEVEL_TRACE) != 0) {
                log_error("Failed to add file output");
                fclose(log_file);
                return 1;
            }
            
            printf("Logging to both console and file 'example.log'\n\n");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── BASIC LOGGING ────────────────────────────┐

            /* Log some messages */
            log_info("Application started");
            log_debug("Debug information: %s", "Some debug data");
            log_warn("This is a warning message");
            log_error("An error occurred: %s", "File not found");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── LEVEL FILTERING ────────────────────────────┐

            /* Test with different levels */
            logger_set_level(LOG_LEVEL_WARN);
            printf("\nLog level set to WARN - only warnings and above will be logged to file:\n");
            log_info("This info message should not appear in file");
            log_warn("This warning should appear in file");
            log_error("This error should appear in file");
            
            /* Reset level */
            logger_set_level(LOG_LEVEL_TRACE);

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── FINAL LOGGING ────────────────────────────┐

            /* Log some more messages */
            log_info("Application shutting down");
            log_debug("Cleanup completed");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── CLEANUP ────────────────────────────┐

            /* Close the file */
            fclose(log_file);
            
            printf("\nLog file 'example.log' has been created. Check its contents!\n");
            printf("=== Example Complete ===\n");
            
            /* Cleanup */
            logger_cleanup();
            
            return 0;

        // └────────────────────────────────────────────────────────────────────┘
    }

// ╚═════════════════════════════════════════════════════════════════════════════════════╝
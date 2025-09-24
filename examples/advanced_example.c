// advanced_example.c — Advanced Features Example
//
// repo   : https://github.com/ItsCbass/loggin.c
// docs   : https://github.com/ItsCbass/loggin.c
// author : https://github.com/ItsCbass
//
// Developed with ❤️ by Sebastian Rivera.

#include "../lib/loggin.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// ╔══════════════════════════════════════ INIT ══════════════════════════════════════╗

    /* Custom output function that logs to a custom format */
    void custom_output(log_event_t *event) {
        FILE *file = (FILE*)event->user_data;
        char time_buf[64];
        
        /* Custom timestamp format */
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", event->time);
        
        /* Custom format: [TIMESTAMP] LEVEL: MESSAGE */
        fprintf(file, "[%s] %s: ", time_buf, logger_level_to_string(event->level));
        vfprintf(file, event->fmt, event->ap);
        fprintf(file, "\n");
        fflush(file);
    }

    /* Simple mutex for thread safety demonstration */
    static pthread_mutex_t logger_mutex = PTHREAD_MUTEX_INITIALIZER;

    void thread_lock_function(bool lock, void *user_data) {
        (void)user_data; /* Unused parameter */
        if (lock) {
            pthread_mutex_lock(&logger_mutex);
        } else {
            pthread_mutex_unlock(&logger_mutex);
        }
    }

    /* Thread function for demonstrating thread safety */
    void* thread_function(void *arg) {
        int thread_id = *(int*)arg;
        
        for (int i = 0; i < 5; i++) {
            log_info("Thread %d: Message %d", thread_id, i + 1);
            usleep(100000); /* Sleep for 100ms */
        }
        
        return NULL;
    }

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════ CORE ══════════════════════════════════════╗

    int main(void) {
        // ┌──────────────────────────── INITIALIZATION ────────────────────────────┐

            FILE *custom_file;
            pthread_t threads[3];
            int thread_ids[3] = {1, 2, 3};
            
            /* Initialize the logger */
            logger_init();
            
            printf("=== Advanced Logging Example ===\n\n");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── FORMATTING OPTIONS ────────────────────────────┐

            printf("1. Testing different formatting options:\n");
            
            /* Show file and line numbers */
            logger_set_show_file_line(true);
            logger_set_show_function(false);
            log_info("File and line numbers enabled");
            
            /* Show function names */
            logger_set_show_function(true);
            log_info("Function names also enabled");
            
            /* Disable colors */
            logger_set_colors(false);
            log_info("Colors disabled");
            
            /* Re-enable colors */
            logger_set_colors(true);
            log_info("Colors re-enabled");

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── CUSTOM OUTPUT ────────────────────────────┐

            printf("\n2. Testing custom output function:\n");
            
            /* Create a custom log file */
            custom_file = fopen("custom.log", "w");
            if (custom_file) {
                logger_add_custom_output(custom_output, custom_file, LOG_LEVEL_TRACE);
                log_info("This message uses custom formatting");
                log_warn("Custom format warning message");
                fclose(custom_file);
                printf("Custom log file 'custom.log' created with custom format\n");
            }

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── THREAD SAFETY ────────────────────────────┐

            printf("\n3. Testing thread safety:\n");
            
            /* Set up thread safety */
            logger_set_lock(thread_lock_function, NULL);
            
            /* Create multiple threads */
            for (int i = 0; i < 3; i++) {
                if (pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0) {
                    log_error("Failed to create thread %d", i);
                }
            }
            
            /* Wait for all threads to complete */
            for (int i = 0; i < 3; i++) {
                pthread_join(threads[i], NULL);
            }

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── LEVEL CONVERSION ────────────────────────────┐

            printf("\n4. Testing level conversion:\n");
            
            /* Test level string conversion */
            const char *level_str = "ERROR";
            log_level_t level = logger_string_to_level(level_str);
            log_info("String '%s' converted to level %d", level_str, level);
            
            const char *converted_str = logger_level_to_string(level);
            log_info("Level %d converted back to string '%s'", level, converted_str);

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── MULTIPLE OUTPUTS ────────────────────────────┐

            printf("\n5. Testing multiple outputs:\n");
            
            /* Add multiple outputs */
            FILE *info_file = fopen("info_only.log", "w");
            FILE *error_file = fopen("errors_only.log", "w");
            
            if (info_file && error_file) {
                logger_add_file_output(info_file, LOG_LEVEL_INFO);
                logger_add_file_output(error_file, LOG_LEVEL_ERROR);
                
                log_info("This should go to info_only.log");
                log_warn("This should go to info_only.log");
                log_error("This should go to both info_only.log and errors_only.log");
                log_fatal("This should go to both files");
                
                fclose(info_file);
                fclose(error_file);
                
                printf("Created 'info_only.log' (INFO and above) and 'errors_only.log' (ERROR and above)\n");
            }

        // └────────────────────────────────────────────────────────────────────┘

        // ┌──────────────────────────── CLEANUP ────────────────────────────┐

            /* Cleanup */
            logger_cleanup();
            pthread_mutex_destroy(&logger_mutex);
            
            printf("\n=== Advanced Example Complete ===\n");
            printf("Check the generated log files:\n");
            printf("- custom.log (custom format)\n");
            printf("- info_only.log (INFO level and above)\n");
            printf("- errors_only.log (ERROR level and above)\n");
            
            return 0;

        // └────────────────────────────────────────────────────────────────────┘
    }

// ╚═════════════════════════════════════════════════════════════════════════════════════╝
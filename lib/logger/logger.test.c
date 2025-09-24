// logger.test.c — Comprehensive Test Suite for Logger
//
// repo   : https://github.com/ItsCbass/loggin.c
// docs   : https://github.com/ItsCbass/loggin.c
// author : https://github.com/ItsCbass
//
// Developed with ❤️ by Sebastian Rivera.

#include "../loggin.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// ╔══════════════════════════════════════ INIT ══════════════════════════════════════╗

    /* Test data constants */
    const char *test_file = "test_file.c";
    const char *test_function = "test_function";
    const int test_line = 42;
    const char *test_message = "Test message";
    const char *test_format = "Test %s with %d";
    const char *test_arg1 = "message";
    const int test_arg2 = 42;

    /* Test output capture */
    static char captured_output[4096];
    static size_t captured_size = 0;

    /* Thread safety test data */
    static pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;
    static int thread_safety_counter = 0;

    /* Test counters */
    static int tests_run = 0;
    static int tests_passed = 0;
    static int tests_failed = 0;

// ╚═════════════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════ CORE ══════════════════════════════════════╗

    // ┌──────────────────────────── TEST UTILITIES ────────────────────────────┐

        /* Custom output function for testing */
        void test_output_capture(log_event_t *event) {
            char buffer[512];
            vsnprintf(buffer, sizeof(buffer), event->fmt, event->ap);
            
            if (captured_size < sizeof(captured_output) - 1) {
                size_t len = strlen(buffer);
                if (captured_size + len < sizeof(captured_output) - 1) {
                    strcat(captured_output, buffer);
                    captured_size += len;
                }
            }
        }

        /* Reset captured output */
        void reset_captured_output(void) {
            memset(captured_output, 0, sizeof(captured_output));
            captured_size = 0;
        }

        /* Thread lock function for testing */
        void test_thread_lock(bool lock, void *user_data) {
            (void)user_data;
            if (lock) {
                pthread_mutex_lock(&test_mutex);
            } else {
                pthread_mutex_unlock(&test_mutex);
            }
        }

        /* Test runner macro */
        #define RUN_TEST(test_name) do { \
            printf("Running test: %s... ", #test_name); \
            tests_run++; \
            if (test_name()) { \
                printf("✅ PASSED\n"); \
                tests_passed++; \
            } else { \
                printf("❌ FAILED\n"); \
                tests_failed++; \
            } \
        } while(0)

        /* Test assertion macro */
        #define TEST_ASSERT(condition) do { \
            if (!(condition)) { \
                printf("Assertion failed: %s\n", #condition); \
                return 0; \
            } \
        } while(0)

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── INITIALIZATION TESTS ────────────────────────────┐

        int test_logger_init_cleanup(void) {
            logger_init();
            logger_cleanup();
            // If we get here without crashing, the test passed
            return 1;
        }

        int test_multiple_init_calls(void) {
            logger_init();
            logger_init(); // Should be safe to call multiple times
            logger_cleanup();
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── CONFIGURATION TESTS ────────────────────────────┐

        int test_logger_set_level(void) {
            logger_init();
            logger_set_level(LOG_LEVEL_TRACE);
            logger_set_level(LOG_LEVEL_ERROR);
            logger_cleanup();
            return 1;
        }

        int test_logger_set_quiet(void) {
            logger_init();
            logger_set_quiet(true);
            logger_set_quiet(false);
            logger_cleanup();
            return 1;
        }

        int test_logger_set_colors(void) {
            logger_init();
            logger_set_colors(false);
            logger_set_colors(true);
            logger_cleanup();
            return 1;
        }

        int test_logger_set_show_file_line(void) {
            logger_init();
            logger_set_show_file_line(false);
            logger_set_show_file_line(true);
            logger_cleanup();
            return 1;
        }

        int test_logger_set_show_function(void) {
            logger_init();
            logger_set_show_function(true);
            logger_set_show_function(false);
            logger_cleanup();
            return 1;
        }

        int test_logger_set_lock(void) {
            logger_init();
            logger_set_lock(test_thread_lock, NULL);
            logger_cleanup();
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── OUTPUT MANAGEMENT TESTS ────────────────────────────┐

        int test_logger_add_console_output(void) {
            logger_init();
            int result = logger_add_console_output(LOG_LEVEL_INFO);
            TEST_ASSERT(result == 0);
            logger_cleanup();
            return 1;
        }

        int test_logger_add_file_output(void) {
            logger_init();
            FILE *test_file = tmpfile();
            TEST_ASSERT(test_file != NULL);
            int result = logger_add_file_output(test_file, LOG_LEVEL_DEBUG);
            TEST_ASSERT(result == 0);
            fclose(test_file);
            logger_cleanup();
            return 1;
        }

        int test_logger_add_file_output_invalid(void) {
            logger_init();
            int result = logger_add_file_output(NULL, LOG_LEVEL_INFO);
            TEST_ASSERT(result == -1);
            logger_cleanup();
            return 1;
        }

        int test_logger_add_custom_output(void) {
            logger_init();
            int result = logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            TEST_ASSERT(result == 0);
            logger_cleanup();
            return 1;
        }

        int test_logger_add_custom_output_invalid(void) {
            logger_init();
            int result = logger_add_custom_output(NULL, NULL, LOG_LEVEL_INFO);
            TEST_ASSERT(result == -1);
            logger_cleanup();
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── UTILITY FUNCTION TESTS ────────────────────────────┐

        int test_logger_level_to_string(void) {
            TEST_ASSERT(strcmp(logger_level_to_string(LOG_LEVEL_TRACE), "TRACE") == 0);
            TEST_ASSERT(strcmp(logger_level_to_string(LOG_LEVEL_DEBUG), "DEBUG") == 0);
            TEST_ASSERT(strcmp(logger_level_to_string(LOG_LEVEL_INFO), "INFO") == 0);
            TEST_ASSERT(strcmp(logger_level_to_string(LOG_LEVEL_WARN), "WARN") == 0);
            TEST_ASSERT(strcmp(logger_level_to_string(LOG_LEVEL_ERROR), "ERROR") == 0);
            TEST_ASSERT(strcmp(logger_level_to_string(LOG_LEVEL_FATAL), "FATAL") == 0);
            
            // Test invalid level
            TEST_ASSERT(strcmp(logger_level_to_string(-1), "UNKNOWN") == 0);
            TEST_ASSERT(strcmp(logger_level_to_string(99), "UNKNOWN") == 0);
            return 1;
        }

        int test_logger_string_to_level(void) {
            TEST_ASSERT(logger_string_to_level("TRACE") == LOG_LEVEL_TRACE);
            TEST_ASSERT(logger_string_to_level("DEBUG") == LOG_LEVEL_DEBUG);
            TEST_ASSERT(logger_string_to_level("INFO") == LOG_LEVEL_INFO);
            TEST_ASSERT(logger_string_to_level("WARN") == LOG_LEVEL_WARN);
            TEST_ASSERT(logger_string_to_level("ERROR") == LOG_LEVEL_ERROR);
            TEST_ASSERT(logger_string_to_level("FATAL") == LOG_LEVEL_FATAL);
            
            // Test case insensitive
            TEST_ASSERT(logger_string_to_level("trace") == LOG_LEVEL_TRACE);
            TEST_ASSERT(logger_string_to_level("info") == LOG_LEVEL_INFO);
            TEST_ASSERT(logger_string_to_level("Error") == LOG_LEVEL_ERROR);
            
            // Test invalid strings
            TEST_ASSERT(logger_string_to_level("INVALID") == LOG_LEVEL_INFO);
            TEST_ASSERT(logger_string_to_level(NULL) == LOG_LEVEL_INFO);
            TEST_ASSERT(logger_string_to_level("") == LOG_LEVEL_INFO);
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── LOGGING TESTS ────────────────────────────┐

        int test_logger_log_basic(void) {
            logger_init();
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            logger_log(LOG_LEVEL_INFO, test_file, test_function, test_line, test_message);
            
            TEST_ASSERT(strstr(captured_output, test_message) != NULL);
            
            logger_cleanup();
            return 1;
        }

        int test_logger_log_with_format(void) {
            logger_init();
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            logger_log(LOG_LEVEL_INFO, test_file, test_function, test_line, test_format, test_arg1, test_arg2);
            
            TEST_ASSERT(strstr(captured_output, test_arg1) != NULL);
            
            logger_cleanup();
            return 1;
        }

        int test_logger_log_level_filtering(void) {
            logger_init();
            logger_set_level(LOG_LEVEL_WARN);
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            logger_log(LOG_LEVEL_INFO, test_file, test_function, test_line, "This should not appear");
            logger_log(LOG_LEVEL_WARN, test_file, test_function, test_line, "This should appear");
            
            TEST_ASSERT(strstr(captured_output, "This should not appear") == NULL);
            TEST_ASSERT(strstr(captured_output, "This should appear") != NULL);
            
            logger_cleanup();
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── MACRO TESTS ────────────────────────────┐

        int test_log_trace_macro(void) {
            logger_init();
            logger_set_level(LOG_LEVEL_TRACE); // Set level to TRACE to allow trace messages
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            log_trace("Trace message");
            
            TEST_ASSERT(strstr(captured_output, "Trace message") != NULL);
            
            logger_cleanup();
            return 1;
        }

        int test_log_info_macro(void) {
            logger_init();
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            log_info("Info message");
            
            TEST_ASSERT(strstr(captured_output, "Info message") != NULL);
            
            logger_cleanup();
            return 1;
        }

        int test_log_error_macro(void) {
            logger_init();
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            log_error("Error message");
            
            TEST_ASSERT(strstr(captured_output, "Error message") != NULL);
            
            logger_cleanup();
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── THREAD SAFETY TESTS ────────────────────────────┐

        void* thread_safety_worker(void *arg) {
            (void)arg;
            for (int i = 0; i < 10; i++) {
                log_info("Thread message %d", i);
                thread_safety_counter++;
            }
            return NULL;
        }

        int test_thread_safety(void) {
            logger_init();
            logger_set_lock(test_thread_lock, NULL);
            
            pthread_t threads[3];
            thread_safety_counter = 0;
            
            // Create multiple threads
            for (int i = 0; i < 3; i++) {
                TEST_ASSERT(pthread_create(&threads[i], NULL, thread_safety_worker, NULL) == 0);
            }
            
            // Wait for all threads to complete
            for (int i = 0; i < 3; i++) {
                pthread_join(threads[i], NULL);
            }
            
            // Verify all messages were logged
            TEST_ASSERT(thread_safety_counter == 30);
            
            logger_cleanup();
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── EDGE CASE TESTS ────────────────────────────┐

        int test_empty_message(void) {
            logger_init();
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            logger_log(LOG_LEVEL_INFO, test_file, test_function, test_line, "");
            
            // Should not crash and should handle empty message gracefully
            TEST_ASSERT(captured_size == 0);
            
            logger_cleanup();
            return 1;
        }

        int test_null_file_name(void) {
            logger_init();
            reset_captured_output();
            logger_add_custom_output(test_output_capture, NULL, LOG_LEVEL_TRACE);
            
            logger_log(LOG_LEVEL_INFO, NULL, test_function, test_line, test_message);
            
            // Should not crash
            TEST_ASSERT(strstr(captured_output, test_message) != NULL);
            
            logger_cleanup();
            return 1;
        }

    // └────────────────────────────────────────────────────────────────────┘

    // ┌──────────────────────────── MAIN TEST RUNNER ────────────────────────────┐

        int main(void) {
            printf("🧪 Running Logger Test Suite\n");
            printf("============================\n\n");
            
            // Initialize tests
            tests_run = 0;
            tests_passed = 0;
            tests_failed = 0;
            
            // Run all tests
            RUN_TEST(test_logger_init_cleanup);
            RUN_TEST(test_multiple_init_calls);
            
            RUN_TEST(test_logger_set_level);
            RUN_TEST(test_logger_set_quiet);
            RUN_TEST(test_logger_set_colors);
            RUN_TEST(test_logger_set_show_file_line);
            RUN_TEST(test_logger_set_show_function);
            RUN_TEST(test_logger_set_lock);
            
            RUN_TEST(test_logger_add_console_output);
            RUN_TEST(test_logger_add_file_output);
            RUN_TEST(test_logger_add_file_output_invalid);
            RUN_TEST(test_logger_add_custom_output);
            RUN_TEST(test_logger_add_custom_output_invalid);
            
            RUN_TEST(test_logger_level_to_string);
            RUN_TEST(test_logger_string_to_level);
            
            RUN_TEST(test_logger_log_basic);
            RUN_TEST(test_logger_log_with_format);
            RUN_TEST(test_logger_log_level_filtering);
            
            RUN_TEST(test_log_trace_macro);
            RUN_TEST(test_log_info_macro);
            RUN_TEST(test_log_error_macro);
            
            RUN_TEST(test_thread_safety);
            
            RUN_TEST(test_empty_message);
            RUN_TEST(test_null_file_name);
            
            // Print results
            printf("\n============================\n");
            printf("📊 Test Results:\n");
            printf("   Total:  %d\n", tests_run);
            printf("   Passed: %d ✅\n", tests_passed);
            printf("   Failed: %d ❌\n", tests_failed);
            
            if (tests_failed == 0) {
                printf("\n🎉 All tests passed!\n");
                return 0;
            } else {
                printf("\n💥 Some tests failed!\n");
                return 1;
            }
        }

    // └────────────────────────────────────────────────────────────────────┘

// ╚═════════════════════════════════════════════════════════════════════════════════════╝
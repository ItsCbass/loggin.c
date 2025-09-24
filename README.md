<!--------------------------------- HEADER --------------------------------->

<div align="center">

# 🔥 loggin.c

*A lightweight, thread-safe logging library for C applications with support for multiple output destinations, customizable formatting, and color output.*

> ⚠️ **Note**: This is a **work in progress** and essentially a **toy project** for learning and experimentation. Nothing serious! 🧸

</div>

<!--------------------------------------------------------------------------->

<!--------------------------------- FEATURES --------------------------------->

## ✨ Features

- **🎨 Multiple Log Levels**: TRACE, DEBUG, INFO, WARN, ERROR, FATAL
- **📤 Flexible Output**: Console, files, and custom output functions
- **🔒 Thread Safety**: Built-in thread safety with customizable locking
- **🌈 Color Support**: ANSI color codes for console output
- **⚙️ Customizable Formatting**: Control what information is displayed
- **🚀 Easy Integration**: Simple API with convenient macros
- **📦 Zero Dependencies**: Only uses standard C library functions
- **🎯 Performance Optimized**: Efficient implementation with minimal overhead

<!--------------------------------------------------------------------------->

<!--------------------------------- VISUAL EXAMPLE --------------------------------->

## 🎨 What It Looks Like

Here's a sample of the beautiful colored output your logging library produces:

```
Logging to both console and file 'example.log'

00:00:27 INFO  examples/file_output_example.c:51: Application started
00:00:27 WARN  examples/file_output_example.c:53: This is a warning message
00:00:27 ERROR examples/file_output_example.c:54: An error occurred: File not found

Log level set to WARN - only warnings and above will be logged to file:

00:00:27 WARN  examples/file_output_example.c:64: This warning should appear in file
00:00:27 ERROR examples/file_output_example.c:65: This error should appear in file
00:00:27 INFO  examples/file_output_example.c:75: Application shutting down
00:00:27 DEBUG examples/file_output_example.c:76: Cleanup completed
```

*Note: In your terminal, each log level will be beautifully color-coded! 🎨*

<!--------------------------------------------------------------------------->

<!--------------------------------- QUICK START --------------------------------->

## 🚀 Quick Start

### Basic Usage

```c
#include "lib/loggin.h"

int main(void) {
    // Initialize the logger
    logger_init();
    
    // Log messages
    log_info("Application started");
    log_debug("Debug value: %d", 42);
    log_warn("This is a warning");
    log_error("An error occurred: %s", "File not found");
    
    // Cleanup
    logger_cleanup();
    return 0;
}
```

👉 **Expected Output:**
```
14:30:25 INFO  main.c:42: Application started
14:30:25 WARN  main.c:45: This is a warning
14:30:25 ERROR main.c:48: An error occurred: File not found
```

### Building and Running Examples

```bash
# Build everything
make

# Run examples
make run-basic      # Basic logging functionality
make run-file       # File output example
make run-advanced   # Advanced features demo

# Or run all examples
make run-all

# Clean up
make clean
```

<!--------------------------------------------------------------------------->

<!--------------------------------- API REFERENCE --------------------------------->

## 📚 API Reference

### Initialization

```c
void logger_init(void);           // Initialize logger with defaults
void logger_cleanup(void);        // Cleanup resources
```

### Configuration

```c
void logger_set_level(log_level_t level);        // Set minimum log level
void logger_set_quiet(bool quiet);               // Enable/disable output
void logger_set_colors(bool use_colors);         // Enable/disable colors
void logger_set_show_file_line(bool show);       // Show file:line info
void logger_set_show_function(bool show);        // Show function names
void logger_set_lock(log_lock_fn_t fn, void *data); // Set thread lock function
```

### Output Management

```c
int logger_add_console_output(log_level_t level);                    // Add console output
int logger_add_file_output(FILE *file, log_level_t level);           // Add file output
int logger_add_custom_output(log_output_fn_t fn, void *data, log_level_t level); // Add custom output
```

### Logging Macros

```c
log_trace(...);   // TRACE level
log_debug(...);   // DEBUG level
log_info(...);    // INFO level
log_warn(...);    // WARN level
log_error(...);   // ERROR level
log_fatal(...);   // FATAL level
```

### Utility Functions

```c
const char* logger_level_to_string(log_level_t level);
log_level_t logger_string_to_level(const char *str);
```

<!--------------------------------------------------------------------------->

<!--------------------------------- EXAMPLES --------------------------------->

## 💡 Examples

### File Output

```c
#include "lib/loggin.h"

int main(void) {
    logger_init();
    
    FILE *log_file = fopen("app.log", "w");
    logger_add_file_output(log_file, LOG_LEVEL_INFO);
    
    log_info("This will be written to app.log");
    log_debug("This won't appear (level too low)");
    
    fclose(log_file);
    logger_cleanup();
    return 0;
}
```

👉 **Expected Output in app.log:**
```
2024-01-15 14:30:25 INFO  main.c:42: This will be written to app.log
```

### Custom Output Function

```c
void my_custom_output(log_event_t *event) {
    // Custom formatting logic here
    printf("CUSTOM: %s\n", event->fmt);
}

int main(void) {
    logger_init();
    logger_add_custom_output(my_custom_output, NULL, LOG_LEVEL_TRACE);
    
    log_info("This uses custom formatting");
    logger_cleanup();
    return 0;
}
```

👉 **Expected Output:**
```
CUSTOM: This uses custom formatting
```

### Thread Safety

```c
#include <pthread.h>

static pthread_mutex_t logger_mutex = PTHREAD_MUTEX_INITIALIZER;

void thread_lock(bool lock, void *data) {
    if (lock) {
        pthread_mutex_lock(&logger_mutex);
    } else {
        pthread_mutex_unlock(&logger_mutex);
    }
}

int main(void) {
    logger_init();
    logger_set_lock(thread_lock, NULL);
    
    // Now logging is thread-safe
    log_info("Thread-safe logging");
    
    logger_cleanup();
    return 0;
}
```

<!--------------------------------------------------------------------------->

<!--------------------------------- LOG LEVELS --------------------------------->

## 📊 Log Levels

| Level | Value | Description | Color |
|-------|-------|-------------|-------|
| TRACE | 0     | Detailed trace information | 🔵 Blue |
| DEBUG | 1     | Debug information | 🔷 Cyan |
| INFO  | 2     | General information | 🟢 Green |
| WARN  | 3     | Warning messages | 🟡 Yellow |
| ERROR | 4     | Error messages | 🔴 Red |
| FATAL | 5     | Fatal error messages | 🟣 Magenta |

<!--------------------------------------------------------------------------->

<!--------------------------------- OUTPUT FORMAT --------------------------------->

## 📝 Output Format

### Console Output (with colors)
```
14:30:25 INFO  main.c:42: Application started
14:30:25 WARN  main.c:45: This is a warning
14:30:25 ERROR main.c:48: An error occurred
```

### File Output
```
2024-01-15 14:30:25 INFO  main.c:42: Application started
2024-01-15 14:30:25 WARN  main.c:45: This is a warning
2024-01-15 14:30:25 ERROR main.c:48: An error occurred
```

<!--------------------------------------------------------------------------->

<!--------------------------------- INSTALLATION --------------------------------->

## 🔧 Installation

### Local Build
```bash
make
```

### System Installation
```bash
make install  # Requires sudo
```

### Uninstall
```bash
make uninstall  # Requires sudo
```

<!--------------------------------------------------------------------------->

<!--------------------------------- PROJECT STRUCTURE --------------------------------->

## 🏗️ Project Structure

```
lib/                          # Root directory for all library code
├── logger.h                  # Main entry point for the logger module
├── logger/                   # Logger-specific directory
│   ├── logger.c              # Core logger implementation
│   ├── logger.test.c         # Logger tests
│   └── utils/                # Specialized utilities
│       ├── console/          # Console output utilities
│       ├── file/             # File output utilities
│       └── threading/        # Threading utilities
```

<!--------------------------------------------------------------------------->

<!--------------------------------- FOOTER --------------------------------->

<div align="center">

---

**Developed with ❤️ by [Sebastian Rivera](https://github.com/ItsCbass)**

*Following the Maysara Code Style: "Code as Art, Structure as Poetry"*

[![GitHub](https://img.shields.io/badge/GitHub-ItsCbass-black?style=for-the-badge&logo=github)](https://github.com/ItsCbass/loggin.c)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

---

### 📚 Code Style Reference

This project follows the **Maysara Code Style** guidelines for beautiful, organized, and maintainable code.

**Maysara Code Style Repository:** [https://github.com/maysara-elshewehy/MCS](https://github.com/maysara-elshewehy/MCS)

*"Code as Art, Structure as Poetry"* - A philosophy that treats code not just as a functional tool, but as an artistic expression that prioritizes both aesthetic beauty and technical excellence.

</div>

<!--------------------------------------------------------------------------->
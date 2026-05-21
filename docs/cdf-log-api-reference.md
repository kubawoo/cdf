# CDF Log Module API Reference

> Module: `cdf-log`, version `0.5.0`. Headers: `logger.h`, `loggerfactory.h` (umbrella `log.h` includes `logger.h`).

---

## Overview

The CDF logging module provides a simple, extensible logging framework with:
- **Log levels**: OFF, ERROR, WARN, INFO, DEBUG, Trace
- **Logger instances**: Named loggers that can be configured with a minimum log level
- **Logger factory**: For retrieving or creating named loggers (singleton-like behavior per name)
- **Formatted output**: Timestamp, log level, source location (file:line), and message

All loggers output to `stdout` in the format:
```
YYYY-MM-DD HH:MM:SS LEVEL [file:line] message
```

---

## Macros and Enums

### `log_msg(x)`
Macro that expands to `x, __FILE__, __LINE__` for automatic source location capture.
```c
call(logger, log, LOG_LEVEL_INFO, log_msg("User login successful"));
// expands to: call(logger, log, LOG_LEVEL_INFO, "User login successful", __FILE__, __LINE__);
```

### `LogLevel` enum
Defined in `logger.h`:

| Value | Description |
|---|---|
| `LOG_LEVEL_OFF` | Disable logging (no output) |
| `LOG_LEVEL_ERROR` | Error conditions |
| `LOG_LEVEL_WARN` | Warning conditions |
| `LOG_LEVEL_INFO` | Informational messages |
| `LOG_LEVEL_DEBUG` | Debug-level messages |
| `LOG_LEVEL_TRACE` | Trace-level messages (most verbose) |

**Note**: Levels are compared numerically (`level <= current_level`). Lower values are more severe:
- OFF = 0
- ERROR = 1
- WARN = 2
- INFO = 3
- DEBUG = 4
- TRACE = 5

Thus, setting level to `LOG_LEVEL_WARN` will output WARN, ERROR, and OFF (though OFF outputs nothing).

---

## Class Hierarchy

Both classes inherit directly from `Object`:
```
Object
  +-- Logger
  +-- LoggerFactory
```

---

## Logger Class

**File:** `logger.h` / `logger.c`

A named logger instance that logs messages at or above its configured level.

### Fields

| Field | Type | Description |
|---|---|---|
| `name` | `String *` | Logger name (REFCINC'd on assignment) |
| `current_level` | `LogLevel` | Minimum level to log (messages at or above this level are output) |
| `log` | `void (*)(ObjectPtr, LogLevel, String *, const char *, int)` | Function pointer to the logging implementation (set to `_log` in constructor) |

### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `Logger_new2` | `Logger *(Logger *this, String *name, LogLevel level)` | Creates a logger with the given name and minimum log level. The name string is REFCINC'd and stored. |

### Destructor

| Method | Signature | Description |
|---|---|---|
| `Logger_delete` | `void (ObjectPtr self)` | Decrements the reference count on the logger's name and calls the parent destructor. |

### Methods

| Method | Signature | Description |
|---|---|---|
| `log` | `void (ObjectPtr self, LogLevel level, String *msg, const char *filename, int line)` | Logs a message if `level >= self->current_level` (note: level comparison uses `<=` because lower numbers are more severe). Formats output as:<br>`YYYY-MM-DD HH:MM:SS LEVEL [filename:line] message`<br>where timestamp is obtained via `DateTime.format("%F %T")`. |

> **Note**: The `log` method is typically invoked via the `call` macro or direct pointer access. The `log_msg` macro simplifies passing filename and line number.

---

## LoggerFactory Class

**File:** `loggerfactory.h` / `loggerfactory.c`

Factory for retrieving or creating named loggers. Each unique name returns the same logger instance (similar to a singleton per name).

### Fields

| Field | Type | Description |
|---|---|---|
| `_loggers` | `Map *` | Map from logger name (`String*`) to logger instance (`Logger*`). Keys and values are REFCINC'd when stored. |

### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `LoggerFactory_new` | `LoggerFactory *(LoggerFactory *this)` | Creates a new factory with an internal `Map` to store loggers. |

### Destructor

| Method | Signature | Description |
|---|---|---|
| `LoggerFactory_delete` | `void (ObjectPtr self)` | Decrements the reference count on the internal loggers map and calls the parent destructor. |

### Methods

| Method | Signature | Description |
|---|---|---|
| `get_logger` | `Logger *(ObjectPtr self, String *name)` | Retrieves the logger for the given name. If no logger exists for that name, creates a new one with name and default level `LOG_LEVEL_INFO`, stores it in the internal map, and returns it. The returned logger has an incremented reference count (caller must `REFCDEC` when done). |

---

## Usage Example

```c
#include <log.h>  // includes logger.h

int main(void) {
    // Get or create a logger named "app"
    LoggerFactory *factory = new(LoggerFactory);
    Logger *app_logger = call(factory, get_logger, new(String, "app"));
    // Note: factory and the string "app" must be cleaned up later

    // Log messages at different levels
    call(app_logger, log, LOG_LEVEL_INFO, log_msg("Application started"));
    call(app_logger, log, LOG_LEVEL_DEBUG, log_msg("Debug details: x=42"));
    call(app_logger, log, LOG_LEVEL_WARN, log_msg("High memory usage detected"));

    // Create another logger for database access
    Logger *db_logger = call(factory, get_logger, new(String, "db"));
    call(db_logger, log, LOG_LEVEL_ERROR, log_msg("Connection failed"));

    // Cleanup: decrement references in reverse order of creation
    REFCDEC(db_logger);
    REFCDEC(app_logger);
    REFCDEC(factory);
    // Note: The String objects passed to get_logger are owned by the factory
    // after being stored in the map, so we do NOT REFCDEC them here.
    // However, the String objects we created with new(String, ...) for the
    // name arguments are NOT stored by the factory (the factory stores the
    // name from the map lookup/creation, which is a different string).
    // Actually, looking at the factory: when creating a new logger, it uses
    // the provided name string and REFCINC's it. So we MUST REFCDEC the
    // strings we created with new(String, ...) after passing to get_logger.
    // But wait: the factory's internal map stores the name string and
    // REFCINC's it. The string we passed in is the same pointer, so we
    // still own a reference and must REFCDEC it.

    // Correction: We must REFCDEC the String objects we created for the names.
    // Let's redo the cleanup properly:

    // Actually, let's rewrite the example to be clearer:

    String *app_name = new(String, "app");
    String *db_name  = new(String, "db");

    LoggerFactory *factory = new(LoggerFactory);
    Logger *app_logger = call(factory, get_logger, app_name);
    Logger *db_logger  = call(factory, get_logger, db_name);

    // Now we can REFCDEC the name strings because the factory has its own references
    REFCDEC(app_name);
    REFCDEC(db_name);

    // Use loggers...
    call(app_logger, log, LOG_LEVEL_INFO, log_msg("Hello from app"));
    call(db_logger,  log, LOG_LEVEL_ERROR, log_msg("DB error"));

    // Cleanup loggers and factory
    REFCDEC(app_logger);
    REFCDEC(db_logger);
    REFCDEC(factory);

    return 0;
}
```

> **Important**: The `log_msg` macro must be used with the `log` method to automatically capture `__FILE__` and `__LINE__`.  
> Example: `call(logger, log, LOG_LEVEL_INFO, log_msg("Message"))`

---

## Notes

- **Thread safety**: The current implementation is **not thread-safe**. Concurrent access to the same logger or factory from multiple threads may cause race conditions.
- **Logger configuration**: Loggers are created with `LOG_LEVEL_INFO` by default via the factory. To change a logger's level, modify its `current_level` field directly (not recommended) or recreate it via the factory with a new level.
- **Output destination**: All loggers write to `stdout`. To change output destination, modify the `_log` function in `logger.c` or extend the `Logger` class.
- **Reference counting**: 
  - The factory retains a reference to each logger name and logger instance.
  - Loggers retain a reference to their name string.
  - Callers must `REFCDEC` loggers obtained from the factory and any `String` objects they create for use with `get_logger` (after the factory has stored its own reference).

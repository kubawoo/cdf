# CDF API Documentation

This directory contains API reference documentation for the CDF (C Development Framework) modules.

## Available API References

- [Core CDF Module](cdf-api-reference.md) - Fundamental OOP classes: Object, String, List, Map, Array, primitives, streams, etc.
- [CDF Logging Module](cdf-log-api-reference.md) - Logging framework with levels, loggers, and factory
- [CDF JSON Module](cdf-json-api-reference.md) - JSON parsing and serialization

## Module Overview

CDF is a monorepo of object-oriented C libraries that implements OOP concepts in C using GCC-specific macros and vararg extensions.

### Core Modules

1. **cdf** - Core OOP framework (Object, String, List, Map, Array, etc.)
2. **cdf-json** - JSON parser and serializer
3. **cdf-http** - HTTP client and multithreaded server
4. **cdf-log** - Logging framework
5. **cdf-db** - Generic database abstraction
6. **cdf-db-sqlite** - SQLite backend for cdf-db
7. **cdf-db-entity** - Entity manager (ORM-like)
8. **cdftk** - CLI toolkit for project management
9. **py2cdf** - Python-like syntax to CDF source translator

### Building and Installation

See [project_overview.md](project_overview.md) for detailed build instructions.

Each module declares dependencies in its `cdfmodule.json` file. Build order matters:
```sh
make -C test-framework && make -C test-framework install
make -C cdf && make -C cdf install
make -C cdf-json && make -C cdf-json install
# ... etc for other modules
```

## Conventions

- **Object Creation**: `new(ClassName, ...)` - supports up to 10 constructor arguments
- **Method Calls**: `call(obj, method, ...)` - virtual method dispatch
- **Memory Management**: Reference counting with `REFCINC()` and `REFCDEC()`
- **Inheritance**: `inherits(BaseClass)` in struct definitions
- **Constructor Chaining**: `super(BaseClass, ThisClass)`
- **Method Overriding**: `override(BaseClass, base_method, fn)`

## Testing

Test files must be named `tc_*.c` and use the test framework API. Test output goes to `output.log`, not stdout.

See [Tutorial.md](Tutorial.md) for examples and getting started guides.
# CDF — C Development Framework

## Overview

CDF (C Development Framework) is a monorepo of object-oriented C libraries that implements OOP concepts in C using GCC-specific macros and vararg extensions. All modules compile into a single static library (`libcdf.a`).

## Project Structure

```
cmake/            # CMake modules (Options.cmake, CompilerSettings.cmake)
src/              # Library sources
  core/           #   Core OOP: Object, String, List, Map, Array, IO streams
  json/           #   JSON parser/serializer
  http/           #   HTTP client + multithreaded server
  log/            #   Logging framework
  db/             #   Generic database abstraction
  db-sqlite/      #   SQLite backend
  db-entity/      #   Entity manager (ORM-like)
tests/            # CTest test suites (tc_*.c files)
  core/
  json/
  http/
  log/
  db/
  db-sqlite/
  db-entity/
examples/         # helloworld, shapes, wwwserver, todo-list-api
docs/             # Documentation
```

## Key Features

### Object-Oriented Programming in C

CDF implements OOP concepts through a set of macros that provide:
- Class definition and instantiation
- Inheritance
- Polymorphism (virtual method calls)
- Reference counting for automatic memory management
- Constructor/destructor patterns

### Memory Management

The framework uses reference counting for automatic memory management:
- Objects are allocated with `new()` constructor macros
- Reference counts are incremented with `REFCINC()` and decremented with `REFCDEC()`
- When reference count reaches zero, objects are automatically deleted
- Manual deletion is also possible with `delete()`

### Custom Macros

CDF provides a comprehensive set of macros for OOP programming in C:

#### Object Creation
- `new(Class, ...)` — Creates a new instance of Class with variable arguments
  - Supports up to 11 constructor arguments through macro overloading
  - Example: `String *s = new(String, "hello");`

#### Method Invocation
- `call(obj, method, ...)` — Virtual method call (uses vtable)
  - Supports up to 11 method arguments
  - Example: `call(s, length);` or `call(map, put, key, value);`
- `obj->method(obj, ...)` — Direct method call equivalent to call()

#### Inheritance and Polymorphism
- `inherits(BaseClass)` — Used in class definitions to inherit from BaseClass
- `super(BaseClass, ThisClass)` — Constructor chaining to parent class
- `override(BaseClass, base_method, fn)` — Override a base class method
- `super_method(BaseClass, method)` — Access to overridden base class method

#### Memory Management
- `delete(obj)` — Force deletion of an object
- `REFCINC(obj)` — Increment reference count
- `REFCDEC(obj)` — Decrement reference count and auto-delete if ≤ 0
- `REFCTMP(obj)` — Temporary reference handling

#### Type Information
- `type_equal(obj, "TypeName")` — Check if object is of specific type
- `types_equal(obj1, obj2)` — Check if two objects are of same type

#### Utility Macros
- `make_this(Class, this_ptr)` — Cast void* to Class* and assign to `this`
- `ObjectPtr` — Typedef for void* used as generic object pointer

## Building and Installation

### Requirements
- CMake >= 3.20
- gcc >= 14 (`-std=c23` support)
- libsqlite3-dev (for database modules)

### Build Commands

| Command | What it does |
|---|---|
| `cmake -S . -B build` | Configure (pass `-D` flags to disable modules) |
| `cmake --build build` | Compile `libcdf.a` + examples |
| `ctest --test-dir build` | Run all tests |
| `cmake --install build` | Install `libcdf.a` + headers to prefix (default `/usr/local`) |

### Disabling Modules

All non-core modules are enabled by default. Disable them at configure time:

```sh
cmake -S . -B build \
    -DCDF_BUILD_JSON=OFF \
    -DCDF_BUILD_HTTP=OFF \
    -DCDF_BUILD_LOG=OFF \
    -DCDF_BUILD_DB=OFF
```

The database modules (`cdf-db`, `cdf-db-sqlite`, `cdf-db-entity`) share a single `CDF_BUILD_DB` switch. Set it to `OFF` to disable all three.

### Installation

Installs to `/usr/local` by default:

```sh
cmake --install build
```

Custom prefix:

```sh
cmake --install build --prefix /opt/cdf
```

Output layout:
```
{prefix}/
  lib/libcdf.a
  include/cdf/core/*.h
  include/cdf/json/*.h
  include/cdf/http/*.h
  include/cdf/log/*.h
  include/cdf/db/*.h
  include/cdf/db-sqlite/*.h
  include/cdf/db-entity/*.h
```

## Testing

### Test Requirements
- Test source files must be named `tc_*.c`
- Each test is a standalone executable using `assert()` from `<assert.h>`
- Tests are registered with CTest and output to stdout

### Running Tests

```sh
# All tests
ctest --test-dir build

# Specific test (regex match)
ctest --test-dir build -R tc_list

# Verbose output
ctest --test-dir build -V
```

### Writing a Test

```c
#include <assert.h>
#include <cdf.h>

static void test_string_creation(void) {
    String *s = new(String, "hello");
    assert(s != NULL);
    REFCDEC(s);
}

int main(void) {
    test_string_creation();
    return 0;
}
```

## Compiler Requirements

- Compiler: `gcc -std=c23` (uses GCC-specific varargs in `src/core/ooc_macros.h`)
- Not portable to clang due to GCC-specific extensions

## Important Gotchas

1. **Test output**: Goes to stdout, not a log file. Check test output on failure.
2. **Module disable flags**: Database modules share one flag (`CDF_BUILD_DB`). Individual database modules cannot be toggled independently.
3. **Examples controlled by `CDF_BUILD_EXAMPLES`**: Disable with `-DCDF_BUILD_EXAMPLES=OFF` to skip building samples.
4. **Memory management**: Proper use of REF macros is essential to prevent leaks or premature deletion.

## Examples

See the `examples/` directory for complete working examples:
- helloworld — Basic introduction to CDF
- shapes — Object-oriented geometry example
- wwwserver — Multithreaded HTTP server implementation
- todo-list-api — REST API with SQLite persistence (requires DB + HTTP + Log)

## Conclusion

CDF provides a robust foundation for object-oriented programming in C, combining the power and performance of C with the abstraction and organization principles of OOP. Through its macro system, reference counting memory management, and modular design, it enables developers to write maintainable, efficient C code with familiar OOP patterns.

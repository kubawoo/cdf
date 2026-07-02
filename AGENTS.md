# CDF - C Development Framework

Monorepo of object-oriented C libraries (OOP via macros, gcc-specific vararg extensions), built with CMake into a single shared library `libcdf.so` (with optional static library `libcdf.a`).

## Project layout

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
```

## Build & install

Requires CMake >= 3.20, gcc >= 14 (`-std=c23`), libsqlite3-dev.

```sh
cmake -S . -B build        # configure (pass -D flags to disable modules)
cmake --build build        # compile libcdf.so + examples
ctest --test-dir build     # run tests (stdout, no output.log)
cmake --install build      # install libcdf.so + headers; add -DCDF_BUILD_STATIC=ON for static
<<<<<<< HEAD

CMake package config is generated at `lib/cmake/cdf/cdfConfig.cmake`, so downstream
projects can use `find_package(cdf)` and link with `target_link_libraries(... cdf::cdf)`.
=======
>>>>>>> main
```

## Module disable flags

All non-core modules enabled by default. Toggle at configure time:

| Flag | Controls |
|---|---|
| `CDF_BUILD_JSON` | `cdf-json` |
| `CDF_BUILD_HTTP` | `cdf-http` |
| `CDF_BUILD_LOG` | `cdf-log` |
| `CDF_BUILD_DB` | `cdf-db`, `cdf-db-sqlite`, `cdf-db-entity` (all three) |
| `CDF_BUILD_STATIC` | Static library (`libcdf.a`) in addition to default shared |
| `CDF_BUILD_EXAMPLES` | All example programs |

A top-level convenience header `<cdf/cdf.h>` is generated at build time and
includes all enabled module headers.

## Testing

- Test files named `tc_*.c`, each is a standalone executable using `assert()`.
- Registered with CTest; output goes to stdout (no `output.log`).
- Run all: `ctest --test-dir build`
- Run one: `ctest --test-dir build -R tc_list`
- `WORKING_DIRECTORY` set to each test's source directory.

## Compiler & flags

- Compiler: `gcc -std=c23` (uses GCC-specific varargs in `src/core/ooc_macros.h` - not portable to clang).
- Flags: `-Wall -fPIC` (set in `cmake/CompilerSettings.cmake`).

## OOP conventions (`src/core/ooc_macros.h`)

```c
String *s = new(String, "hello");     // constructor
call(obj, method, arg);               // virtual method call
obj->method(obj, arg);                // direct call equivalent
REFCDEC(obj);                         // decref + auto-delete when refcount = 0
delete(obj);                          // force-delete
super(BaseClass, ThisClass);          // constructor chaining
override(BaseClass, base_method, fn); // virtual override
```

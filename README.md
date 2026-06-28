# CDF — C Development Framework

Object-oriented libraries for C, built with macros and gcc-specific vararg extensions.

## Modules

| Module | Description |
|---|---|
| `cdf` | Core: Object, String, List, Map, Array, IO streams |
| `cdf-json` | JSON parser/serializer |
| `cdf-http` | HTTP client and multithreaded server |
| `cdf-log` | Logging framework |
| `cdf-db` | Generic database abstraction |
| `cdf-db-sqlite` | SQLite backend for `cdf-db` |
| `cdf-db-entity` | Entity manager (ORM-like) |
| `examples/` | helloworld, shapes, wwwserver, todo-list-api |

See [`docs/Tutorial.md`](docs/Tutorial.md) for a getting-started guide.

## Build & install

Requires CMake >= 3.20, gcc >= 14 (`-std=c23`), and `libsqlite3-dev`.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
cmake --install build --prefix /usr/local
```

| Command | What it does |
|---|---|
| `cmake -S . -B build` | Configure (pass `-DCDF_BUILD_JSON=OFF` etc. to disable modules) |
| `cmake --build build` | Compile `libcdf.a` + examples |
| `ctest --test-dir build` | Run all tests (stdout, no output.log) |
| `cmake --install build` | Install `libcdf.a` + headers to `prefix` |

All modules are built into a single static library (`libcdf.a`). Disable individual modules at configure time:

```sh
cmake -S . -B build \
    -DCDF_BUILD_JSON=OFF \
    -DCDF_BUILD_HTTP=OFF \
    -DCDF_BUILD_LOG=OFF \
    -DCDF_BUILD_DB=OFF
```

Database modules (`cdf-db`, `cdf-db-sqlite`, `cdf-db-entity`) share the `CDF_BUILD_DB` switch — set it to `OFF` to disable all three.

## OOP quick reference

```c
String *s = new(String, "hello");     // constructor
call(obj, method, arg);               // virtual method call
REFCDEC(obj);                         // decref, auto-delete at 0
delete(obj);                          // force-delete
super(BaseClass, ThisClass);          // constructor chaining
override(BaseClass, base_method, fn); // virtual override
```

## License

MIT

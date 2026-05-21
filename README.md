# CDF — C Development Framework

Object-oriented libraries for C, built with macros and gcc-specific vararg extensions.


## Warning: AI inside

This repository was partially built by an AI. If the code compiles, consider it a miracle. If it doesn't, the AI probably `replaceAll`'d something it shouldn't have. You have been warned.


## Modules

| Module | Description |
|---|---|
| `test-framework` | Custom test runner (test cases compiled as `.so`, run via `testrunner`) |
| `cdf` | Core libs: Object, String, List, Map, Array, etc. (in src/) and `io/` (streams) |
| `cdf-json` | JSON parser/serializer |
| `cdf-http` | HTTP client and multithreaded server |
| `cdf-log` | Logging framework |
| `cdf-db` | Generic database abstraction |
| `cdf-db-sqlite` | SQLite backend for `cdf-db` |
| `cdf-db-entity` | Entity manager (ORM-like) |
| `cdftk` | CLI toolkit (`create-new-project`, `build`, `test` commands) |
| `examples/` | Sample applications: helloworld, shapes, shop, wwwserver |

See [`cdf/doc/Tutorial.md`](cdf/doc/Tutorial.md) for a getting-started guide with code samples.

## Build & install

Requires `gcc` (>= 4.x), `make`, and `jq`. The `cdf-db-sqlite` module additionally needs `libsqlite3-dev`. Tested on Linux.

Each module declares its dependencies in `cdfmodule.json`. Modules install to `$CDF_HOME/{group}/{name}/{version}` (default `~/.cdf`).

Build and install all:

```sh
make install
```

Available targets per module:

| Command | What it does |
|---|---|
| `make` / `make build` | Compile sources |
| `make test` | Build + run tests |
| `make valgrind` | Build + run tests under valgrind |
| `make clean` | Remove build artifacts |
| `make install` | Copy `dist/` to `$CDF_HOME` |

The root `Makefile` builds all modules sequentially but does not run `install`, so downstream modules cannot link until each dependency is installed first.

### Examples

Build examples after modules are installed:

```sh
make -C examples
```

Individual examples:

```sh
make -C examples/helloworld run     # Hello CDF!
make -C examples/shapes             # Object-oriented shapes demo
make -C examples/shop               # Shop server example
make -C examples/wwwserver run      # HTTP server demo
```

## cdfmodule.json

Each module has a `cdfmodule.json` that declares its identity and dependencies:

```json
{
    "group": "cdf",
    "name": "cdf-json",
    "version": "0.4.0",
    "description": "JSON handling lib",
    "dependencies": [
        {
            "group": "cdf",
            "name": "cdf",
            "version": "0.4.0"
        },
        {
            "group": "cdf",
            "name": "test-framework",
            "version": "0.4.0",
            "scope": "test"
        }
    ]
}
```

- `name` is the module's directory name.
- `group` is always `cdf` in this repository.
- `version` is `major.minor.patch` or `major.minor.patch-SNAPSHOT` (the latter enables debug builds).
- `dependencies` list required modules. Dependencies with `"scope": "test"` are only needed at test time.
- Versions in dependency entries must match the depended module's own version exactly. Makefiles resolve install paths from these entries, reading `$CDF_HOME` (default `~/.cdf`).

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

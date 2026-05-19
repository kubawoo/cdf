# CDF — C Development Framework

Monorepo of object-oriented C libraries (OOP via macros, gcc-specific vararg extensions).

## Project layout

```
test-framework/   # custom test runner (tc_*.c → .so files, run via testrunner)
cdf/              # core libs: ooc/ (Object, String, List, Map, Array, etc.), io/ (streams)
cdf-json/         # JSON parser/serializer
cdf-http/         # HTTP client + multithreaded server
cdf-log/          # logging framework
cdf-db/           # generic database abstraction
cdf-db-sqlite/    # sqlite backend for cdf-db
cdf-db-entity/    # entity manager (ORM-like)
cdftk/            # CLI toolkit (depends on cdf + cdf-json). Supports commands: `create-new-project <name>`, `build`, `test`
py2cdf/           # Python-like syntax to CDF source translator (.cs files → .csc/.csh)
examples/         # helloworld, shapes, shop, wwwserver
```

## Dependencies & install

Modules declare deps in `cdfmodule.json`. Built modules install to `$CDF_HOME/{group}/{name}/{version}` (default `~/.cdf`). Version strings are mangled by the Makefiles (e.g., `0.3.0` → path `0.3`, `0.2.4-SNAPSHOT` → path `0.2-SNAPSHOT`).

**Build + install order matters** — a module's tests and downstream consumers expect its deps to be installed under `$CDF_HOME`:
```sh
make -C test-framework && make -C test-framework install
make -C cdf && make -C cdf install
make -C cdf-json && make -C cdf-json install
# ... etc for cdf-http, cdf-log, cdf-db, cdf-db-sqlite, cdf-db-entity
```

## Commands

| Command | What it does |
|---|---|
| `make` or `make build` | Compile sources within a module |
| `make test` | Build + run tests (output in `output.log`) |
| `make valgrind` | Build + run tests under valgrind |
| `make clean` | Remove build artifacts |
| `make install` | Copy `dist/` to `$CDF_HOME` |
| `make dist` | Build + assemble headers + shared libs into `dist/` |

Root `Makefile` builds projects sequentially (order: `test-framework cdf cdf-json cdf-http cdf-log cdf-db cdf-db-sqlite cdf-db-entity`), but still needs `make install` for each dep before downstream modules can link.

## Testing

- Test source files **must** be named `tc_*.c` and use the `test_framework.h` API.
- Each `tc_*.c` compiles to a **shared library** (`tc_*.so`) via `-shared -fPIC`.
- Tests are run with `testrunner`:
  ```sh
  LD_LIBRARY_PATH=".:../src:$CDF_HOME/cdf/<dep>/<ver>/lib" $TEST_RUNNER -l<lib> tc_*.so
  ```
  The `-l` flags are passed to `dlopen` for runtime linking. Test output goes to `output.log`.
- Test file structure:
  ```c
  void testcase(TEST_CASE_ARGUMENTS) { ASSERT_EQUAL(1, 1); }
  TEST_CASES_BEGIN
      TEST_CASE(testcase);
  TEST_CASES_END
  ```
- `CDF_HOME` defaults to `$HOME/.cdf`. Makefiles resolve `TEST_FRAMEWORK_PATH` et al. from `cdfmodule.json`.

## Compiler & flags

- Compiler: `gcc -std=c17` (uses GCC-specific varargs in `ooc_macros.h` — not portable to clang).
- Debug builds get `-g`; release builds get `-Os` (controlled by `SNAPSHOT` in version string).

## OOP conventions (src/ooc_macros.h)

```c
String *s = new(String, "hello");     // constructor
call(obj, method, arg);               // virtual method call
obj->method(obj, arg);                // direct call equivalent
REFCDEC(obj);                         // decref + auto-delete when refcount ≤ 0
delete(obj);                          // force-delete
super(BaseClass, ThisClass);          // constructor chaining
override(BaseClass, base_method, fn); // virtual override
```


## Key gotchas

- **Test output goes to `output.log`**, not stdout. Check that file on failure.
- **`-l` flags to `testrunner`** are critical — omitting a needed lib causes dlopen failures at test time.
- **`cdfmodule.json` version format** with `-SNAPSHOT` triggers debug builds. The version mangling in `Makefile.inc` produces install paths like `0.3` (from `0.3.0`) or `0.2-SNAPSHOT` (from `0.2.4-SNAPSHOT`).
- **`bootstrap.sh`** in `cdftk/` automates dep cloning + build + install from GitLab tags, but is not needed for monorepo workflow.
- **Pre-existing build artifacts (`.o`, `.so`, executables) are committed** — cleaning before rebuild is recommended if source changes.

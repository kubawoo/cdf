# CDF — C Development Framework

## Overview

CDF (C Development Framework) is a monorepo of object-oriented C libraries that implements OOP concepts in C using GCC-specific macros and vararg extensions. The framework provides a foundation for building C applications with object-oriented design patterns while maintaining compatibility with standard C.

## Project Structure

```
test-framework/   # Custom test runner (tc_*.c → .so files, run via testrunner)
cdf/              # Core libs: ooc/ (Object, String, List, Map, Array, etc.), io/ (streams)
cdf-json/         # JSON parser/serializer
cdf-http/         # HTTP client + multithreaded server
cdf-log/          # Logging framework
cdf-db/           # Generic database abstraction
cdf-db-sqlite/    # SQLite backend for cdf-db
cdf-db-entity/    # Entity manager (ORM-like)
cdftk/            # CLI toolkit (depends on cdf + cdf-json). Supports commands: `create-new-project <name>`, `build`, `test`
py2cdf/           # Python-like syntax to CDF source translator (.cs files → .csc/.csh)
examples/         # helloworld, shapes, shop, wwwserver
docs/             # Documentation (this directory)
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
- `new(Class, ...)` - Creates a new instance of Class with variable arguments
  - Supports up to 11 constructor arguments through macro overloading
  - Example: `String *s = new(String, "hello");`

#### Method Invocation
- `call(obj, method, ...)` - Virtual method call (uses vtable)
  - Supports up to 11 method arguments
  - Example: `call(s, length);` or `call(map, put, key, value);`
- `obj->method(obj, ...)` - Direct method call equivalent to call()

#### Inheritance and Polymorphism
- `inherits(BaseClass)` - Used in class definitions to inherit from BaseClass
- `super(BaseClass, ThisClass)` - Constructor chaining to parent class
- `override(BaseClass, base_method, fn)` - Override a base class method
- `super_method(BaseClass, method)` - Access to overridden base class method

#### Memory Management
- `delete(obj)` - Force deletion of an object
- `REFCINC(obj)` - Increment reference count
- `REFCDEC(obj)` - Decrement reference count and auto-delete if ≤ 0
- `REFCTMP(obj)` - Temporary reference handling

#### Type Information
- `type_equal(obj, "TypeName")` - Check if object is of specific type
- `types_equal(obj1, obj2)` - Check if two objects are of same type

#### Utility Macros
- `make_this(Class, this_ptr)` - Cast void* to Class* and assign to `this`
- `ObjectPtr` - Typedef for void* used as generic object pointer

## Building and Installation

### Build Commands
- `make` or `make build` - Compile sources within a module
- `make test` - Build + run tests (output in output.log)
- `make valgrind` - Build + run tests under valgrind
- `make clean` - Remove build artifacts
- `make install` - Copy dist/ to $CDF_HOME
- `make dist` - Build + assemble headers + shared libs into dist/

### Installation Path
Built modules install to `$CDF_HOME/{group}/{name}/{version}` (default `~/.cdf`).
Version strings are mangled by the Makefiles:
- `0.3.0` → path `0.3`
- `0.2.4-SNAPSHOT` → path `0.2-SNAPSHOT`

### Build Order
Dependencies must be installed before dependent modules can link:
```sh
make -C test-framework && make -C test-framework install
make -C cdf && make -C cdf install
make -C cdf-json && make -C cdf-json install
# ... etc for cdf-http, cdf-log, cdf-db, cdf-db-sqlite, cdf-db-entity
```

Root Makefile builds projects sequentially but still requires `make install` for each dependency.

## Testing

### Test Requirements
- Test source files must be named `tc_*.c`
- Must use the `test_framework.h` API
- Each test file compiles to a shared library (`tc_*.so`) via `-shared -fPIC`

### Test Structure
```c
void testcase(TEST_CASE_ARGUMENTS) {
    ASSERT_EQUAL(1, 1);
}
TEST_CASES_BEGIN
    TEST_CASE(testcase);
TEST_CASES_END
```

### Running Tests
Tests are run with testrunner:
```sh
LD_LIBRARY_PATH=".:../src:$CDF_HOME/cdf/<dep>/<ver>/lib" $TEST_RUNNER -l<lib> tc_*.so
```
The `-l` flags are passed to dlopen for runtime linking.
**Important**: Test output goes to `output.log`, not stdout.

## Compiler Requirements

- Compiler: `gcc -std=c17` (uses GCC-specific varargs in ooc_macros.h)
- Not portable to clang due to GCC-specific extensions
- Debug builds get `-g`; release builds get `-Os` (controlled by SNAPSHOT in version string)

## Important Gotchas

1. **Test output location**: Check `output.log` on failure, not stdout
2. **Test linker flags**: `-l` flags to testrunner are critical — omitting a needed lib causes dlopen failures
3. **Version string format**: `-SNAPSHOT` triggers debug builds; version mangling affects install paths
4. **Build artifacts**: Pre-existing build artifacts (.o, .so, executables) are committed — cleaning before rebuild is recommended if source changes
5. **Memory management**: Proper use of REF macros is essential to prevent leaks or premature deletion

## Py2CDF Translator

The py2cdf module provides a Python-like syntax translator that converts `.cs` files (Python-like DSL) into C source code using CDF's OOP macro system.

### Usage
```sh
python3 py2cdf.py -s -I <include-dirs> -i file.cs      # Generate .csc source
python3 py2cdf.py -h -i file.cs                          # Generate .csh header
python3 py2cdf.py -s -h -o outdir -i file.cs             # Both, to output dir
```

### Input Language
A restricted Python subset that maps directly to OOC constructs:
```python
import ooc.h
import io.h

def hello():
    s = "hello"
    i = 42
    b = True
    x = None
    l = ['a', 1, True, x]
    c = Console()
    c.print_object(s)
    call_return = obj.method(arg)
```

### Generated Code Conventions
- String literals in constructor args are auto-wrapped: `new(String, "...")`
- Variables declared at function top; cleaned up with `REFCDEC` at function end
- Temp variables prefixed `_tmp_N` for intermediate values

## Examples

See the `examples/` directory for complete working examples:
- helloworld - Basic introduction to CDF
- shapes - Object-oriented geometry example
- shop - Retail simulation demonstrating complex object interactions
- wwwserver - Multithreaded HTTP server implementation

## Module Dependencies

Each module declares its dependencies in a `cdfmodule.json` file. The build system ensures dependencies are installed in the correct order under `$CDF_HOME`.

## Conclusion

CDF provides a robust foundation for object-oriented programming in C, combining the power and performance of C with the abstraction and organization principles of OOP. Through its macro system, reference counting memory management, and modular design, it enables developers to write maintainable, efficient C code with familiar OOP patterns.
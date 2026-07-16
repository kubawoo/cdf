# CDF - C Development Framework

Object-oriented libraries for the C programming language.

## Key Features

### Object-Oriented Programming in C

CDF implements OOP concepts through a set of macros that provide:

    Class definition and instantiation
    Inheritance
    Polymorphism (virtual method calls)
    Reference counting for automatic memory management
    Constructor/destructor patterns

### Memory Management

The framework uses reference counting for automatic memory management:

    Objects are allocated with new() constructor macros
    Reference counts are incremented with REFCINC() and decremented with REFCDEC()
    When reference count reaches zero, objects are automatically deleted
    
### Custom Macros

CDF provides a comprehensive set of macros for OOP programming in C:
Object Creation

    new(Class, ...) - Creates a new instance of Class with variable arguments
        Supports up to 11 constructor arguments through macro overloading
        Example: String *s = new(String, "hello");

Method Invocation

    call(obj, method, ...) - Virtual method call (uses vtable)
        Supports up to 11 method arguments
        Example: call(s, length); or call(map, put, key, value);
    obj->method(obj, ...) - Direct method call equivalent to call()

Inheritance and Polymorphism

    inherits(BaseClass) - Used in class definitions to inherit from BaseClass
    super(BaseClass, ThisClass) - Constructor chaining to parent class
    override(BaseClass, base_method, fn) - Override a base class method
    super_method(BaseClass, method) - Access to overridden base class method

Memory Management

    REFCINC(obj) - Increment reference count
    REFCDEC(obj) - Decrement reference count and auto-delete if =0

Type Information

    type_equal(obj, "TypeName") - Check if object is of specific type
    types_equal(obj1, obj2) - Check if two objects are of same type


## Modules

| Module | Description |
|---|---|
| `core` | Core classes and macros, includinf memory management |
| `db` | Generic database abstraction |
| `db-entity` | Entity manager (ORM-like) |
| `db-sqlite` | SQLite backend for `cdf-db` |
| `http` | HTTP client and multithreaded server |
| `io` | I/O classes - files, streams, etc. |
| `json` | JSON parser/serializer |
| `log` | Logging framework |


## Build & install

Requires CMake >= 3.20, gcc >= 14 (`-std=c23`). `db-sqlite` requires `libsqlite3-dev`.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
cmake --install build --prefix /usr/local
```

| Command | What it does |
|---|---|
| `cmake -S . -B build` | Configure (pass `-DCDF_BUILD_JSON=OFF` etc. to disable modules) |
| `cmake --build build` | Compile `libcdf.so` + examples |
| `ctest --test-dir build` | Run all tests (stdout, no output.log) |
| `cmake --install build` | Install `libcdf.so` + headers to `prefix` |

All modules are built into a single library (`libcdf.so`). Disable optional modules at configure time:

```sh
cmake -S . -B build \
    -DBUILD_TESTING=OFF \
    -DCDF_BUILD_EXAMPLES=OFF \
    -DCDF_BUILD_DB_SQLITE=OFF
```


## OOP with CDF quick reference

See [`docs/Tutorial.md`](docs/Tutorial.md) for a getting-started guide.


## License

MIT

# CDF Examples

Sample applications demonstrating the CDF framework.

## Prerequisites

Build the framework from the repository root:

```
cmake -S .. -B ../build
cmake --build ../build
```

Examples are built as part of the main build. Individual executables land in `build/examples/`.

```
../build/examples/helloworld/helloworld
../build/examples/shapes/shapes
../build/examples/wwwserver/wwwserver
../build/examples/todo-list-api/todo-list-api
```

## Examples

### helloworld

Basic CDF program — creates a `String` and prints it via `Console`.

### shapes

Demonstrates OOP with inheritance and polymorphism:

- `Shape` — base class with virtual `area`, `circumference`, `to_string`
- `Rectangle` — inherits `Shape`, overrides methods
- `Circle` — inherits `Shape`, overrides methods
- `Square` — inherits `Rectangle`

Creates a list of shapes and iterates polymorphically.

### wwwserver

Minimal HTTP server serving an HTML page on port 19876. Uses `cdf-http`.

### todo-list-api (disabled without DB + HTTP + Log)

JSON-based HTTP API server with SQLite persistence using `cdf-db`, `cdf-db-sqlite`, `cdf-db-entity`, `cdf-http`, `cdf-json`, and `cdf-log`.

# CDF Examples

Sample applications demonstrating the CDF framework.

## Prerequisites

All CDF modules must be built and installed first:

```
make install
```

## Build

```
make -C examples
```

## Examples

### helloworld

Basic CDF program — creates a `String` and prints it via `Console`.

```
make -C examples/helloworld run
```

### shapes

Demonstrates OOP with inheritance and polymorphism:

- `Shape` — base class with virtual `area`, `circumference`, `to_string`
- `Rectangle` — inherits `Shape`, overrides methods
- `Circle` — inherits `Shape`, overrides methods
- `Square` — inherits `Rectangle`

Creates a list of shapes and iterates polymorphically.

```
make -C examples/shapes run
```

### shop

JSON-based HTTP API server with entity persistence. Uses `cdf-http`, `cdf-json`, `cdf-db`, `cdf-db-sqlite`, and `cdf-db-entity`. Serves customer data as JSON on port 19876.

```
make -C examples/shop run
```

### wwwserver

Minimal HTTP server serving an HTML page on port 19876. Uses `cdf-http`.

```
make -C examples/wwwserver run
```

## Cleaning

```
make -C examples clean
```

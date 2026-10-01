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
../build/examples/chatter/chatter
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

### chatter (disabled without TUI)

A terminal chat client for any OpenAI-compatible endpoint, such as a
`llama-server`. Uses `cdf-tui`, `cdf-http`, `cdf-json` and `cdf-log`.

```
# Defaults to the llama.cpp server on the Docker host, port 8080.
../build/examples/chatter/chatter

# Or point it somewhere else and pick a model.
../build/examples/chatter/chatter http://localhost:8080 unsloth/Qwen3.6-35B-A3B-GGUF:Q4_K_M

# Let a reasoning model think before answering. Slower, but more thorough.
../build/examples/chatter/chatter -t
```

Type a message and press Enter. The request runs on a worker thread, so the
screen keeps updating and Ctrl-C keeps working while the model is thinking; the
reply is appended when it arrives whole.

| Key | Action |
| --- | --- |
| `Enter` | Send the input line |
| `PgUp` / `PgDn`, `Up` / `Down` | Scroll the transcript |
| `Ctrl-U` | Clear the input line |
| `Ctrl-K` | Clear the transcript |
| `Ctrl-C` | Quit |

The whole history is resent on every request, as the chat completions API
requires. Reasoning models are asked not to deliberate unless `-t` is given,
since the working can run to thousands of tokens and take minutes.

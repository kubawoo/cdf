# cdftk — CDF CLI Toolkit

A command-line tool for creating, building, and testing CDF projects.

## Commands

### `create <name>`

Scaffolds a new CDF project with:

- `src/main.c` — starter hello-world program
- `test/tc_example.c` — starter test case
- `cdfmodule.json` — module descriptor with `cdf` and `test-framework` dependencies

```
cdftk create myapp
cd myapp
```

### `build`

Builds the project in the current directory. Reads `cdfmodule.json`, resolves dependency install paths from `$CDF_HOME`, and generates a `build/Makefile` with correct include/library flags.

```
cdftk build
```

### `test`

Runs tests via the test-framework runner. Requires a prior `cdftk build`.

```
cdftk test
```

## Usage

```
cdftk create <name>   Create a new CDF project
cdftk build                        Build the project
cdftk test                         Run tests
```

## Dependencies

- `cdf` — core OOP library
- `cdf-json` — JSON parsing
- `test-framework` (test scope) — test runner

All resolved at build time via `$CDF_HOME`.

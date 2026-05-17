# py2cdf — Python-like syntax to CDF source translator

Translates a Python-like DSL into C source code using the [CDF](https://opencode.ai) OOP macro system (`new`, `call`, `inherits`, `REFCDEC`, etc.).

Input files use the `.cs` extension. The translator produces a `.csc` (C source) and a `.csh` (C header) per input file.

## Usage

```sh
python3 py2cdf.py -s -I <include-dirs> -i file.cs      # generate .csc source
python3 py2cdf.py -h -i file.cs                          # generate .csh header
python3 py2cdf.py -s -h -o outdir -i file.cs             # both, to output dir
```

### Options

| Flag | Description |
|---|---|
| `-i`, `--input` | Input `.cs` file (required) |
| `-s`, `--source` | Generate C source (`*.csc`) |
| `-h`, `--headers` | Generate C header (`*.csh`) |
| `-o`, `--output-dir` | Output directory (default: `.`) |
| `-I`, `--include` | Header search dirs for type resolution (repeatable) |
| `-v`, `--verbose` | Print generated code to stdout |
| `-d`, `--debug` | Print parsed AST and type database |
| `-p`, `--pretend` | Dry run (don't write files) |

## Input language

A restricted Python subset that maps directly to OOC constructs:

```
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

## Type resolution

The `-I` flag points `ooctypes.py` at directories containing `.h` and `.csh` files.
It parses `typedef struct { ... } TypeName;` and `struct _Name { ... };` patterns
to build a type database used for:

- Resolving method return types (via `inherits(Base)` chains)
- Looking up which class defines a method (for the `call((Class *)obj, method)` cast)
- Validating constructor calls (e.g. `SQLiteDatabase("name")`)

## Generated code conventions

- String literals in constructor args are auto-wrapped: `new(String, "...")`
- Variables declared at function top; cleaned up with `REFCDEC` at function end
- Temp variables prefixed `_tmp_N` for intermediate values

## Dependencies

- Python 3.8+
- Installed CDF modules under `$CDF_HOME` (default `~/.cdf`) for the C runtime libraries

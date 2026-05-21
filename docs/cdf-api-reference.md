# CDF Core Module API Reference

> Module: `cdf`, version `0.5.0`. Headers: `cdf.h` (umbrella), `ooc.h` (OOC classes), `io.h` (I/O classes).

---

## Class Hierarchy

```
Object (root)
  +-- String
  +-- List
  +-- Map
  +-- Array
  +-- Boolean
  +-- Integer
  +-- Long
  +-- Double
  +-- Pair
  +-- Stack
  +-- DateTime
  +-- StringTokenizer
  +-- InputStream (abstract)
  |     +-- FileInputStream
  |     +-- StringInputStream
  +-- OutputStream (abstract)
  |     +-- ConsoleOutputStream
  |     +-- FileOutputStream
  |     +-- StringOutputStream
  +-- File
  +-- Directory
  +-- Console
```

---

## Macros (ooc_macros.h)

### new(ClassName, ...)
Creates a new instance of `ClassName`. Supports 0–10 additional arguments which are forwarded to the matching `ClassName_newN` constructor.
```c
String *s = new(String, "hello");
List   *l = new(List);
```

### call(obj, method, ...)
Virtual method dispatch. Invokes `method` on `obj` with optional arguments.
```c
call(s, append_cstring, " world");
call(map, put, key, value);
```

### this(ClassName, ...)
Inline constructor for chaining in `super()` calls. Allocates via `ClassName_new` if `this` is NULL.

### delete(obj)
Force-deletes an object regardless of reference count: calls `_ooc_destructor` then `free()`.

### inherits(BaseClass)
Embeds `BaseClass` as the first struct field (named `_ooc_base`), enabling safe upcasts.

### super(BaseClass, ThisClass)
Constructor chaining. If `this` is NULL, allocates `sizeof(ThisClass)`. Calls `BaseClass_new`, sets destructor to `ThisClass_delete`, and sets the `type` field.

### super_delete(BaseClass, obj)
Calls the parent destructor: `BaseClass_delete((BaseClass*)obj)`.

### override(BaseClass, base_method, method)
Sets the function pointer for `base_method` to `method` in both the parent and current vtable slots.

### super_method(BaseClass, method)
Captures the base class implementation of `method` before overriding it.

### make_this(Class, ptr)
Casts `ptr` to `Class*` and assigns it to `this`.

### type_equal(obj, "TypeName")
Returns `true` if `obj`'s runtime type matches the given string.

### types_equal(obj1, obj2)
Returns `true` if both objects have the same runtime type string.

### REFCINC(obj)
Increments reference count.

### REFCDEC(obj)
Decrements reference count; if it reaches 0, auto-deletes.

### REFCTMP(obj)
Decrements reference count and returns the pointer (for temporary references in chained expressions).

### singleton(x)
Returns a module-level singleton instance of class `x`, created on first call.

### ObjectPtr
Typedef for `void*`, used as a generic object pointer.

---

## Memory Management

- `new()` allocates an object with reference count = 1.
- `REFCINC` / `REFCDEC` manage shared ownership.
- When `_refc` reaches 0, `delete()` is called automatically, invoking the destructor and freeing memory.
- Getters (e.g. `List.get`, `Map.get`) increment the reference count on the returned object; the caller must `REFCDEC` when done.
- `delete()` forces immediate destruction regardless of reference count.

---

## Class Reference

### Object

**File:** `ooc_object.h`

Root class of all CDF objects.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_ooc_destructor` | `void (*)(ObjectPtr)` | Destructor function pointer (set by `super()`) |
| `_refc` | `int` | Reference count |
| `type` | `const char *` | Runtime type name string |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `Object_new` | `Object *(Object *this)` | Constructor. If `this` is NULL, allocates a new `Object`. |
| `Object_delete` | `void (ObjectPtr self)` | Destructor. No-op base implementation. |
| `equals` | `bool (ObjectPtr self, ObjectPtr other)` | Identity check (`self == other`). |
| `to_string` | `String *(ObjectPtr self)` | Returns `"Object of type <type>"` as a new String. |
| `copy` | `ObjectPtr (ObjectPtr self)` | Returns NULL (subclasses override for deep copy). |

---

### String

**File:** `ooc_string.h`

Immutable-ish string object wrapping a C `char*` buffer.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `length` | `unsigned int` | Current string length |
| `_content` | `char *` | Internal C string buffer |
| `_allocated` | `unsigned int` | Allocated buffer size |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `String_new` | `String *(String *this)` | Empty string. |
| `String_new1` | `String *(String *this, const char *cstr)` | From C string (copied). |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `set_text` | `void (void *self, const char *text)` | Replace content with copy of C string. |
| `to_cstring` | `const char *(void *self)` | Returns pointer to internal C string. |
| `to_string` | `String *(void *self)` | Returns self with incremented refcount. |
| `equals` | `bool (void *self, void *other)` | Content equality with another String. |
| `copy` | `ObjectPtr (void *self)` | Deep copy (new String with same content). |
| `equals_cstring` | `bool (void *self, const char *cstr)` | Compare with C string. |
| `append_cstring` | `void (void *self, const char *cstr)` | Append C string. |
| `append` | `void (void *self, String *other)` | Append another String. |
| `append_char` | `void (void *self, const char c)` | Append single character. |
| `clear` | `void (void *self)` | Empty the string. |
| `trim` | `void (void *self)` | Remove leading/trailing whitespace in-place. |
| `contains_any` | `bool (void *self, String *chars)` | Does the string contain any of the given characters? |
| `contains_any_char` | `bool (void *self, const char *chars)` | Does the string contain any of the given C characters? |
| `index_of_char` | `int (void *self, const char c)` | First index of `c`, or -1. |
| `next_index_of_char` | `int (void *self, int from, const char c)` | Index of `c` at or after `from`, or -1. |
| `index_of_string` | `int (void *self, String *sub)` | First index of substring, or -1. |
| `next_index_of_string` | `int (void *self, int from, String *sub)` | Index of substring at or after `from`, or -1. |
| `index_of_cstring` | `int (void *self, const char *sub)` | First index of C substring, or -1. |
| `next_index_of_cstring` | `int (void *self, int from, const char *sub)` | Index of C substring at or after `from`, or -1. |
| `substring` | `String *(void *self, int from, int to)` | New String with chars `[from, to)`. |
| `substring_from` | `String *(void *self, int from)` | New String from `from` to end. |
| `format` | `void (ObjectPtr self, const char *fmt, ...)` | sprintf-style formatting into the string. |
| `char_at` | `const char (ObjectPtr self, int pos)` | Character at position (no bounds check). |

---

### List

**File:** `ooc_list.h`

Doubly-linked list.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_first` | `_ListItem *` | Pointer to first node. |
| `_last` | `_ListItem *` | Pointer to last node. |
| `length` | `int` | Number of elements. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `List_new` | `List *(List *this)` | Empty list. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `add` | `void (ObjectPtr self, ObjectPtr item)` | Append item to end. |
| `remove` | `void (ObjectPtr self, int index)` | Remove item at index (0-based). |
| `get` | `ObjectPtr (ObjectPtr self, int index)` | Item at index (REFINC'd, caller must REFCDEC). |
| `set` | `void (ObjectPtr self, int index, ObjectPtr item)` | Replace item at index. |
| `insert` | `void (ObjectPtr self, int index, ObjectPtr item)` | Insert item at index, shifting subsequent elements. |
| `contains` | `bool (ObjectPtr self, ObjectPtr item)` | Linear membership test via `equals`. |
| `to_string` | `String *(ObjectPtr self)` | Format: `"[elem1; elem2; ...]"`. |

---

### Map

**File:** `ooc_map.h`

Key-value map backed by parallel key/value lists (linear search).

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_keys` | `List *` | List of keys. |
| `_values` | `List *` | Parallel list of values. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Map_new` | `Map *(Map *this)` | Empty map. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `get_length` | `int (void *self)` | Number of entries. |
| `put` | `void (void *self, ObjectPtr key, ObjectPtr value)` | Insert key-value; replaces existing entry for same key (by `equals`). |
| `get` | `ObjectPtr (void *self, ObjectPtr key)` | Value for key (REFINC'd, caller must REFCDEC), or NULL. |
| `remove` | `void (void *self, ObjectPtr key)` | Remove entry by key. |
| `contains_key` | `bool (void *self, ObjectPtr key)` | Does key exist? |
| `contains_value` | `bool (void *self, ObjectPtr value)` | Does value exist? |
| `get_keys` | `List *(ObjectPtr self)` | Copy of keys list (REFINC'd, caller must REFCDEC). |
| `to_string` | `String *(ObjectPtr self)` | Format: `"{key=value; ...}"`. |

---

### Array

**File:** `ooc_array.h`

Fixed-size array of `Object*` pointers. Bounds-checked (aborts on out-of-bounds).

**Fields:**

| Field | Type | Description |
|---|---|---|
| `length` | `int` | Number of elements. |
| `_values` | `Object **` | Internal pointer array. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Array_new1` | `Array *(Array *this, int size)` | Pre-allocate array of `size` (all slots NULL). |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `set` | `void (ObjectPtr self, int index, ObjectPtr item)` | Set slot (REFCDECs old value, REFCINCs new). Aborts on out-of-bounds. |
| `get` | `ObjectPtr (ObjectPtr self, int index)` | Get slot (REFINC'd, caller must REFCDEC). Aborts on out-of-bounds. |

---

### Boolean

**File:** `ooc_primitives.h`

Boxed boolean value.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `value` | `bool` | The boolean value. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Boolean_new` | `Boolean *(Boolean *this)` | Default `false`. |
| `Boolean_new1` | `Boolean *(Boolean *this, bool value)` | With initial value. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `to_string` | `String *(ObjectPtr self)` | `"true"` or `"false"`. |
| `equals` | `bool (ObjectPtr self, ObjectPtr other)` | Value equality. |
| `from_string` | `bool (ObjectPtr self, String *s)` | Parse `"true"` or `"false"` (case-sensitive). Returns true on success. |

---

### Integer

**File:** `ooc_primitives.h`

Boxed `int` value.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `value` | `int` | The integer value. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Integer_new` | `Integer *(Integer *this)` | Value 0. |
| `Integer_new1` | `Integer *(Integer *this, int value)` | With initial value. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `to_string` | `String *(ObjectPtr self)` | Formatted via `printf("%d")`. |
| `equals` | `bool (ObjectPtr self, ObjectPtr other)` | Value equality (also checks runtime type). |
| `from_string` | `bool (ObjectPtr self, String *s)` | Parse via `strtol`. Returns true on success. |
| `add` | `Integer *(ObjectPtr self, Integer *other)` | Returns new Integer with sum. |
| `add_int` | `Integer *(ObjectPtr self, int n)` | Returns new Integer with sum. |
| `increment` | `void (ObjectPtr self, Integer *other)` | In-place addition. |
| `increment_int` | `void (ObjectPtr self, int n)` | In-place addition. |
| `subtract` | `Integer *(ObjectPtr self, Integer *other)` | Returns new Integer with difference. |
| `subtract_int` | `Integer *(ObjectPtr self, int n)` | Returns new Integer with difference. |
| `decrement` | `void (ObjectPtr self, Integer *other)` | In-place subtraction. |
| `decrement_int` | `void (ObjectPtr self, int n)` | In-place subtraction. |
| `multiply` | `Integer *(ObjectPtr self, Integer *other)` | Returns new Integer with product. |
| `multiply_int` | `Integer *(ObjectPtr self, int n)` | Returns new Integer with product. |
| `multiply_inplace` | `void (ObjectPtr self, Integer *other)` | In-place multiplication. |
| `multiply_inplace_int` | `void (ObjectPtr self, int n)` | In-place multiplication. |
| `divide` | `Integer *(ObjectPtr self, Integer *other)` | Returns new Integer with quotient. |
| `divide_int` | `Integer *(ObjectPtr self, int n)` | Returns new Integer with quotient. |
| `divide_inplace` | `void (ObjectPtr self, Integer *other)` | In-place division. |
| `divide_inplace_int` | `void (ObjectPtr self, int n)` | In-place division. |

---

### Long

**File:** `ooc_primitives.h`

Boxed `long` value. Same structure and methods as Integer, with `long value` field and `*_long` variants (e.g. `add_long`, `increment_long`).

---

### Double

**File:** `ooc_primitives.h`

Boxed `double` value. Same structure and methods as Integer, with `double value` field and `*_double` variants. `to_string` formats with `"%.4f"`.

---

### Pair

**File:** `ooc_pair.h`

Holds a left and right object reference.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_left` | `Object *` | Left object. |
| `_right` | `Object *` | Right object. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Pair_new2` | `Pair *(Pair *this, ObjectPtr left, ObjectPtr right)` | With left and right values (both REFCINC'd). |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `left` | `ObjectPtr (ObjectPtr self)` | Returns left (REFINC'd, caller must REFCDEC). |
| `right` | `ObjectPtr (ObjectPtr self)` | Returns right (REFINC'd, caller must REFCDEC). |
| `to_string` | `String *(ObjectPtr self)` | Format: `"<left|right>"`. |

---

### Stack

**File:** `ooc_stack.h`

LIFO stack backed by a List.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_list` | `List *` | Internal backing list. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Stack_new` | `Stack *(Stack *this)` | Empty stack. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `push` | `void (ObjectPtr self, ObjectPtr item)` | Push item onto top of stack. |
| `pop` | `ObjectPtr (ObjectPtr self)` | Remove and return top item (caller must REFCDEC). |
| `peek` | `ObjectPtr (ObjectPtr self)` | Return top item without removing (REFINC'd). |
| `size` | `int (ObjectPtr self)` | Current number of items. |
| `to_string` | `String *(ObjectPtr self)` | Delegates to underlying list. |

---

### DateTime

**File:** `ooc_datetime.h`

Date and time value.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_time` | `struct tm` | Broken-down time struct. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `DateTime_new` | `DateTime *(DateTime *this)` | Current date/time (`time(NULL)` + `localtime`). |
| `DateTime_new3` | `DateTime *(DateTime *this, int y, int m, int d)` | Date only (time set to 00:00:00). |
| `DateTime_new6` | `DateTime *(DateTime *this, int y, int m, int d, int h, int min, int s)` | Full date and time. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `to_string` | `String *(ObjectPtr self)` | ISO 8601 format: `"%FT%T"` (e.g. `2026-05-21T14:30:00`). |
| `format` | `String *(ObjectPtr self, const char *fmt)` | Custom format via `strftime`; returns new String. |

---

### StringTokenizer

**File:** `ooc_stringtokenizer.h`

Splits a string into tokens by a delimiter character.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_content` | `String *` | Source string to split. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `StringTokenizer_new1` | `StringTokenizer *(StringTokenizer *this, String *content)` | With content to split. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `split_by_char` | `List *(ObjectPtr self, const char delimiter)` | Split by `delimiter`, returns List of String tokens. |

---

### InputStream

**File:** `inputstream.h`

Abstract base class for byte-level input streams.

**Fields:**

*(None beyond inherited Object fields.)*

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `InputStream_new` | `InputStream *(InputStream *this)` | Default constructor. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `read` | `int (ObjectPtr self)` | Returns next byte (0–255) or -1 on EOF. Base implementation returns -1. |

---

### OutputStream

**File:** `outputstream.h`

Abstract base class for character output streams.

**Fields:**

*(None beyond inherited Object fields.)*

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `OutputStream_new` | `OutputStream *(OutputStream *this)` | Default constructor. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `write` | `void (ObjectPtr self, char c)` | Write single character. Base implementation is no-op. |
| `write_string` | `void (ObjectPtr self, String *s)` | Write entire string character by character. |
| `to_string` | `String *(ObjectPtr self)` | Returns representation of accumulated output (overridden by subclasses). |

---

### FileInputStream

**File:** `fileinputstream.h`

Reads characters from a File object.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_file` | `File *` | Backing File object. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `FileInputStream_new1` | `FileInputStream *(FileInputStream *this, File *file)` | Opens `file` in read mode (`"r"`). |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `read` | `int (ObjectPtr self)` | Delegates to `file->read_char()`. Returns char (0–255) or -1 on EOF. |

---

### StringInputStream

**File:** `stringinputstream.h`

Reads characters sequentially from a String.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_txt` | `String *` | Source string. |
| `_pos` | `int` | Current read position. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `StringInputStream_new1` | `StringInputStream *(StringInputStream *this, String *txt)` | With source string. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `read` | `int (ObjectPtr self)` | Returns next character or -1 at end. |

---

### ConsoleOutputStream

**File:** `consoleoutputstream.h`

Writes characters to stdout.

**Fields:**

*(None beyond inherited OutputStream fields.)*

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `ConsoleOutputStream_new` | `ConsoleOutputStream *(ConsoleOutputStream *this)` | Default constructor. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `write` | `void (ObjectPtr self, char c)` | Writes to stdout via `printf("%c", c)`. |

---

### FileOutputStream

**File:** `fileoutputstream.h`

Writes characters to a File.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_file` | `File *` | Backing File object. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `FileOutputStream_new1` | `FileOutputStream *(FileOutputStream *this, File *file)` | Opens file in write mode (`"w"`, overwrite). |
| `FileOutputStream_new2` | `FileOutputStream *(FileOutputStream *this, File *file, bool append)` | If `append` is true, opens in append mode (`"a"`); otherwise overwrite (`"w"`). |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `write` | `void (ObjectPtr self, char c)` | Delegates to `file->write_char(c)`. |

---

### StringOutputStream

**File:** `stringoutputstream.h`

Accumulates written characters into an internal String buffer.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_txt` | `String *` | Internal accumulation buffer. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `StringOutputStream_new` | `StringOutputStream *(StringOutputStream *this)` | Creates empty internal buffer. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `write` | `void (ObjectPtr self, char c)` | Appends character to internal buffer. |
| `to_string` | `String *(ObjectPtr self)` | Returns internal buffer (REFINC'd, caller must REFCDEC). |

---

### File

**File:** `file.h`

Wraps a stdio `FILE*` with OOP interface.

**Fields:**

| Field | Type | Description |
|---|---|---|
| `_fp` | `FILE *` | Underlying stdio file pointer. |
| `_path` | `String *` | File path. |

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `File_new1` | `File *(File *this, String *path)` | With file path (file is not opened yet). |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `open` | `bool (ObjectPtr self, const char *mode)` | Opens file with `fopen(path, mode)`. Returns true on success. |
| `close` | `void (ObjectPtr self)` | Closes file via `fclose`. |
| `is_open` | `bool (ObjectPtr self)` | Returns true if file pointer is not NULL. |
| `exists` | `bool (ObjectPtr self)` | Checks existence via `access(path, F_OK)`. |
| `read_char` | `int (ObjectPtr self)` | Reads single character via `fgetc`. Returns char or -1 on EOF. |
| `read` | `String *(ObjectPtr self)` | Reads entire file into a String. File must be open. |
| `write_char` | `bool (ObjectPtr self, char c)` | Writes single character via `fputc`. Returns true on success. |
| `write_cstring` | `bool (ObjectPtr self, const char *str)` | Writes C string via `fputs`. Returns true on success. |
| `write_string` | `bool (ObjectPtr self, String *s)` | Writes String content. Returns true on success. |
| `flush` | `bool (ObjectPtr self)` | Flushes via `fflush`. Returns true on success. |

---

### Directory

**File:** `directory.h`

Directory creation utility.

**Fields:**

*(None beyond inherited Object fields.)*

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Directory_new` | `Directory *(Directory *this)` | Default constructor. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `create` | `bool (ObjectPtr self, String *path)` | Creates directory with permissions 0755 via `mkdir(path, 0755)`. Returns true on success. |

---

### Console

**File:** `console.h`

Simple console output utility.

**Fields:**

*(None beyond inherited Object fields.)*

**Constructors:**

| Constructor | Signature | Description |
|---|---|---|
| `Console_new` | `Console *(Console *this)` | Default constructor. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `print_object` | `void (ObjectPtr self, ObjectPtr obj)` | Prints `obj->to_string()` via `printf`. |
| `print_cstring` | `void (ObjectPtr self, const char *str)` | Prints C string via `printf("%s", str)`. |

---

## Singleton Support

**File:** `ooc_singleton.h`

```c
#define singleton(x) /* ... */
ObjectPtr _cdf_get_singleton(const char *name);
ObjectPtr _cdf_set_singleton(const char *name, ObjectPtr instance);
```

- `singleton(ClassName)` returns a module-level singleton, created via `new(ClassName)` on first call.
- Singletons are stored in a static Map and cleaned up via `atexit`.

---

## Umbrella Headers

| Header | Includes |
|---|---|
| `cdf.h` | `ooc.h`, `io.h` |
| `ooc.h` | All `ooc_*.h` headers |
| `io.h` | All I/O class headers |

---

## Usage Example

```c
#include <cdf.h>

int main(void) {
    String *msg = new(String, "Hello, CDF!");
    Console *c = new(Console);
    call(c, print_object, msg);

    List *list = new(List);
    call(list, add, new(Integer, 42));
    call(list, add, new(String, "world"));

    for (int i = 0; i < list->length; i++) {
        Object *obj = call(list, get, i);
        call(c, print_object, obj);
        REFCDEC(obj);
    }

    REFCDEC(msg);
    REFCDEC(c);
    REFCDEC(list);
    return 0;
}
```

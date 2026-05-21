# CDF JSON Module API Reference

> Module: `cdf-json`, version `0.5.0`. Headers: `json.h` (umbrella), `jsonobject.h`, `eventparser.h`, `parserhandlers.h`.

---

## Overview

The CDF JSON module provides JSON parsing and serialization capabilities:
- **JSON parsing**: Event-based parser that converts JSON text into CDF objects (String, Integer, Double, Long, Boolean, List, JsonObject)
- **JSON serialization**: Convert JsonObject instances to JSON-formatted strings
- **Builder pattern**: Helper classes to construct JsonObject instances during parsing

The module follows an event-driven parsing model similar to SAX for XML, where the parser calls handler methods as it encounters JSON structural elements (objects, arrays, values).

---

## Dependencies

This module depends on:
- `cdf` (core OOP framework)
- Specifically uses: `Object`, `String`, `List`, `Map`, `Stack`

---

## Class Hierarchy

```
Object
  +-- JsonObject (extends Object, implements JSON object serialization)
  +-- JsonEventsHandler (base class for parsing event handlers)
  |     +-- JsonObjectBuilderEventsHandler (default handler that builds JsonObject tree)
  +-- JsonEventsParser (event-driven JSON parser)
  +-- JsonObject (also serves as base for builder handler)
```

Note: `JsonObjectBuilderEventsHandler` inherits from both `JsonEventsHandler` and `JsonObject` (via the builder pattern).

---

## Enums and Constants

### JsonEventsParser Parse Results (eventparser.h)

| Constant | Value | Description |
|---|---|---|
| `CJSON_PARSE_SUCCESS` | 0 | Parsing completed successfully |
| `CJSON_PARSE_NO_HANDLER` | 1 | No event handler provided |
| `CJSON_PARSE_NO_INPUT` | 2 | NULL input stream provided |
| `CJSON_PARSE_INVALID_JSON` | 3 | Invalid JSON syntax encountered |
| `CJSON_PARSE_INVALID_VALUE` | 4 | Invalid JSON value (e.g., malformed number) |

---

## Class Reference

### JsonObject

**File:** `jsonobject.h` / `jsonobject.c`

Represents a JSON object (map of string keys to JSON values) with serialization capabilities.

#### Fields

| Field | Type | Description |
|---|---|---|
| `_map` | `Map *` | Internal map from String* keys to Object* values (REFCINC'd) |
| `to_string` | `String *(*)(ObjectPtr)` | Virtual method returning JSON string representation |
| `put_value` | `void (*)(ObjectPtr, String *, ObjectPtr)` | Method to store a key-value pair |
| `get_value` | `ObjectPtr (*)(ObjectPtr, String *)` | Method to retrieve a value by key (REFINC'd) |
| `get_map` | `Map *(*)(ObjectPtr)` | Method to get the internal map (REFINC'd) |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `JsonObject_new` | `JsonObject *(JsonObject *this)` | Creates a new empty JsonObject with an internal Map and sets up method pointers. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `JsonObject_delete` | `void (ObjectPtr self)` | Decrements reference count on internal map and calls parent destructor. |

#### Methods

| Method | Signature | Description |
|---|---|---|
| `to_string` | `String *(ObjectPtr self)` | Returns JSON-formatted string representation of the object. Keys are quoted, values are recursively serialized (strings quoted, numbers/raw, objects/arrays formatted). |
| `put_value` | `void (ObjectPtr self, String *key, ObjectPtr value)` | Stores `value` under `key`. The value's reference count is incremented (caller retains ownership). |
| `get_value` | `ObjectPtr (ObjectPtr self, String *key)` | Returns the value associated with `key` (REFINC'd, caller must REFCDEC). Returns NULL if key not found. |
| `get_map` | `Map *(ObjectPtr self)` | Returns the internal map (REFINC'd, caller must REFCDEC). |

> **Serialization rules** (in `value_to_string` helper):
> - `null` → `"null"`
> - `String` → `"<quoted and escaped content>"` (quotes added, content not escaped beyond existing quotes)
> - `List` → `"[elem1, elem2, ...]"` (elements serialized recursively)
> - Other objects → `object->to_string()` (must return valid JSON fragment)

### JsonEventsHandler

**File:** `eventparser.h`

Abstract base class defining the event handler interface for the JSON parser.

#### Fields

| Field | Type | Description |
|---|---|---|
| `object_begin` | `void (*)(ObjectPtr, String *)` | Called when an object starts. `name` is the key in parent object/array, or NULL for root/object-in-array. |
| `object_end` | `void (*)(ObjectPtr)` | Called when an object ends. |
| `array_begin` | `void (*)(ObjectPtr, String *)` | Called when an array starts. `name` is the key in parent object, or NULL for root/array-in-array. |
| `array_end` | `void (*)(ObjectPtr)` | Called when an array ends. |
| `value` | `void (*)(ObjectPtr, String *, Object*)` | Called when a primitive value is encountered. `name` is the key in parent object, or NULL for root/value-in-array. `value` is the parsed object (caller does NOT own reference). |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `JsonEventsHandler_new` | `JsonEventsHandler *(JsonEventsHandler *this)` | Initializes all function pointers to NULL. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `JsonEventsHandler_delete` | `void (ObjectPtr self)` | Empty implementation (calls parent destructor only). |

### JsonEventsParser

**File:** `eventparser.h` / `eventparser.c`

Event-driven JSON parser that reads from an InputStream and calls handler methods.

#### Fields

| Field | Type | Description |
|---|---|---|
| `_states` | `Stack *` | Stack of parser state integers (IDLE, IN_OBJECT, etc.) |
| `_handler` | `JsonEventsHandler *` | Event handler to call (REFCINC'd) |
| `_buffer` | `String *` | Temporary buffer for accumulating number/string literals |
| `_name` | `String *` | Buffer for the current object key name |
| `_value` | `String *` | Buffer for accumulating string literal content (when not quoted) |
| `parse` | `int (*)(ObjectPtr, InputStream *)` | Main parsing method |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `JsonEventsParser_new1` | `JsonEventsParser *(JsonEventsParser *this, JsonEventsHandler *handler)` | Creates a parser with the given handler (handler's reference count is incremented). Initializes internal buffers and sets initial state to IDLE. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `JsonEventsParser_delete` | `void (ObjectPtr self)` | Decrements reference counts on handler, buffers, states stack, and calls parent destructor. |

#### Methods

| Method | Signature | Description |
|---|---|---|
| `parse` | `int (ObjectPtr self, InputStream *json_stream)` | Parses JSON from the given input stream. Returns one of the CJSON_PARSE_* constants. Consumes the entire stream unless a parsing error occurs early. |

> **Parser internals**: Uses a state machine with states:
> - `IDLE`: Whitespace or expecting '{' or '['
> - `IN_OBJECT`: Inside object, expecting '}', '"' (key start), or ','
> - `IN_ARRAY`: Inside array, expecting ']', '"' (string start), '{', '}', or ','
> - `IN_NAME`: Reading a key name (until closing quote)
> - `NAME_DONE`: Just finished reading key, expecting ':'
> - `READY_FOR_VALUE': About to parse a value (object, array, or literal)
> - `IN_VALUE`: Reading a literal value (until delimiter)

When in `IN_NAME` state and a quoted string is completed:
- If current state stack top is `IN_ARRAY`: treats the string as a value in array
- Otherwise: treats the string as an object key and transitions to `NAME_DONE`

### JsonObjectBuilderEventsHandler

**File:** `parserhandlers.h` / `parserhandlers.c`

Concrete implementation of `JsonEventsHandler` that builds a JsonObject tree from parsing events. This is the default handler used for standard JSON parsing to CDF objects.

#### Fields (inherited + own)

| Field | Type | Description |
|---|---|---|
| `object` | `JsonObject *` | Root object being built (REFCINC'd when set) |
| `_stack` | `Stack *` | Stack of intermediate objects/arrays during building (REFCINC'd items) |
| *(inherits `object_begin`, `object_end`, `array_begin`, `array_end`, `value` from JsonEventsHandler)* | | |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `JsonObjectBuilderEventsHandler_new` | `JsonObjectBuilderEventsHandler *(JsonObjectBuilderEventsHandler *this)` | Sets up method overrides for all five handler methods, creates internal stack. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `JsonObjectBuilderEventsHandler_delete` | `void (ObjectPtr self)` | Decrements reference count on stack and root object, then calls parent destructor. |

#### Event Handler Methods

| Method | Signature | Description |
|---|---|---|
| `object_begin` | `void (ObjectPtr self, String *name)` | - If `name` is NULL and stack is empty: creates root JsonObject, pushes to stack<br>- If `name` is NULL and top of stack is a List: creates JsonObject, adds it to the List, pushes JsonObject to stack<br>- If `name` is not NULL: creates JsonObject, sets it as value for `name` on peeked stack object, pushes new JsonObject to stack |
| `object_end` | `void (ObjectPtr self)` | Pops and REFCDECs the top object from stack (finished building it) |
| `array_begin` | `void (ObjectPtr self, String *name)` | Creates a new List, sets it as value for `name` on peeked stack object (or as root if name NULL and stack empty), pushes List to stack |
| `array_end` | `void (ObjectPtr self)` | Pops and REFCDECs the top List from stack (finished building it) |
| `value` | `void (ObjectPtr self, String *name, Object *value)` | - If top of stack is a List: adds `value` to the List<br>- If top of stack is a JsonObject: sets `value` for `name` on the JsonObject<br>In both cases, `value` is consumed (its reference count is NOT incremented by handler; caller retains ownership per JsonEventsHandler contract) |

> **Note on value ownership**: The parser's `_getValue` function creates new objects (Integer, Double, etc.) for literals. These objects are passed to the handler's `value` method. The handler does NOT increment the reference count (per JsonEventsHandler contract), meaning the handler assumes it does NOT own the object. However, when the handler stores the value in a List or JsonObject via `put` or `add`, those methods DO increment the reference count. Thus, the handler must NOT REFCDEC the value after storing it, but also must not leak it if not stored. The current implementation correctly handles this by not calling REFCDEC on the value in either branch.

#### Usage

After parsing completes with this handler:
- The root JsonObject is accessible via `handler->object`
- Caller must REFCDEC the handler when done
- To keep the root object beyond handler lifetime: `REFCINC(handler->object)` before deleting handler

---

## Umbrella Header

### json.h

**File:** `json.h`

Simple umbrella header that includes:
```c
#include "jsonobject.h"
#include "eventparser.h"
#include "parserhandlers.h"
```

---

## Usage Example

```c
#include <json.h>
#include <cdf.h>  // for String, Console, etc.

int main(void) {
    // 1. Parse JSON string into CdF objects
    String *json_str = new(String, 
        "{\"name\":\"John\",\"age\":30,\"pets\":[\"dog\",\"cat\"],\"address\":{\"street\":\"123 Main St\"}}");
    
    StringInputStream *stream = new(StringInputStream, json_str);
    
    // 2. Create handler and parser
    JsonObjectBuilderEventsHandler *handler = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser *parser = new(JsonEventsParser, handler);
    
    // 3. Parse
    int result = call(parser, parse, stream);
    if (result != CJSON_PARSE_SUCCESS) {
        // handle error
    }
    
    // 4. Get the parsed object
    JsonObject *root = handler->object;  // This is the root JsonObject
    REFCINC(root);  // We want to keep it after deleting handler
    
    // 5. Extract values
    String *name = call(root, get_value, new(String, "name"));
    Integer *age = call(root, get_value, new(String, "age"));
    
    Console *c = new(Console);
    call(c, print_object, name);
    call(c, print_object, age);
    
    // 6. Serialize back to JSON
    String *json_out = call(root, to_string);
    call(c, print_object, json_out);
    
    // 7. Cleanup
    REFCDEC(name);
    REFCDEC(age);
    REFCDEC(json_out);
    REFCDEC(root);
    REFCDEC(handler);
    REFCDEC(parser);
    REFCDEC(stream);
    REFCDEC(json_str);
    REFCDEC(c);
    
    return 0;
}
```

> **Output**:
> ```
> John
> 30
> {"address":{"street":"123 Main St"},"age":30,"name":"John","pets":["dog","cat"]}
> ```
> 
> (Note: Map iteration order is not guaranteed)

---

## Important Notes

1. **Reference counting with parser events**:
   - The `value` event handler receives objects that it does NOT own (no REFCINC)
   - If the handler stores the value (via `List.add` or `JsonObject.put_value`), those methods REFCINC the value
   - The handler must NOT REFCDEC the value after storing it
   - If the handler does NOT store the value, it must REFCDEC it to avoid leaks
   - The default `JsonObjectBuilderEventsHandler` always stores values, so it never REFCDECs them

2. **Stream ownership**:
   - The parser does NOT take ownership of the InputStream
   - Caller must manage the stream's lifetime

3. **Error handling**:
   - Parser returns error codes; does not abort or exit
   - After an error, parser state is undefined; create a new parser for retry

4. **Number parsing**:
   - The parser uses simple heuristics: 
     - Contains `.` → Double
     - Else → Long
   - This means integers that fit in Long are parsed as Long, not Integer
   - Boolean and string literals are detected by content

5. **UTF-8 support**:
   - The parser processes raw bytes; does not validate or decode UTF-8
   - String values are stored as byte sequences; proper display requires UTF-8 capable console

6. **Extending the handler**:
   - To create a custom handler, inherit from `JsonEventsHandler` and override the methods you need
   - For partial building (e.g., only collect certain fields), override `value` and ignore unwanted names
   - Remember the ownership contract: `value` method does NOT own the passed object

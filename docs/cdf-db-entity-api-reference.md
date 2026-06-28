# CDF Entity Manager Module API Reference

> Module: `cdf-db-entity`, version `0.5.0`. Headers: `db_entity.h` (umbrella, includes `entity.h`), `entity.h`, `entitymanager.h`.

---

## Overview

The CDF Entity Manager module provides an Object-Relational Mapping (ORM) abstraction layer built on top of the CDF database abstraction (`cdf-db`). It allows developers to work with database records as objects (entities) with automatic persistence.

Key features:
- **Entity**: Base class for persistent objects with automatic ID field and JSON/map serialization
- **EntityManager**: Manages entity persistence (save, load, update, remove) and table schema management
- **FieldMetadata**: Describes the fields of an entity (name, type, offset in memory)
- **Automatic Table Creation**: Creates tables based on entity field definitions
- **JSON Serialization**: Convert entities to/from JSON using the CDF JSON module

This module follows the Active Record pattern where entities know how to persist themselves.

---

## Dependencies

This module depends on:
- `cdf-db` (database abstraction)
- `cdf-json` (JSON parsing/serialization)
- `cdf` (core OOP framework)

---

## Class Hierarchy

```
Object
  +-- Entity (abstract base for persistent entities)
  |     +-- UserEntity (example user-defined entity)
  +-- EntityManager (manages entity persistence)
  +-- EntityMetadata (internal cache of entity schema)
  +-- FieldMetadata (describes entity fields)
```

---

## Class Reference

### Entity

**File:** `entity.h` / `entity.c`

Abstract base class for persistent entities. Defines an ID field and methods for serialization, mapping, and table name definition. To create a persistent entity, inherit from this class and define fields.

#### Fields

| Field | Type | Description |
|---|---|---|
| `id` | `Long *` | Unique identifier for the entity (auto-generated on save) |
| `_fields` | `List * (*)(ObjectPtr)` | Internal method to get field metadata (without ID field) |
| `fields` | `List * (*)(ObjectPtr, bool)` | Get field metadata (with or without ID field) |
| `table_name` | `String *(*)(ObjectPtr)` | Get the table name for this entity (defaults to class name) |
| `to_json` | `String *(*)(ObjectPtr, List *)` | Convert entity to JSON string (only specified fields) |
| `from_json` | `void (*)(ObjectPtr, String *, List *)` | Populate entity from JSON string (only specified fields) |
| `to_map` | `Map *(*)(ObjectPtr, List *)` | Convert entity to Map (field name -> value) |
| `from_map` | `void (*)(ObjectPtr, Map *, List *)` | Populate entity from Map (field name -> value) |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `Entity_new` | `Entity *(Entity *this)` | Initializes the entity with default method implementations. Sets `id` to `NULL`. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `Entity_delete` | `void (ObjectPtr self)` | Decrements reference count on `id` and calls parent destructor. |

#### Methods

| Method | Signature | Description |
|---|---|---|
| `table_name` | `String *(ObjectPtr self)` | Returns the table name (defaults to the object's type string, e.g., `"UserEntity"`). Override in subclasses to customize. |
| `to_json` | `String *(ObjectPtr self, List *fields)` | Serializes the entity to a JSON string. If `fields` is `NULL`, uses all fields (including ID). Returns a new `String` (caller must `REFCDEC`). |
| `from_json` | `void (ObjectPtr self, String *json, List *fields)` | Populates the entity from a JSON string. If `fields` is `NULL`, uses all fields. Ignores fields not present in the entity. |
| `to_map` | `Map *(ObjectPtr self, List *fields)` | Converts the entity to a `Map` where keys are field names and values are field values. If `fields` is `NULL`, uses all fields (including ID). Returns a new `Map` (caller must `REFCDEC`). |
| `from_map` | `void (ObjectPtr self, Map *map, List *fields)` | Populates the entity from a `Map`. If `fields` is `NULL`, uses all fields. Ignores map keys not corresponding to entity fields. |
| `_fields` | `List *(ObjectPtr self)` | Internal method to get field metadata (without ID field). Returns an empty `List` by default; subclasses should override to return actual field metadata. |
| `fields` | `List *(ObjectPtr self, bool with_id)` | Returns field metadata. If `with_id` is `true`, prepends the ID field to the list. Returns a new `List` (caller must `REFCDEC`). |

> **FieldMetadata**: Describes a field in an entity.
> 
> **File:** `entity.h`
> 
> | Field | Type | Description |
> |---|---|---|
> | `offset` | `intptr_t` | Byte offset of the field from the start of the entity object |
> | `name` | `String *` | Field name (REFCINC'd) |
> | `type` | `String *` | Field type string (e.g., `"String"`, `"Integer"`, `"Long"`; REFCINC'd) |
> 
> **Constructor**:
> - `FieldMetadata_new4`: `FieldMetadata *(FieldMetadata *this, Entity *obj, Object **field, String *name, String *type)`
>   - Creates field metadata for a field at `field` address within `obj`
>   - `name` and `type` strings are REFCINC'd and stored
>   - `offset` is calculated as `(intptr_t)field - (intptr_t)obj`
> 
> **Destructor**:
> - `FieldMetadata_delete`: `void (ObjectPtr self)`
>   - Decrements reference counts on `name` and `type`, then calls parent destructor

---

### EntityManager

**File:** `entitymanager.h` / `entitymanager.c`

Manages the persistence of entities to a database. Handles table creation, CRUD operations, and caching of entity metadata.

#### Fields

| Field | Type | Description |
|---|---|---|
| `load` | `void (*)(ObjectPtr, Entity *)` | Load an entity by ID from the database (only ID needs to be set) |
| `remove` | `void (*)(ObjectPtr, Entity *)` | Remove an entity from the database (only ID needs to be set) |
| `save` | `void (*)(ObjectPtr, Entity *)` | Save a new entity to the database (ID should NOT be set) |
| `update` | `void (*)(ObjectPtr, Entity *)` | Update an existing entity in the database (all fields should be set) |
| `db` | `Database *` | Database factory used to create connections |
| `conn` | `DbConnection *` | Active database connection |
| `known_entities` | `Map *` | Cache of entity metadata (key: entity type string, value: `EntityMetadata*`) |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `EntityManager_new1` | `EntityManager *(EntityManager *this, Database *db)` | Creates a new entity manager using the given database factory. Opens a connection from the database and initializes the metadata cache. The database reference is REFCINC'd. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `EntityManager_delete` | `void (ObjectPtr self)` | Decrements reference counts on `conn`, `db`, and `known_entities`, then calls parent destructor. |

#### Methods

| Method | Signature | Description |
|---|---|---|
| `load` | `void (ObjectPtr self, Entity *entity)` | Loads an entity by its ID. Only the entity's `id` field needs to be set before calling. After the call, the entity's fields will be populated from the database. If no entity is found with the given ID, the ID is set to `-1`. |
| `remove` | `void (ObjectPtr self, Entity *entity)` | Removes the entity with the given ID from the database. Only the entity's `id` field needs to be set before calling. |
| `save` | `void (ObjectPtr self, Entity *entity)` | Saves a new entity to the database. The entity's ID field must NOT be set (or be `NULL`) before calling. After the call, the entity's ID field will be set to the auto-generated database ID. |
| `update` | `void (ObjectPtr self, Entity *entity)` | Updates an existing entity in the database. All fields of the entity should be set before calling. The entity's ID field is used to determine which row to update. (Note: Currently a TODO stub in the implementation.) |

#### Internal Methods (Not part of public API)

| Method | Signature | Description |
|---|---|---|
| `get_metadata` | `EntityMetadata *(ObjectPtr self, Entity *entity)` | Retrieves or creates metadata for the given entity type. If the entity type is not in the cache, creates metadata by calling the entity's `fields` method, checks if the table exists, and creates it if not. |
| `table_exists` | `bool (DbConnection *conn, String *tablename)` | Checks if a table exists in the database. |
| `create_table` | `void (DbConnection *conn, String *tablename, List *fields)` | Creates a table with the given name and fields. Maps field types to SQL types (String→TEXT, Integer/Long→INT). |

### EntityMetadata

**File:** `entitymanager.h` / `entitymanager.c`

Internal class used by `EntityManager` to cache entity schema information.

#### Fields

| Field | Type | Description |
|---|---|---|
| `table_name` | `String *` | Name of the database table for this entity (REFCINC'd) |
| `fields` | `List *` | List of `FieldMetadata` objects for this entity (REFCINC'd) |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `EntityMetadata_new2` | `EntityMetadata *(EntityMetadata *this, String *table_name, List *fields)` | Creates metadata with the given table name and field list. The table_name and fields are REFCINC'd and stored. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `EntityMetadata_delete` | `void (ObjectPtr self)` | Decrements reference counts on `table_name` and `fields`, then calls parent destructor. |

---

## Usage Example

```c
#include <db_entity.h>
#include <cdf.h>  // for String, Console, etc.
#include <cdf-db.h>  // for Database, SQLiteDatabase
#include <cdf-db-sqlite.h>  // for SQLiteDatabase

// 1. Define an entity class by inheriting from Entity
typedef struct {
    inherits(Entity);
    Long * id;
    String * name;
    Integer * age;
} UserEntity;

// 2. Implement the _fields method to return field metadata
static List * UserEntity_fields(ObjectPtr _this) {
    make_this(UserEntity, _this);
    List * fields = new(List);
    
    // Add name field
    FieldMetadata * name_fm = new(FieldMetadata, this, &(this->name),
        new(String, "name"), new(String, "String"));
    call(fields, add, name_fm);
    REFCDEC(name_fm);
    
    // Add age field
    FieldMetadata * age_fm = new(FieldMetadata, this, &(this->age),
        new(String, "age"), new(String, "Integer"));
    call(fields, add, age_fm);
    REFCDEC(age_fm);
    
    return fields;
}

// 3. Optionally override table_name (defaults to class name)
static String * UserEntity_table_name(ObjectPtr _this) {
    return new(String, "users");
}

// 4. In your type's initialization, set the method pointers
// (This would typically be done in a constructor or initialization function)
// For simplicity in this example, we'll assume these are set via overrides
// In practice, you'd use the OOC macros to set these in the constructor

int main(void) {
    // 5. Set up database and entity manager
    SQLiteDatabase *db = new(SQLiteDatabase);  // in-memory database
    EntityManager *em = new(EntityManager, db);
    
    // 6. Create and save an entity
    UserEntity *user = new(UserEntity);
    // Note: In a real implementation, you would set the method pointers
    // for _fields and table_name in the UserEntity constructor
    // For this example, we'll assume they're set correctly
    
    call(user, set_text, new(String, "John Doe"));  // Assuming name field
    call(user, set_int, new(Integer, 30));         // Assuming age field
    
    call(em, save, user);  // Saves to database and sets user->id
    
    // 7. Load the entity back
    UserEntity *loaded_user = new(UserEntity);
    call(loaded_user, set_id, call(user, id));  // Set ID to load
    call(em, load, loaded_user);
    
    // 8. Use the loaded entity
    Console *c = new(Console);
    call(c, print_object, call(loaded_user, get_field, new(String, "name")));
    call(c, print_object, call(loaded_user, get_field, new(String, "age")));
    
    // 9. Cleanup
    REFCDEC(loaded_user);
    REFCDEC(user);
    REFCDEC(em);
    REFCDEC(db);
    REFCDEC(c);
    
    return 0;
}
```

> **Note**: The example above is simplified. In practice, you would:
> 1. Define your entity as a struct inheriting from Entity
> 2. Implement the `_fields` method to return your field metadata
> 3. Optionally override `table_name` if you want a custom table name
> 4. Use the OOC macros properly in your entity's constructor to set up the method pointers
> 5. The EntityManager will automatically create tables based on your entity definitions

---

## Important Notes

1. **Reference Counting**:
   - The `EntityManager` retains a reference to the `Database` and `DbConnection` (caller must manage their lifetimes appropriately)
   - The `EntityManager` caches `EntityMetadata` objects in its `known_entities` map (REFCINC'd when stored)
   - Entities passed to manager methods are NOT retained by the manager (caller retains ownership)
   - The `id` field of an entity is REFCINC'd when set and REFCDEC'd when cleared or when the entity is deleted

2. **Automatic Table Creation**:
   - When an entity is first used with an EntityManager, the manager checks if the corresponding table exists
   - If not, it creates the table with columns for each field (plus an `id INTEGER PRIMARY KEY` column)
   - Field type mapping: `String` - `TEXT`, `Integer`/`Long` - `INT` (Note: This is a simplification; real implementations might need more precise mapping)

3. **JSON and Map Serialization**:
   - The `to_json`/`from_json` and `to_map`/`from_map` methods work with a field list
   - If the field list is `NULL`, all fields (including ID) are processed
   - These methods use the CDF JSON module internally
   - Only fields present in both the entity and the provided list are processed

4. **Error Handling**:
   - The implementation does not currently return error codes from entity manager methods
   - Errors during database operations are printed to stderr via `fprintf`
   - For production use, consider extending the interface to provide error information

5. **Limitations**:
   - The `update` method is currently a TODO stub
   - No support for relationships between entities (no foreign key handling)
   - No support for cascading deletes or updates
   - No query capabilities beyond loading by ID
   - Table name defaults to the entity's type string (which may not be desirable)

6. **Thread Safety**:
   - The EntityManager is **not thread-safe**. Each instance should be used by only one thread at a time.
   - If multiple threads need to access the database, each should have its own EntityManager (and thus its own database connection)

7. **Memory Management**:
   - Entities manage the lifetime of their field objects (String, Integer, etc.) via normal OOP reference counting
   - When an entity is deleted, its `id` field is DECREF'd
   - FieldMetadata objects are managed by the EntityManager's cache and are cleaned up when the manager is deleted

---

## See Also

- [CDF Database Abstraction API](cdf-db-api-reference.md) for the underlying database interfaces
- [CDF SQLite Database Driver](cdf-db-sqlite-api-reference.md) for a concrete database implementation
- [CDF JSON API](cdf-json-api-reference.md) for JSON serialization details used by the entity manager

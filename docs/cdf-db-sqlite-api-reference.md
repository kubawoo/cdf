# CDF SQLite Database Driver API Reference

> Module: `cdf-db-sqlite`, version `0.5.0`. Headers: `db_sqlite.h` (umbrella), `sqlite_database.h`, `sqlite_connection.h`, `sqlite_prepared_statement.h`.

---

## Overview

The CDF SQLite database driver provides a concrete implementation of the CDF database abstraction (`cdf-db`) for SQLite. It includes:
- **SQLiteDatabase**: Factory for creating SQLite connections (extends `Database`)
- **SQLiteConnection**: SQLite-specific connection implementation (extends `DbConnection`)
- **SQLitePreparedStatement**: SQLite-specific prepared statement (extends `DbPreparedStatement`)

This driver allows CDF applications to interact with SQLite databases using the generic `cdf-db` interfaces.

---

## Dependencies

This module depends on:
- `cdf-db` (database abstraction)
- `cdf` (core OOP framework)
- System SQLite3 library (via `-lsqlite3`)

---

## Class Hierarchy (within this module)

```
Database (from cdf-db)
  +-- SQLiteDatabase
DbConnection (from cdf-db)
  +-- SQLiteConnection
DbPreparedStatement (from cdf-db)
  +-- SQLitePreparedStatement
```

---

## Class Reference

### SQLiteDatabase

**File:** `sqlite_database.h` / `sqlite_database.c`

Factory for creating SQLite database connections. Can connect to a file-based database or an in-memory database.

#### Fields

| Field | Type | Description |
|---|---|---|
| `_name` | `String *` | Database filename or `":memory:"` for in-memory database (REFCINC'd) |
| *(inherits `to_string` and `open` from `Database`)* | | |

#### Constructors

| Constructor | Signature | Description |
|---|---|---|
| `SQLiteDatabase_new` | `SQLiteDatabase *(SQLiteDatabase *this)` | Creates an in-memory database (`":memory:"`). |
| `SQLiteDatabase_new1` | `SQLiteDatabase *(SQLiteDatabase *this, String *name)` | Creates a connection to the database file `name` (or `":memory:"`). The name string is REFCINC'd and stored. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `SQLiteDatabase_delete` | `void (ObjectPtr self)` | Decrements reference count on `_name` and calls parent (`Database`) destructor. |

#### Methods (Overrides from Database)

| Method | Signature | Description |
|---|---|---|
| `to_string` | `String *(ObjectPtr self)` | Returns a string like: `"SQLite v<version>; name=<dbname>"` where `<version>` is from `sqlite3_libversion()` and `<dbname>` is the database name. |
| `open` | `DbConnection *(ObjectPtr self)` | Opens a new `SQLiteConnection` to the database. Returns `NULL` if `sqlite3_open` fails. The caller owns the reference and must `REFCDEC` the connection when done. |

---

### SQLiteConnection

**File:** `sqlite_connection.h` / `sqlite_connection.c`

Concrete implementation of `DbConnection` for SQLite. Provides methods to execute SQL, query, prepare statements, and retrieve error information.

#### Fields

| Field | Type | Description |
|---|---|---|
| `_db` | `sqlite3 *` | Pointer to the SQLite database connection handle |
| *(inherits function pointers from `DbConnection`)* | | |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `SQLiteConnection_new1` | `SQLiteConnection *(SQLiteConnection *this, sqlite3 *db)` | Wraps an existing `sqlite3*` handle (typically from `sqlite3_open`). The connection takes ownership of the handle and will close it when deleted. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `SQLiteConnection_delete` | `void (ObjectPtr self)` | Calls parent (`DbConnection`) destructor, which in turn calls the `close` method (see below). |

#### Methods (Overrides from DbConnection)

| Method | Signature | Description |
|---|---|---|
| `exec` | `bool (ObjectPtr self, String *sql)` | Executes a SQL statement (non-query) via `sqlite3_exec`. Returns `true` on success (`SQLITE_OK`), `false` on failure. |
| `query` | `List *(ObjectPtr self, String *sql)` | Convenience method that prepares and executes a query, returning the result set. Equivalent to calling `prepare_statement` then `query_statement` with no bindings and then deleting the statement. Returns a `List` of `Map` objects (each map represents a row, with column names as keys and values as `Object*` (String, Integer, Double, etc.)). The caller owns the reference and must `REFCDEC` the list and its elements. Returns `NULL` if preparation fails. |
| `last_error_msg` | `String *(ObjectPtr self)` | Returns the last error message (from `sqlite3_errmsg`) as a new `String`. The caller owns the reference and must `REFCDEC` it. |
| `last_error_code` | `int (ObjectPtr self)` | Returns the last error code (from `sqlite3_errcode`). |
| `last_inserted_id` | `long (ObjectPtr self)` | Returns the row ID of the last inserted row (from `sqlite3_last_insert_rowid`). |
| `close` | `void (ObjectPtr self)` | Closes the SQLite connection via `sqlite3_close` and sets `_db` to `NULL`. |
| `prepare_statement` | `DbPreparedStatement *(ObjectPtr self, String *sql)` | Prepares a SQL statement and returns a `SQLitePreparedStatement` object. Returns `NULL` if `sqlite3_prepare_v2` fails. The caller owns the reference and must `REFCDEC` the statement when done. |
| `exec_statement` | `bool (ObjectPtr self, DbPreparedStatement *stmt, List *bindings)` | Executes a prepared statement with the given list of bindings. Each binding can be a `String`, `Integer`, or `Double` object. Returns `true` if the statement executed successfully (`SQLITE_DONE`). |
| `query_statement` | `List *(ObjectPtr self, DbPreparedStatement *stmt, List *bindings)` | Executes a prepared query with the given bindings and returns the result set as a `List` of `Map` objects (each map: column name -> value). The caller owns the reference and must `REFCDEC` the list and its elements. |

> **Binding Notes**: 
> - The `bindings` list (if not NULL) is iterated and each element is bound as a parameter (1-indexed).
> - Supported binding types: `String` (as text), `Integer` (as integer), `Double` (as real).
> - After binding, each binding object is `REFCDEC'd` (the caller retains ownership of the original list and its elements? Actually, the function `REFCDEC`s each binding after use, so the caller should not rely on the binding objects after calling this method. The caller must manage the lifetime of the binding objects independently; the method does not take ownership of the list or its elements beyond the duration of the call.)

---

### SQLitePreparedStatement

**File:** `sqlite_prepared_statement.h` / `sqlite_prepared_statement.c`

Concrete implementation of `DbPreparedStatement` for SQLite. Holds a compiled SQL statement.

#### Fields

| Field | Type | Description |
|---|---|---|
| `_stmt` | `sqlite3_stmt *` | Pointer to the prepared statement handle |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `SQLitePreparedStatement_new2` | `SQLitePreparedStatement *(SQLitePreparedStatement *this, sqlite3 *db, String *sql)` | Prepares the SQL string `sql` using the given database connection handle `db`. The statement is not owned by the `sqlite3*` handle; the statement must be finalized (via `sqlite3_finalize`) when done, which is handled by the destructor. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `SQLitePreparedStatement_delete` | `void (ObjectPtr self)` | Finalizes the prepared statement via `sqlite3_finalize` and calls parent destructor. |

> **Note**: This class does not override any methods from `DbPreparedStatement` (it is a marker interface). The actual execution is done via the `SQLiteConnection` methods `exec_statement` and `query_statement`, which take a `DbPreparedStatement*` parameter.

---

## Usage Example

```c
#include <db_sqlite.h>  // includes sqlite_database.h
#include <cdf.h>        // for String, Console, etc.

int main(void) {
    // 1. Create a database instance (in-memory for this example)
    SQLiteDatabase *db = new(SQLiteDatabase);
    
    // 2. Open a connection
    DbConnection *conn = call(db, open);
    if (conn == NULL) {
        // handle error (e.g., check last_error_msg/code on db? Actually, errors are on connection)
        REFCDEC(db);
        return 1;
    }
    
    // 3. Create a table
    String *create_sql = new(String, 
        "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    if (!call(conn, exec, create_sql)) {
        String *err = call(conn, last_error_msg);
        call(c, print_object, err);
        REFCDEC(err);
        // cleanup
        REFCDEC(create_sql);
        REFCDEC(conn);
        REFCDEC(db);
        return 1;
    }
    REFCDEC(create_sql);
    
    // 4. Insert a row using a prepared statement
    String *insert_sql = new(String, "INSERT INTO users (name, age) VALUES (?, ?)");
    DbPreparedStatement *stmt = call(conn, prepare_statement, insert_sql);
    if (stmt == NULL) {
        // handle error
        REFCDEC(insert_sql);
        REFCDEC(conn);
        REFCDEC(db);
        return 1;
    }
    
    // Bind parameters
    String *name = new(String, "Alice");
    Integer *age = new(Integer, 30);
    List *bindings = new(List);
    call(bindings, add, name);
    call(bindings, add, age);
    
    if (!call(conn, exec_statement, stmt, bindings)) {
        String *err = call(conn, last_error_msg);
        call(c, print_object, err);
        REFCDEC(err);
    }
    
    // Cleanup bindings and statement
    REFCDEC(bindings);
    REFCDEC(stmt);
    REFCDEC(insert_sql);
    // Note: name and age are REFCDEC'd by the binding loop in exec_statement
    
    // 5. Query the table
    String *select_sql = new(String, "SELECT id, name, age FROM users");
    List *results = call(conn, query, select_sql);
    if (results != NULL) {
        // Print each row
        Console *c = new(Console);
        for (int i = 0; i < results->length; i++) {
            Map *row = call(results, get, i);
            String *id_str = call(call(row, get, new(String, "id")), to_string);
            String *name_str = call(call(row, get, new(String, "name")), to_string);
            String *age_str = call(call(row, get, new(String, "age")), to_string);
            
            call(c, print_object, id_str);
            call(c, print_object, name_str);
            call(c, print_object, age_str);
            
            REFCDEC(id_str);
            REFCDEC(name_str);
            REFCDEC(age_str);
            REFCDEC(row);
        }
        REFCDEC(c);
        REFCDEC(results);
    }
    REFCDEC(select_sql);
    
    // 6. Cleanup
    REFCDEC(conn);
    REFCDEC(db);
    
    return 0;
}
```

> **Output** (to console):
> ```
> 1
> Alice
> 30
> ```

---

## Important Notes

1. **Reference Counting**:
   - The `SQLiteDatabase.open` method returns a `SQLiteConnection` with an incremented reference count (caller must `REFCDEC`).
   - The `SQLiteConnection.prepare_statement` method returns a `SQLitePreparedStatement` with an incremented reference count (caller must `REFCDEC`).
   - The `query` and `query_statement` methods return a `List` of `Map` objects. Each `Map` and its contents (column name `String` and value objects) have incremented reference counts; the caller must `REFCDEC` the list and all its elements.
   - The `last_error_msg` method returns a `String` with an incremented reference count (caller must `REFCDEC`).
   - In binding functions (`exec_statement`, `query_statement`), each binding object is `REFCDEC'd` after use, so the caller should not access the binding objects after the call. The caller retains ownership of the `List` and its elements (they are not `REFCDEC'd` by the binding loop).

2. **Error Handling**:
   - After any method that might fail (`open`, `exec`, `prepare_statement`, `exec_statement`, `query_statement`), check the connection's `last_error_code` and `last_error_msg`.
   - The `query` method returns `NULL` if statement preparation fails.

3. **Thread Safety**:
   - SQLite connections are **not thread-safe**. Each connection should be used by only one thread at a time.
   - The SQLite library can be configured for multi-threading, but this driver does not implement any locking. If you need to share a connection between threads, you must synchronize access externally.

4. **Resource Management**:
   - Always close connections when done to release the database file handle.
   - Statements are finalized automatically when deleted (via destructor).

5. **Type Mapping**:
   - SQLite types -> CDF objects:
     - `INTEGER` -> `Integer`
     - `TEXT` -> `String`
     - `FLOAT` or `REAL` -> `Double`
     - `BLOB` -> currently ignored (returns `NULL`); consider extending to handle blobs if needed.
   - Note: SQLite's `INTEGER` is 64-bit, but CDF's `Integer` is `int` (typically 32-bit). For large integers, consider using `Long` (but the current implementation uses `Integer`). This is a limitation; you may need to adjust the `get_value` function in `sqlite_connection.c` to return `Long` for large values.

6. **Extensions**:
   - To support additional SQLite features (e.g., transactions, blobs, custom functions), extend the `SQLiteConnection` class or use the underlying `sqlite3*` handle via a getter (not currently provided; you could add one).

---

## See Also

- [CDF Database Abstraction API](cdf-db-api-reference.md) for the generic interfaces this driver implements.
- [CDF Entity Manager API](cdf-db-entity-api-reference.md) for an ORM-like layer that can work with this driver.

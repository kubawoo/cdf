# CDF Database Abstraction Module API Reference

> Module: `cdf-db`, version `0.5.0`. Headers: `db.h` (umbrella), `database.h`, `connection.h`, `prepared_statement.h`.

---

## Overview

The CDF database abstraction module provides a generic interface for interacting with SQL databases. It defines:
- **Database**: Abstract factory for creating connections
- **Connection**: Interface for executing queries and statements
- **PreparedStatement**: Interface for prepared SQL statements

This module is designed to be extended by specific database drivers (e.g., SQLite, PostgreSQL, MySQL). The concrete implementations provide the actual database-specific logic.

---

## Dependencies

This module depends on:
- `cdf` (core OOP framework)
- Specifically uses: `Object`, `String`, `List`

---

## Class Hierarchy

```
Object
  +-- Database (abstract)
  |     +-- SQLiteDatabase (in cdf-db-sqlite)
  +-- DbConnection (abstract)
  |     +-- SQLiteConnection (in cdf-db-sqlite)
  +-- DbPreparedStatement (abstract)
        +-- SQLitePreparedStatement (in cdf-db-sqlite)
```

---

## Macros and Conventions

- All classes inherit from `Object` and use the standard CDF OOP macros (`new`, `call`, `delete`, `REFCINC`, `REFCDEC`, etc.)
- Methods that return `Object*` (including `String*`, `List*`, etc.) increment the reference count on the returned object; the caller must `REFCDEC` when done.
- Error handling is done via `last_error_code()` and `last_error_msg()` methods on the connection.

---

## Class Reference

### Database

**File:** `database.h` / `database.c`

Abstract base class for database factories. Subclasses (like `SQLiteDatabase`) implement the `open` method to create concrete connections.

#### Fields

| Field | Type | Description |
|---|---|---|
| `to_string` | `String *(*)(ObjectPtr)` | Virtual method for string representation |
| `open` | `DbConnection *(*)(ObjectPtr)` | Abstract method to open a connection; must be implemented by subclasses |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `Database_new` | `Database *(Database *this)` | Sets up default implementations: `to_string` returns `"Database of unknown type"`, `open` returns `NULL`. Subclasses should override these. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `Database_delete` | `void (ObjectPtr self)` | Calls parent destructor. |

#### Methods

| Method | Signature | Description |
|---|---|---|
| `to_string` | `String *(ObjectPtr self)` | Returns a string representation of the database instance. |
| `open` | `DbConnection *(ObjectPtr self)` | Opens and returns a new connection. Returns `NULL` on failure. The caller owns the reference and must `REFCDEC` the connection when done. |

---

### DbConnection

**File:** `connection.h` / `connection.c`

Abstract base class for database connections. Subclasses implement methods to execute SQL, query, prepare statements, and retrieve error information.

#### Fields

| Field | Type | Description |
|---|---|---|
| `exec` | `bool (*)(ObjectPtr, String *)` | Execute a SQL statement (non-query) |
| `query` | `List *(*)(ObjectPtr, String *)` | Execute a query and return results as a List of String (each row as a String? Actually, see notes) |
| `last_error_msg` | `String *(*)(ObjectPtr)` | Get the last error message as a String |
| `last_error_code` | `int (*)(ObjectPtr)` | Get the last error code as an integer |
| `last_inserted_id` | `long (*)(ObjectPtr)` | Get the last inserted row ID |
| `close` | `void (*)(ObjectPtr)` | Close the connection |
| `prepare_statement` | `DbPreparedStatement *(*)(ObjectPtr, String *)` | Prepare a SQL statement for later execution |
| `exec_statement` | `bool (*)(ObjectPtr, DbPreparedStatement *, List *)` | Execute a prepared statement with parameters |
| `query_statement` | `List *(*)(ObjectPtr, DbPreparedStatement *, List *)` | Execute a prepared query with parameters |

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `DbConnection_new` | `DbConnection *(DbConnection *this)` | Initializes all function pointers to `NULL` except `close` (set to empty function) and `exec` (set to return `false`). Subclasses must set the appropriate function pointers. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `DbConnection_delete` | `void (ObjectPtr self)` | Calls the `close` method (if set) and then the parent destructor. |

#### Methods

| Method | Signature | Description |
|---|---|---|
| `exec` | `bool (ObjectPtr self, String *sql)` | Execute a SQL statement (e.g., INSERT, UPDATE, DELETE). Returns `true` on success, `false` on failure. |
| `query` | `List *(ObjectPtr self, String *sql)` | Execute a query and return the result set. Returns a `List` where each element is a `String` representing a row? Actually, the current implementation returns `NULL`. Subclasses should return a meaningful representation (e.g., List of Maps or Lists). The caller must `REFCDEC` the returned list and its elements. |
| `last_error_msg` | `String *(ObjectPtr self)` | Returns the last error message (REFINC'd, caller must REFCDEC). |
| `last_error_code` | `int (ObjectPtr self)` | Returns the last error code. |
| `last_inserted_id` | `long (ObjectPtr self)` | Returns the last inserted row ID. |
| `close` | `void (ObjectPtr self)` | Closes the connection, releasing any resources. |
| `prepare_statement` | `DbPreparedStatement *(ObjectPtr self, String *sql)` | Prepares a SQL statement and returns a statement object. Returns `NULL` on failure. The caller owns the reference and must `REFCDEC` the statement when done. |
| `exec_statement` | `bool (ObjectPtr self, DbPreparedStatement *stmt, List *params)` | Executes a prepared statement with the given parameters. Returns `true` on success. |
| `query_statement` | `List *(ObjectPtr self, DbPreparedStatement *stmt, List *params)` | Executes a prepared query with the given parameters and returns the result set. |

> **Note on `query` and `query_statement`**: The current abstract implementation returns `NULL`. Concrete implementations (like in cdf-db-sqlite) should return a `List` where each element represents a row. The exact format (e.g., List of Strings, List of Maps) is implementation-specific. Callers should consult the specific driver's documentation.

---

### DbPreparedStatement

**File:** `prepared_statement.h` / `prepared_statement.c`

Abstract base class for prepared SQL statements. Currently a marker interface with no methods beyond those inherited from `Object`.

#### Fields

*(None beyond inherited Object fields.)*

#### Constructor

| Constructor | Signature | Description |
|---|---|---|
| `DbPreparedStatement_new` | `DbPreparedStatement *(DbPreparedStatement *this)` | Default constructor. |

#### Destructor

| Method | Signature | Description |
|---|---|---|
| `DbPreparedStatement_delete` | `void (ObjectPtr self)` | Calls parent destructor. |

> **Note**: This class is intended to be extended by concrete implementations (e.g., `SQLitePreparedStatement`) that add methods for binding parameters and executing the statement. However, in the current abstraction, the methods for executing prepared statements are on the `DbConnection` class (`exec_statement`, `query_statement`) and take the statement as a parameter.

---

## Usage Example (Abstract)

Since this is an abstraction layer, usage requires a concrete implementation (e.g., from cdf-db-sqlite). Here is how you would use it generically:

```c
#include <db.h>
#include <cdf.h>  // for String, Console, etc.

int main(void) {
    // Assume we have a concrete Database instance (e.g., SQLiteDatabase)
    Database *db = /* ... */;
    
    // Open a connection
    DbConnection *conn = call(db, open);
    if (conn == NULL) {
        // handle error
        return 1;
    }
    
    // Execute a query
    String *sql = new(String, "SELECT * FROM users");
    List *results = call(conn, query, sql);
    // Process results (implementation-specific)
    
    // Or use a prepared statement
    String *insert_sql = new(String, "INSERT INTO users (name, age) VALUES (?, ?)");
    DbPreparedStatement *stmt = call(conn, prepare_statement, insert_sql);
    // Bind parameters and execute (via connection methods)
    
    // Cleanup
    REFCDEC(sql);
    REFCDEC(insert_sql);
    if (results) REFCDEC(results);
    if (stmt) REFCDEC(stmt);
    REFCDEC(conn);
    REFCDEC(db);
    
    return 0;
}
```

---

## Implementation Notes

- **Error Handling**: After any method that might fail (open, exec, query, prepare_statement, exec_statement, query_statement), check the connection's `last_error_code` and `last_error_msg` for details.
- **Reference Counting**: 
  - The `Database.open` and `DbConnection.prepare_statement` methods transfer ownership of the returned object to the caller (refcount incremented).
  - The `query` and `query_statement` methods transfer ownership of the returned `List` (and its contents, if they are Objects) to the caller.
  - Error message strings from `last_error_msg` are REFCINC'd and must be REFCDEC'd by the caller.
- **Thread Safety**: The abstract interface does not specify thread safety. Concrete implementations may or may not be thread-safe; consult the specific driver's documentation.
- **Resource Management**: Always close connections and statements when done to release database resources.

---

## See Also

- [CDF SQLite Database Driver](cdf-db-sqlite-api-reference.md) for a concrete implementation of this abstraction.
- [CDF Entity Manager](cdf-db-entity-api-reference.md) for an ORM-like layer built on top of this abstraction.

# Todo List REST API

A todo list REST API example using `cdf-http`, `cdf-json`, and `cdf-db-entity` with SQLite persistence.

## Prerequisites

Requires `libsqlite3-dev` (or equivalent) installed on the system.

## Build

This example is a cdftk-managed project:

```
cdftk build
```

## Run

With in-memory database (data is lost when the server stops):

```
cdftk run
```

With a file-based SQLite database:

```
LD_LIBRARY_PATH="$CDF_HOME/cdf/cdf/0.4/lib:$CDF_HOME/cdf/cdf-json/0.4/lib:$CDF_HOME/cdf/cdf-http/0.4/lib:$CDF_HOME/cdf/cdf-db/0.4/lib:$CDF_HOME/cdf/cdf-db-sqlite/0.4/lib:$CDF_HOME/cdf/cdf-db-entity/0.4/lib" ./build/todo-list-api /path/to/todos.db
```

The server starts on port 19876.

## Endpoints

| Method | Path         | Description    |
|--------|--------------|----------------|
| GET    | /todos       | List all items |
| POST   | /todos       | Create an item |
| GET    | /todos/:id   | Get item by id |
| PATCH  | /todos/:id   | Update an item |

#include "handlers.h"
#include "todo_item.h"

static void set_content_type(HttpResponse * response) {
    call(response, add_header, REFCTMP(new(HttpHeader,
        REFCTMP(new(String, "Content-Type")),
        REFCTMP(new(String, "application/json")))));
}

static void send_error(HttpResponse * response, HttpStatus status, const char * msg) {
    response->status = status;
    String * json = new(String);
    call(json, format, "{\"error\":\"%s\"}", msg);
    call(response, append_content, json);
    REFCDEC(json);
    set_content_type(response);
}

static void handle_create(TodoRequestHandler * handler, HttpResponse * response, String * content) {
    StringInputStream * sis = new(StringInputStream, content);
    JsonObjectBuilderEventsHandler * h = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * p = new(JsonEventsParser, (JsonEventsHandler *)h);
    int result = call(p, parse, (InputStream *)sis);
    REFCDEC(sis);
    REFCDEC(p);

    if (result != CJSON_PARSE_SUCCESS) {
        REFCDEC(h);
        send_error(response, HTTP_STATUS_BAD_REQUEST, "Invalid JSON");
        return;
    }

    JsonObject * obj = h->object;
    REFCINC(obj);
    REFCDEC(h);

    String * key;
    String * title = NULL, * desc = NULL, * due = NULL, * assignee = NULL;

    key = new(String, "title");
    title = call(obj, get_value, key);
    REFCDEC(key);

    key = new(String, "description");
    desc = call(obj, get_value, key);
    REFCDEC(key);

    key = new(String, "due_date");
    due = call(obj, get_value, key);
    REFCDEC(key);

    key = new(String, "assignee");
    assignee = call(obj, get_value, key);
    REFCDEC(key);

    String * e_title = NULL, * e_desc = NULL, * e_due = NULL, * e_ass = NULL;

    if (!title) { e_title = new(String, ""); title = e_title; }
    if (!desc) { e_desc = new(String, ""); desc = e_desc; }
    if (!due) { e_due = new(String, ""); due = e_due; }
    if (!assignee) { e_ass = new(String, ""); assignee = e_ass; }

    String * status = new(String, "pending");
    TodoItem * item = new(TodoItem, 0L, title, desc, due, assignee, status);
    REFCDEC(status);

    if (e_title) REFCDEC(e_title); else REFCDEC(title);
    if (e_desc) REFCDEC(e_desc); else REFCDEC(desc);
    if (e_due) REFCDEC(e_due); else REFCDEC(due);
    if (e_ass) REFCDEC(e_ass); else REFCDEC(assignee);

    call(handler->em, save, (Entity *)item);

    JsonObject * resp_obj = todo_item_to_json(item);
    String * json_str = call(resp_obj, to_string);
    REFCDEC(resp_obj);

    response->status = HTTP_STATUS_CREATED;
    call(response, append_content, json_str);
    REFCDEC(json_str);
    set_content_type(response);
    REFCDEC(item);
    REFCDEC(obj);
}

static void handle_list(TodoRequestHandler * handler, HttpResponse * response) {
    String * sql = new(String, "SELECT * FROM TodoItem;");
    List * results = call(handler->em->conn, query, sql);
    REFCDEC(sql);

    List * items = new(List);
    if (results) {
        for (int i = 0; i < results->length; i++) {
            Map * row = call(results, get, i);
            TodoItem * item = new(TodoItem);
            call((Entity *)item, from_map, row, NULL);
            JsonObject * obj = todo_item_to_json(item);
            call(items, add, obj);
            REFCDEC(obj);
            REFCDEC(item);
            REFCDEC(row);
        }
        REFCDEC(results);
    }

    JsonObject * wrapper = new(JsonObject);
    call(wrapper, put_value, REFCTMP(new(String, "items")), items);
    REFCDEC(items);

    String * json_str = call(wrapper, to_string);
    REFCDEC(wrapper);

    call(response, append_content, json_str);
    REFCDEC(json_str);
    set_content_type(response);
}

static void handle_detail(TodoRequestHandler * handler, HttpResponse * response, long id) {
    TodoItem * item = new(TodoItem, id, NULL, NULL, NULL, NULL, NULL);

    call(handler->em, load, (Entity *)item);

    if (!((Entity *)item)->id || ((Entity *)item)->id->value < 0) {
        REFCDEC(item);
        send_error(response, HTTP_STATUS_NOT_FOUND, "Not found");
        return;
    }

    JsonObject * obj = todo_item_to_json(item);
    String * json_str = call(obj, to_string);
    REFCDEC(obj);

    call(response, append_content, json_str);
    REFCDEC(json_str);
    set_content_type(response);
    REFCDEC(item);
}

static void handle_update(TodoRequestHandler * handler, HttpResponse * response, long id, String * content) {
    StringInputStream * sis = new(StringInputStream, content);
    JsonObjectBuilderEventsHandler * h = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * p = new(JsonEventsParser, (JsonEventsHandler *)h);
    int result = call(p, parse, (InputStream *)sis);
    REFCDEC(sis);
    REFCDEC(p);

    if (result != CJSON_PARSE_SUCCESS) {
        REFCDEC(h);
        send_error(response, HTTP_STATUS_BAD_REQUEST, "Invalid JSON");
        return;
    }

    JsonObject * obj = h->object;
    REFCINC(obj);
    REFCDEC(h);

    TodoItem * item = new(TodoItem, id, NULL, NULL, NULL, NULL, NULL);
    call(handler->em, load, (Entity *)item);

    if (!((Entity *)item)->id || ((Entity *)item)->id->value < 0) {
        REFCDEC(item);
        REFCDEC(obj);
        send_error(response, HTTP_STATUS_NOT_FOUND, "Not found");
        return;
    }

    String * key;
    String * val;

    key = new(String, "title");
    val = call(obj, get_value, key);
    REFCDEC(key);
    if (val) {
        REFCDEC(item->title);
        item->title = val;
        REFCINC(val);
    }

    key = new(String, "description");
    val = call(obj, get_value, key);
    REFCDEC(key);
    if (val) {
        REFCDEC(item->description);
        item->description = val;
        REFCINC(val);
    }

    key = new(String, "due_date");
    val = call(obj, get_value, key);
    REFCDEC(key);
    if (val) {
        REFCDEC(item->due_date);
        item->due_date = val;
        REFCINC(val);
    }

    key = new(String, "assignee");
    val = call(obj, get_value, key);
    REFCDEC(key);
    if (val) {
        REFCDEC(item->assignee);
        item->assignee = val;
        REFCINC(val);
    }

    key = new(String, "status");
    val = call(obj, get_value, key);
    REFCDEC(key);
    if (val) {
        todo_item_set_status(item, val);
        REFCDEC(val);
    }

    List * fields = call((Entity *)item, fields, false);
    String * sql = new(String, "UPDATE TodoItem SET ");
    for (int i = 0; i < fields->length; i++) {
        FieldMetadata * fm = call(fields, get, i);
        Object * fval = *((Object **)((intptr_t)item + fm->offset));
        call(sql, append, fm->name);
        call(sql, append_cstring, "=");
        if (fval) {
            String * sv = call(fval, to_string);
            call(sql, append_char, '\'');
            call(sql, append, sv);
            call(sql, append_char, '\'');
            REFCDEC(sv);
        } else {
            call(sql, append_cstring, "NULL");
        }
        if (i < fields->length - 1)
            call(sql, append_cstring, ", ");
        REFCDEC(fm);
    }
    call(sql, append_cstring, " WHERE id=");
    String * id_str = call(((Entity *)item)->id, to_string);
    call(sql, append, id_str);
    REFCDEC(id_str);
    call(sql, append_cstring, ";");
    REFCDEC(fields);

    bool ok = call(handler->em->conn, exec, sql);
    REFCDEC(sql);

    if (!ok) {
        REFCDEC(item);
        REFCDEC(obj);
        send_error(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Update failed");
        return;
    }

    JsonObject * resp_obj = todo_item_to_json(item);
    String * json_str = call(resp_obj, to_string);
    REFCDEC(resp_obj);

    call(response, append_content, json_str);
    REFCDEC(json_str);
    set_content_type(response);
    REFCDEC(item);
    REFCDEC(obj);
}

void TodoRequestHandler_handle(void * _this, HttpRequest * request, HttpResponse * response) {
    TodoRequestHandler * handler = (TodoRequestHandler *)_this;
    if (call(request->path, equals_cstring, "/todos")) {
        if (request->method == HTTP_METHOD_POST) {
            handle_create(handler, response, request->content);
            return;
        }
        if (request->method == HTTP_METHOD_GET) {
            handle_list(handler, response);
            return;
        }
        response->status = HTTP_STATUS_METHOD_NOT_ALLOWED;
        return;
    }

    if (request->path->length > 7 && call(request->path, index_of_cstring, "/todos/") == 0) {
        String *id_str = call(request->path, substring_from, 7);
        Long *id_long = new(Long, 0L);
        if (call(id_long, from_string, id_str)) {
            long id = id_long->value;
            REFCDEC(id_str);
            REFCDEC(id_long);
            if (request->method == HTTP_METHOD_GET) {
                handle_detail(handler, response, id);
                return;
            }
            if (request->method == HTTP_METHOD_PATCH) {
                handle_update(handler, response, id, request->content);
                return;
            }
            response->status = HTTP_STATUS_METHOD_NOT_ALLOWED;
            return;
        }
        REFCDEC(id_str);
        REFCDEC(id_long);
    }

    response->status = HTTP_STATUS_NOT_FOUND;
}

void TodoRequestHandler_delete(ObjectPtr _this) {
    make_this(TodoRequestHandler, _this);
    REFCDEC(this->em);
    super_delete(HttpRequestHandler, _this);
}

TodoRequestHandler * TodoRequestHandler_new(TodoRequestHandler * this) {
    super(HttpRequestHandler, TodoRequestHandler);
    override(HttpRequestHandler, handle, TodoRequestHandler_handle);
    return this;
}

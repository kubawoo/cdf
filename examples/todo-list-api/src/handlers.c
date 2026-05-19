#include "handlers.h"
#include "todo_item.h"
#include <string.h>
#include <stdio.h>

static List * todos = NULL;
static long next_id = 1;

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

static void handle_create(HttpResponse * response, String * content) {
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

    key = new(String, "title");
    String * title = call(obj, get_value, key);
    REFCDEC(key);

    key = new(String, "description");
    String * desc = call(obj, get_value, key);
    REFCDEC(key);

    key = new(String, "due_date");
    String * due = call(obj, get_value, key);
    REFCDEC(key);

    key = new(String, "assignee");
    String * assignee = call(obj, get_value, key);
    REFCDEC(key);

    String * e_title = NULL, * e_desc = NULL, * e_due = NULL, * e_ass = NULL;

    if (!title) { e_title = new(String, ""); title = e_title; }
    if (!desc) { e_desc = new(String, ""); desc = e_desc; }
    if (!due) { e_due = new(String, ""); due = e_due; }
    if (!assignee) { e_ass = new(String, ""); assignee = e_ass; }

    String * status = new(String, "pending");
    TodoItem * item = new(TodoItem, next_id++, title, desc, due, assignee, status);
    REFCDEC(status);

    if (e_title) REFCDEC(e_title); else REFCDEC(title);
    if (e_desc) REFCDEC(e_desc); else REFCDEC(desc);
    if (e_due) REFCDEC(e_due); else REFCDEC(due);
    if (e_ass) REFCDEC(e_ass); else REFCDEC(assignee);

    if (!todos) todos = new(List);
    call(todos, add, item);
    REFCDEC(item);

    JsonObject * resp_obj = todo_item_to_json(item);
    String * json_str = call(resp_obj, to_string);
    REFCDEC(resp_obj);

    response->status = HTTP_STATUS_CREATED;
    call(response, append_content, json_str);
    REFCDEC(json_str);
    set_content_type(response);
    REFCDEC(obj);
}

static void handle_list(HttpResponse * response) {
    List * arr = new(List);

    if (todos) {
        for (int i = 0; i < todos->length; i++) {
            TodoItem * item = call(todos, get, i);
            JsonObject * obj = todo_item_to_json(item);
            call(arr, add, obj);
            REFCDEC(obj);
            REFCDEC(item);
        }
    }

    JsonObject * wrapper = new(JsonObject);
    call(wrapper, put_value, REFCTMP(new(String, "items")), arr);
    REFCDEC(arr);

    String * json_str = call(wrapper, to_string);
    REFCDEC(wrapper);

    call(response, append_content, json_str);
    REFCDEC(json_str);
    set_content_type(response);
}

static void handle_detail(HttpResponse * response, long id) {
    if (!todos) {
        send_error(response, HTTP_STATUS_NOT_FOUND, "Not found");
        return;
    }

    for (int i = 0; i < todos->length; i++) {
        TodoItem * item = call(todos, get, i);
        if (item->id == id) {
            REFCDEC(item);
            item = call(todos, get, i);
            JsonObject * obj = todo_item_to_json(item);
            REFCDEC(item);
            String * json_str = call(obj, to_string);
            REFCDEC(obj);
            call(response, append_content, json_str);
            REFCDEC(json_str);
            set_content_type(response);
            return;
        }
        REFCDEC(item);
    }

    send_error(response, HTTP_STATUS_NOT_FOUND, "Not found");
}

static void handle_update(HttpResponse * response, long id, String * content) {
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

    if (!todos) {
        REFCDEC(obj);
        send_error(response, HTTP_STATUS_NOT_FOUND, "Not found");
        return;
    }

    int found = 0;
    for (int i = 0; i < todos->length; i++) {
        TodoItem * item = call(todos, get, i);
        if (item->id == id) {
            String * key;
            String * val;

            key = new(String, "title");
            val = call(obj, get_value, key);
            REFCDEC(key);
            if (val) {
                REFCDEC(item->title);
                item->title = val;
            }

            key = new(String, "description");
            val = call(obj, get_value, key);
            REFCDEC(key);
            if (val) {
                REFCDEC(item->description);
                item->description = val;
            }

            key = new(String, "due_date");
            val = call(obj, get_value, key);
            REFCDEC(key);
            if (val) {
                REFCDEC(item->due_date);
                item->due_date = val;
            }

            key = new(String, "assignee");
            val = call(obj, get_value, key);
            REFCDEC(key);
            if (val) {
                REFCDEC(item->assignee);
                item->assignee = val;
            }

            key = new(String, "status");
            val = call(obj, get_value, key);
            REFCDEC(key);
            if (val) {
                todo_item_set_status(item, val);
                REFCDEC(val);
            }

            REFCDEC(item);

            item = call(todos, get, i);
            JsonObject * resp_obj = todo_item_to_json(item);
            REFCDEC(item);
            String * json_str = call(resp_obj, to_string);
            REFCDEC(resp_obj);
            call(response, append_content, json_str);
            REFCDEC(json_str);
            set_content_type(response);

            found = 1;
            break;
        }
        REFCDEC(item);
    }

    REFCDEC(obj);

    if (!found)
        send_error(response, HTTP_STATUS_NOT_FOUND, "Not found");
}

void TodoRequestHandler_handle(void * _this, HttpRequest * request, HttpResponse * response) {
    const char * path = call(request->path, to_cstring);

    if (strcmp(path, "/todos") == 0) {
        if (request->method == HTTP_METHOD_POST) {
            handle_create(response, request->content);
            return;
        }
        if (request->method == HTTP_METHOD_GET) {
            handle_list(response);
            return;
        }
        response->status = HTTP_STATUS_METHOD_NOT_ALLOWED;
        return;
    }

    long id;
    if (sscanf(path, "/todos/%ld", &id) == 1) {
        if (request->method == HTTP_METHOD_GET) {
            handle_detail(response, id);
            return;
        }
        if (request->method == HTTP_METHOD_PATCH) {
            handle_update(response, id, request->content);
            return;
        }
        response->status = HTTP_STATUS_METHOD_NOT_ALLOWED;
        return;
    }

    response->status = HTTP_STATUS_NOT_FOUND;
}

void TodoRequestHandler_delete(ObjectPtr _this) {
    super_delete(HttpRequestHandler, _this);
}

TodoRequestHandler * TodoRequestHandler_new(TodoRequestHandler * this) {
    super(HttpRequestHandler, TodoRequestHandler);
    override(HttpRequestHandler, handle, TodoRequestHandler_handle);
    return this;
}

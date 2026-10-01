#include "chat.h"
#include <stdio.h>

ChatTurn * ChatTurn_new2(ChatTurn * this, String * role, String * content) {
    super(Object, ChatTurn);
    REFCINC(role);
    REFCINC(content);
    this->role = role;
    this->content = content;
    return this;
}

void ChatTurn_delete(ObjectPtr _this) {
    make_this(ChatTurn, _this);
    REFCDEC(this->role);
    REFCDEC(this->content);
    super_delete(Object, _this);
}

ChatClient * new_chat_client(const char * base_url, const char * model, const char * token, bool thinking) {
    ChatClient * chat = malloc(sizeof(ChatClient));
    chat->base_url = new(String, base_url);
    chat->model = new(String, model);
    /* NULL rather than an empty string, so "no token given" stays
       distinguishable from "the token is empty" when the header is built. */
    chat->token = token == NULL ? NULL : new(String, token);
    chat->turns = new(List);
    chat->client = new(HttpClient);
    chat->thinking = thinking;

    LoggerFactory * lf = singleton(LoggerFactory);
    chat->logger = call(lf, get_logger_cstring, "chatter");

    return chat;
}

/* Attaches the bearer token when one was configured. A server that needs no
   authentication rejects an empty Authorization header as firmly as a wrong
   one, so nothing is sent at all when no token was given. */
static void _add_auth_header(ChatClient * chat, HttpRequest * request) {
    if(chat->token == NULL) {
        return;
    }
    String * value = new(String, "Bearer ");
    call(value, append, chat->token);
    call(request, add_header, REFCTMP(new(HttpHeader,
        REFCTMP(new(String, "Authorization")),
        value)));
    REFCDEC(value);
}

/* Joins the base URL and a path with exactly one slash between them, so both
   "http://host:8080" and "http://host:8080/" give a well-formed URL. */
static String * _endpoint(ChatClient * chat, const char * path) {
    String * url = new(String);
    call(url, append, chat->base_url);
    if(url->length > 0 && call(url, char_at, (int) url->length - 1) != '/') {
        call(url, append_char, '/');
    }
    call(url, append_cstring, path);
    return url;
}

/* Checks a response and reports why it was unusable. Returns false when the
   caller should stop; the reason is left in err. */
static bool _check_response(ChatClient * chat, HttpResponse * response, String * err) {
    if(response == NULL) {
        call(err, clear);
        String * detail = chat_describe_failure(call(chat->base_url, to_cstring));
        call(err, append, detail);
        REFCDEC(detail);
        return false;
    }

    if(response->status != HTTP_STATUS_OK) {
        call(err, clear);
        call(err, format, "Server returned HTTP %d.\n%s", (int) response->status,
             response->content->length > 0 ? call(response->content, to_cstring) : "(no body)");
        return false;
    }
    return true;
}

/* Parses a JSON body into a freshly built handler the caller must release.
   Returns NULL when the body did not parse. */
static JsonObjectBuilderEventsHandler * _parse_body(HttpResponse * response) {
    StringInputStream * stream = new(StringInputStream, response->content);
    JsonObjectBuilderEventsHandler * handler = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    int rc = call(parser, parse, (InputStream *) stream);
    REFCDEC(parser);
    REFCDEC(stream);
    if(rc != CJSON_PARSE_SUCCESS) {
        REFCDEC(handler);
        return NULL;
    }
    return handler;
}

/* With thinking off the model still wraps its answer in empty <think> tags,
   which are an artefact of the chat template rather than part of the reply. */
static String * _strip_think_tags(const char * text) {
    if(strstr(text, "<think>") == NULL && strstr(text, "</think>") == NULL) {
        return new(String, text);
    }

    String * out = new(String);
    const char * p = text;
    while(*p != '\0') {
        if(strncmp(p, "<think>", 7) == 0) {
            const char * end = strstr(p, "</think>");
            if(end != NULL) {
                p = end + 8;      /* strlen("</think>") */
                continue;
            }
        }
        call(out, append_char, *p);
        p++;
    }
    return out;
}

void chat_add_turn(ChatClient * chat, const char * role, const char * content) {
    ChatTurn * turn = new(ChatTurn, new(String, role), new(String, content));
    call(chat->turns, add, turn);
    REFCDEC(turn);
}

void chat_clear(ChatClient * chat) {
    /* The list owns its turns and releases them when it is dropped. List has
       no clear(), so the list itself is replaced. */
    REFCDEC(chat->turns);
    chat->turns = new(List);
}

String * chat_build_request_body(ChatClient * chat, int max_tokens) {
    List * messages = new(List);
    for(int i = 0; i < call(chat->turns, size); ++i) {
        ChatTurn * turn = (ChatTurn *) call(chat->turns, get, i);
        JsonObject * message = new(JsonObject);
        call(message, put_value, REFCTMP(new(String, "role")), turn->role);
        call(message, put_value, REFCTMP(new(String, "content")), turn->content);
        call(messages, add, message);
        REFCDEC(message);
        REFCDEC(turn);
    }

    JsonObject * body = new(JsonObject);
    call(body, put_value, REFCTMP(new(String, "model")), chat->model);
    call(body, put_value, REFCTMP(new(String, "messages")), messages);
    call(body, put_value, REFCTMP(new(String, "max_tokens")), REFCTMP(new(Integer, max_tokens)));
    /* The reply is not streamed, so the response arrives in one piece. */
    call(body, put_value, REFCTMP(new(String, "stream")), REFCTMP(new(Boolean, false)));

    /* A chat interface wants the answer, not the working. Reasoning models
       default to thinking out loud, and on this server a short greeting cost
       more than a thousand tokens of deliberation — minutes of waiting. The
       flag is passed through to the chat template by llama.cpp; a server that
       ignores it simply thinks as before. */
    JsonObject * template_kwargs = new(JsonObject);
    call(template_kwargs, put_value, REFCTMP(new(String, "enable_thinking")), REFCTMP(new(Boolean, chat->thinking)));
    call(body, put_value, REFCTMP(new(String, "chat_template_kwargs")), template_kwargs);
    REFCDEC(template_kwargs);

    String * json = call(body, to_string);
    REFCDEC(body);
    REFCDEC(messages);
    return json;
}

String * chat_describe_failure(const char * url) {
    String * msg = new(String);
    call(msg, format, "No response from %s.\nIs the server running, and is %s reachable from this machine?",
         url, url);
    return msg;
}

/* Returns the string at key when it is a non-empty String, otherwise NULL. */
static String * _first_non_empty_string(JsonObject * obj, const char * key) {
    String * k = new(String, key);
    Object * value = call(obj, get_value, k);
    REFCDEC(k);
    String * result = NULL;
    if(value != NULL && type_equal(value, "String")) {
        String * text = (String *) value;
        if(text->length > 0) {
            result = text;
            REFCINC(result);
        }
    }
    REFCDEC(value);
    return result;
}

/* Pulls choices[0].message.content out of a completion response. A reasoning
   model that runs out of budget mid-thought returns an empty content with its
   reasoning in reasoning_content; showing that beats showing nothing. */
static String * _extract_reply(JsonObject * root) {
    String * key = new(String, "choices");
    Object * choices = call(root, get_value, key);
    REFCDEC(key);
    if(choices == NULL || !type_equal(choices, "List")) {
        REFCDEC(choices);
        return NULL;
    }

    List * list = (List *) choices;
    if(call(list, size) == 0) {
        REFCDEC(list);
        return NULL;
    }

    JsonObject * first = (JsonObject *) call(list, get, 0);
    String * reply = NULL;
    if(first != NULL && type_equal(first, "JsonObject")) {
        String * mk = new(String, "message");
        Object * message = call(first, get_value, mk);
        REFCDEC(mk);
        if(message != NULL && type_equal(message, "JsonObject")) {
            reply = _first_non_empty_string((JsonObject *) message, "content");
            if(reply == NULL) {
                reply = _first_non_empty_string((JsonObject *) message, "reasoning_content");
            }
        }
        REFCDEC(message);
    }
    REFCDEC(first);
    REFCDEC(list);
    return reply;
}

/* Reasoning models can return their reasoning with an empty content when they
   run out of budget mid-thought. Say so instead of showing a blank message. */
static String * _explain_empty_content(ChatClient * chat) {
    String * msg = new(String);
    call(msg, append_cstring,
         "The model returned no text. It may have spent the whole reply on reasoning;\ntry again, or ask for a shorter answer.");
    return msg;
}

String * chat_send(ChatClient * chat, int max_tokens, String * err) {
    String * body = chat_build_request_body(chat, max_tokens);

    String * url = _endpoint(chat, "v1/chat/completions");

    HttpRequest * request = new(HttpRequest, HTTP_METHOD_POST, url);
    call(request, add_header, REFCTMP(new(HttpHeader,
        REFCTMP(new(String, "Content-Type")),
        REFCTMP(new(String, "application/json")))));
    _add_auth_header(chat, request);
    call(request, append_content, body);

    String * trace = new(String);
    call(trace, format, "POST %s (%zu bytes of history)", call(url, to_cstring),
         call(chat->turns, size));
    call(chat->logger, log, LOG_LEVEL_DEBUG, log_msg(trace));
    REFCDEC(trace);

    HttpResponse * response = call(chat->client, send_request, request);

    REFCDEC(request);
    REFCDEC(url);
    REFCDEC(body);

    /* The body is parsed before the response is released: _parse_body reads
       response->content, so the response has to outlive it. */
    JsonObjectBuilderEventsHandler * handler = NULL;
    bool ok = _check_response(chat, response, err);
    if(ok) {
        handler = _parse_body(response);
    }
    REFCDEC(response);

    if(!ok) {
        return NULL;
    }

    if(handler == NULL) {
        call(err, clear);
        call(err, append_cstring, "The reply was not valid JSON.");
        return NULL;
    }

    JsonObject * root = call(handler, get_object);
    REFCDEC(handler);

    String * reply = root == NULL ? NULL : _extract_reply(root);
    REFCDEC(root);

    if(reply == NULL) {
        return _explain_empty_content(chat);
    }

    String * stripped = _strip_think_tags(call(reply, to_cstring));
    REFCDEC(reply);
    return stripped;
}

/* Each entry is an object with an "id" naming the model; anything without one is
   skipped rather than shown as a blank row, so a server that adds a different
   kind of entry does not break the list. */
static List * _collect_model_ids(List * entries) {
    List * models = new(List);
    for(int i = 0; i < call(entries, size); ++i) {
        Object * entry = call(entries, get, i);
        if(entry != NULL && type_equal(entry, "JsonObject")) {
            String * id = _first_non_empty_string((JsonObject *) entry, "id");
            if(id != NULL) {
                call(models, add, id);
                REFCDEC(id);
            }
        }
        REFCDEC(entry);
    }
    return models;
}

List * chat_list_models(ChatClient * chat, String * err) {
    String * url = _endpoint(chat, "v1/models");

    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, url);
    _add_auth_header(chat, request);

    HttpResponse * response = call(chat->client, send_request, request);

    REFCDEC(request);

    /* Parsed before the response is released, since the body lives in it. */
    JsonObjectBuilderEventsHandler * handler = NULL;
    bool ok = _check_response(chat, response, err);
    if(ok) {
        handler = _parse_body(response);
    }
    REFCDEC(response);
    REFCDEC(url);

    if(!ok) {
        return NULL;
    }

    if(handler == NULL) {
        call(err, clear);
        call(err, append_cstring, "The model list was not valid JSON.");
        return NULL;
    }

    /* Some servers wrap the array in {"data":[...]}, others answer with a bare
       array, so both roots are accepted. */
    List * entries = call(handler, get_list);
    if(entries == NULL) {
        JsonObject * root = call(handler, get_object);
        String * key = new(String, "data");
        Object * data = root == NULL ? NULL : call(root, get_value, key);
        REFCDEC(key);
        if(data != NULL && type_equal(data, "List")) {
            entries = (List *) data;
        } else {
            REFCDEC(data);
        }
        REFCDEC(root);
    }

    List * models = entries == NULL ? new(List) : _collect_model_ids(entries);
    REFCDEC(entries);
    REFCDEC(handler);
    return models;
}

void chat_client_delete(ChatClient * chat) {
    if(!chat) return;
    REFCDEC(chat->turns);
    REFCDEC(chat->token);
    REFCDEC(chat->model);
    REFCDEC(chat->base_url);
    REFCDEC(chat->client);
    /* The logger is owned by the LoggerFactory, which caches it for the rest of
       the process; get_logger_cstring only handed out a borrowed reference. */
    REFCDEC(chat->logger);
    free(chat);
}
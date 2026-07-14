#include "eventparser.h"
#include <stdlib.h>
#include <ctype.h>

void JsonEventsHandler_delete(ObjectPtr _this) {
    //empty destructor
    super_delete(Object, _this);
}

JsonEventsHandler * JsonEventsHandler_new(JsonEventsHandler * this) {
    super(Object, JsonEventsHandler);
    this->value = NULL;
    this->object_begin = NULL;
    this->object_end = NULL;
    this->array_begin = NULL;
    this->array_end = NULL;
    return this;
}

typedef enum {
    IDLE,
    IN_OBJECT,
    IN_ARRAY,
    IN_NAME,
    NAME_DONE,
    READY_FOR_VALUE,
    IN_VALUE,
} _State;

typedef enum {
    BOOLEAN,
    DOUBLE,
    INTEGER,
    LONG,
    STRING,
    ARRAY,
    OBJECT
} _Type;


int _parse(ObjectPtr _this, InputStream * json_stream, bool ignore_trailing_characters);
_Type _guessType(String * s);
Object * _getValue(String * s, _Type type);


int _JsonEventsParser_parse(ObjectPtr _this, InputStream * json_stream) {
    make_this(JsonEventsParser, _this);
    if(this->_handler == NULL) {
        return CJSON_PARSE_NO_HANDLER;
    }

    if(json_stream == NULL) {
        return CJSON_PARSE_NO_INPUT;
    }

    return _parse(this, json_stream, false);
}


JsonEventsParser * JsonEventsParser_new1(JsonEventsParser * this, JsonEventsHandler * handler) {
    super(Object, JsonEventsParser);
    REFCINC(handler);
    this->_handler = handler;
    this->parse = _JsonEventsParser_parse;
    this->_buffer = new(String);
    this->_name = new(String);
    this->_value = new(String);
    this->_state_depth = 0;
    this->_states[this->_state_depth++] = IDLE;
    this->_read_pos = 0;
    this->_read_end = 0;
    return this;
}


void JsonEventsParser_delete(ObjectPtr _this) {
    make_this(JsonEventsParser, _this);
    REFCDEC(this->_name);
    REFCDEC(this->_buffer);
    REFCDEC(this->_value);
    REFCDEC(this->_handler);
    super_delete(Object, _this);
}

static void push_state(JsonEventsParser * this, int state) {
    if (this->_state_depth < CJSON_MAX_DEPTH) {
        this->_states[this->_state_depth++] = state;
    }
}

static int peek_state(JsonEventsParser * this) {
    return this->_state_depth > 0 ? this->_states[this->_state_depth - 1] : IDLE;
}

static void pop_state(JsonEventsParser * this) {
    if (this->_state_depth > 0) {
        this->_state_depth--;
    }
}

int _processIdle(ObjectPtr _this, char c) {
    make_this(JsonEventsParser, _this);
    if(isspace(c)) {
        return CJSON_PARSE_SUCCESS;
    }
    if(c == '{') {
        push_state(this, IN_OBJECT);
        if(this->_handler->object_begin != NULL) {
            call(this->_handler, object_begin, NULL);
        }
        return CJSON_PARSE_SUCCESS;
    } else if(c == '[') {
        push_state(this, IN_ARRAY);
        if(this->_handler->array_begin != NULL) {
            call(this->_handler, array_begin, NULL);
        }
        return CJSON_PARSE_SUCCESS;
    }
    return CJSON_PARSE_INVALID_JSON;
}


int _processInObject(ObjectPtr _this, char c) {
    make_this(JsonEventsParser, _this);
    if(isspace(c)) {
        return CJSON_PARSE_SUCCESS;
    }

    if(c == '}') {
        pop_state(this);
        if(this->_handler->object_end != NULL) {
            call(this->_handler, object_end);
        }
    } else if(c == '"') {
        push_state(this, IN_NAME);
    } else if(c == ',') {
        return CJSON_PARSE_SUCCESS;
    } else {
        return CJSON_PARSE_INVALID_JSON;
    }

    return CJSON_PARSE_SUCCESS;
}

int _processInName(ObjectPtr _this, char c) {
    make_this(JsonEventsParser, _this);
    if(c=='"') {
        pop_state(this);
        if(peek_state(this) == IN_ARRAY) {
            Object * value = (Object *) new(String, call(this->_buffer, to_cstring));
            if(this->_handler->value != NULL) {
                String * name = call(this->_name, copy);
                call(this->_handler, value, name, value);
                REFCDEC(name);
            }
            REFCDEC(value);
            call(this->_name, clear);
            call(this->_buffer, clear);
        } else {
            push_state(this, NAME_DONE);
            call(this->_name, set_text, call(this->_buffer, to_cstring));
            call(this->_buffer, clear);
        }
    } else {
        call(this->_buffer, append_char, c);
    }
    return CJSON_PARSE_SUCCESS;
}

int _processNameDone(ObjectPtr _this, char c) {
    make_this(JsonEventsParser, _this);
    if(isspace(c)) {
        return CJSON_PARSE_SUCCESS;
    }

    if(c == ':') {
        pop_state(this);
        push_state(this, READY_FOR_VALUE);
        return CJSON_PARSE_SUCCESS;
    }

    return CJSON_PARSE_INVALID_JSON;
}

int _processReadyForValue(ObjectPtr _this, char c) {
    make_this(JsonEventsParser, _this);
    if(isspace(c)) {
        return CJSON_PARSE_SUCCESS;
    }

    if(c == '{') {
        pop_state(this); // READY_FOR_VALUE
        push_state(this, IN_OBJECT);
        if(this->_handler->object_begin != NULL) {
            String * name = call(this->_name, copy);
            call(this->_handler, object_begin, name);
            REFCDEC(name);
        }
        call(this->_name, clear);
        return CJSON_PARSE_SUCCESS;
    } else if(c == '[') {
        pop_state(this); // READY_FOR_VALUE
        push_state(this, IN_ARRAY);
        if(this->_handler->array_begin != NULL) {
            String * name = call(this->_name, copy);
            call(this->_handler, array_begin, name);
            REFCDEC(name);
        }
        call(this->_name, clear);
        return CJSON_PARSE_SUCCESS;
    }

    call(this->_buffer, append_char, c);
    pop_state(this); // READY_FOR_VALUE
    push_state(this, IN_VALUE);
    return CJSON_PARSE_SUCCESS;
}

int _processInValue(ObjectPtr _this, char c) {
    make_this(JsonEventsParser, _this);

    if(c == ',' || c == '}') {
        if(c == ',' && this->_buffer->length > 0) {
            const char * buf = call(this->_buffer, to_cstring);
            if(buf[0] == '"') {
                int quote_count = 0;
                for(int i = 0; i < this->_buffer->length; i++) {
                    if(buf[i] == '"') quote_count++;
                }
                if(quote_count % 2 == 1) {
                    call(this->_buffer, append_char, c);
                    return CJSON_PARSE_SUCCESS;
                }
            }
        }
        call(this->_buffer, trim);
        _Type type = _guessType(this->_buffer);
        if(type < 0) {
            return CJSON_PARSE_INVALID_VALUE;
        }
        Object * value = _getValue(this->_buffer, type);
        if(this->_handler->value != NULL) {
            String * name = call(this->_name, copy);
            call(this->_handler, value, name , value);
            REFCDEC(name);
        }
        REFCDEC(value);

        pop_state(this); // IN_VALUE
        if(c == '}') {
            pop_state(this); // IN_OBJECT
            if(this->_handler->object_end != NULL) {
                call(this->_handler, object_end);
            }
        }
        call(this->_name, clear);
        call(this->_buffer, clear);
        return CJSON_PARSE_SUCCESS;
    }

    call(this->_buffer, append_char, c);
    return CJSON_PARSE_SUCCESS;
}

int _processInArray(ObjectPtr _this, char c)
{
    make_this(JsonEventsParser, _this);
    if(isspace(c)) {
        return CJSON_PARSE_SUCCESS;
    }

    if(c == '"') {
        push_state(this, IN_NAME);
        return CJSON_PARSE_SUCCESS;
    } else if(c == '{') {
        if(this->_handler->object_begin != NULL) {
            call(this->_handler, object_begin, NULL);
        }
        push_state(this, IN_OBJECT);
        return CJSON_PARSE_SUCCESS;
    } else if (c == ',') {
        return CJSON_PARSE_SUCCESS;
    } else if (c == ']') {
        pop_state(this);
        if(this->_handler->array_end != NULL) {
            call(this->_handler, array_end);
        }
        return CJSON_PARSE_SUCCESS;
    }

    return CJSON_PARSE_INVALID_JSON;
}

_Type _guessType(String * s)
{
    const char * value = call(s, to_cstring);
    if(strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
        return BOOLEAN;
    }

    if(value[0] == '"' && value[strlen(value) - 1] == '"') {
        return STRING;
    }

    if(strchr(value, '.') != NULL) {
        return DOUBLE;
    }

    return LONG;
}

Object * _getValue(String * s, _Type type) {
    Object * p = NULL;
    switch(type) {
        case INTEGER: {
            Integer * i = new(Integer);
            call(i, from_string, s);
            p = (Object *)i;
            break;
        }
        case DOUBLE: {
            Double * d = new(Double);
            call(d, from_string, s);
            p = (Object *)d;
            break;
        }
        case STRING: {
            p = (Object *)  call(s, substring, 1, s->length - 1);
            break;
        }
        case BOOLEAN: {
            Boolean * b = new(Boolean);
            call(b, from_string, s);
            p = (Object *)b;
            break;
        }
        case LONG: {
            Long * i = new(Long);
            call(i, from_string, s);
            p = (Object *)i;
            break;
        }
        default:
            break;
    }
    return p;
}


int _parse(ObjectPtr _this, InputStream * json_stream, bool ignore_trailing_characters) {
    make_this(JsonEventsParser, _this);
    while(true) {
        if (this->_read_pos >= this->_read_end) {
            this->_read_pos = 0;
            this->_read_end = 0;
            while (this->_read_end < CJSON_BUFFER_SIZE) {
                int i = call(json_stream, read);
                if (i < 0) break;
                this->_read_buf[this->_read_end++] = (char)i;
            }
            if (this->_read_end == 0) break;
        }

        char c = this->_read_buf[this->_read_pos++];
        int ret = CJSON_PARSE_SUCCESS;

        switch(peek_state(this)) {
            case IDLE:
                ret = _processIdle(this, c);
                break;
            case IN_OBJECT:
                ret = _processInObject(this, c);
                break;
            case IN_ARRAY:
                ret = _processInArray(this, c);
                break;
            case IN_NAME:
                ret = _processInName(this, c);
                break;
            case NAME_DONE:
                ret = _processNameDone(this, c);
                break;
            case READY_FOR_VALUE:
                ret = _processReadyForValue(this, c);
                break;
            case IN_VALUE:
                ret = _processInValue(this, c);
                break;
        }

        if(ret != CJSON_PARSE_SUCCESS) {
            return ret;
        }

        //end of parsing
        if(peek_state(this) == IDLE) {
            if(ignore_trailing_characters) {
                return CJSON_PARSE_SUCCESS;
            } else {
                _parse(this, json_stream, true);
            }
        }
    }

    if(peek_state(this) != IDLE) {
        return CJSON_PARSE_INVALID_JSON;
    }

    return CJSON_PARSE_SUCCESS;
}



#include "parserhandlers.h"

bool _in_array(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    Object * o = call(this->_stack, peek);
    bool ret = false;
    if(type_equal(o, "List")) {
        ret = true;
    }
    REFCDEC(o);
    return ret;
}

void _obeh_object_begin(ObjectPtr _this, String * name) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    if(name == NULL) {
        if(call(this->_stack, size) == 0) {
            // root object
            this->object = new(JsonObject);
            call(this->_stack, push, this->object);
        } else if (_in_array(this)) {
            // in array
            List * list = call(this->_stack, peek);
            JsonObject * jo = REFCTMP(new(JsonObject));
            call(list, add, jo);
            call(this->_stack, push, jo);
            REFCDEC(list);
        } else {
            //TODO: error
        }
    } else {
        JsonObject * obj = REFCTMP(new(JsonObject));
        JsonObject * jo = call(this->_stack, peek);
        call(jo, put_value, name, obj);
        call(this->_stack, push, obj);
        REFCDEC(jo);
    }
}

void _obeh_object_end(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    JsonObject * obj = call(this->_stack, pop);
    REFCDEC(obj);
}

void _obeh_array_begin(ObjectPtr _this, String * name) {
    make_this(JsonObjectBuilderEventsHandler, _this);

    List * list = REFCTMP(new(List));
    JsonObject * jo = call(this->_stack, peek);
    call(jo, put_value, name, list);
    call(this->_stack, push, list);
    REFCDEC(jo);
}

void _obeh_array_end(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    List * list = call(this->_stack, pop);
    REFCDEC(list);
}

void _obeh_value(ObjectPtr _this, String * name, Object * value) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    String * vs = call(value, to_string);
    REFCDEC(vs);

    if(_in_array(this)) {
        List * list = call(this->_stack, peek);
        call(list, add, value);
        REFCDEC(list);
    } else {
        JsonObject * jo = call(this->_stack, peek);
        call(jo, put_value, name, value);
        REFCDEC(jo);
    }
}

JsonObjectBuilderEventsHandler * JsonObjectBuilderEventsHandler_new(JsonObjectBuilderEventsHandler * this) {
    super(JsonEventsHandler, JsonObjectBuilderEventsHandler);
    override(JsonEventsHandler, object_begin, _obeh_object_begin);
    override(JsonEventsHandler, object_end, _obeh_object_end);
    override(JsonEventsHandler, array_begin, _obeh_array_begin);
    override(JsonEventsHandler, array_end, _obeh_array_end);
    override(JsonEventsHandler, value, _obeh_value);

    this->_stack = new(Stack);
    return this;
}

void JsonObjectBuilderEventsHandler_delete(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    REFCDEC(this->_stack);
    REFCDEC(this->object);
    super_delete(JsonEventsHandler, _this);
}

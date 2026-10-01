#include "parserhandlers.h"

static bool _in_array(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    Object * o = call(this->_stack, peek);
    bool ret = false;
    if(type_equal(o, "List")) {
        ret = true;
    }
    REFCDEC(o);
    return ret;
}

static void _obeh_object_begin(ObjectPtr _this, String * name) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    if(name == NULL) {
        if(call(this->_stack, size) == 0) {
            // root object
            this->_object = new(JsonObject);
            call(this->_stack, push, this->_object);
        } else if (_in_array(this)) {
            // in array
            List * list = call(this->_stack, peek);
            /* REFCTMP hands the count to the two owners below: the list itself
               and the stack, which gives its reference back at object_end. */
            JsonObject * jo = REFCTMP(new(JsonObject));
            call(list, add, jo);
            call(this->_stack, push, jo);
            REFCDEC(list);
        } else {
            //TODO: error
        }
    } else {
        /* Same split here — put_value takes one reference, the stack one. */
        JsonObject * obj = REFCTMP(new(JsonObject));
        JsonObject * jo = call(this->_stack, peek);
        call(jo, put_value, name, obj);
        call(this->_stack, push, obj);
        REFCDEC(jo);
    }
}

static void _obeh_object_end(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    JsonObject * obj = call(this->_stack, pop);
    REFCDEC(obj);
}

/* A document may be a bare array rather than an object — /v1/models returns one.
   There is no name to hang it off and no parent object to put it in, so the
   array is kept as the root in its own right; get_list() hands it back. */
static void _obeh_array_begin(ObjectPtr _this, String * name) {
    make_this(JsonObjectBuilderEventsHandler, _this);

    if(name == NULL && call(this->_stack, size) == 0) {
        /* Root array. Two owners: the handler, which hands the list back from
           get_list(), and the stack, which drops its reference at array_end.
           Leaving it to the stack alone would free the root list there and
           leave the handler pointing at recycled memory. */
        this->_list = new(List);
        call(this->_stack, push, this->_list);
        return;
    }

    /* A nested array is owned by its parent object and by the stack. The stack
       is empty when there is no parent to hand it to — a top-level array, which
       the branch above already took. */
    List * list = REFCTMP(new(List));
    JsonObject * jo = call(this->_stack, peek);
    if(jo == NULL) {
        REFCDEC(list);
        return;
    }
    call(jo, put_value, name, list);
    call(this->_stack, push, list);
    REFCDEC(jo);
}

static void _obeh_array_end(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    List * list = call(this->_stack, pop);
    REFCDEC(list);
}

static void _obeh_value(ObjectPtr _this, String * name, Object * value) {
    make_this(JsonObjectBuilderEventsHandler, _this);

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

static JsonObject * _get_object(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    JsonObject * obj = this->_object;
    REFCINC(obj);
    return obj;
}

/* Non-NULL only when the document was a top-level array. */
static List * _get_list(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    List * list = this->_list;
    REFCINC(list);
    return list;
}

JsonObjectBuilderEventsHandler * JsonObjectBuilderEventsHandler_new(JsonObjectBuilderEventsHandler * this) {
    super(JsonEventsHandler, JsonObjectBuilderEventsHandler);
    override(JsonEventsHandler, object_begin, _obeh_object_begin);
    override(JsonEventsHandler, object_end, _obeh_object_end);
    override(JsonEventsHandler, array_begin, _obeh_array_begin);
    override(JsonEventsHandler, array_end, _obeh_array_end);
    override(JsonEventsHandler, value, _obeh_value);

    this->get_object = _get_object;
    this->get_list = _get_list;

    this->_stack = new(Stack);
    this->_object = nullptr;
    this->_list = nullptr;
    return this;
}

void JsonObjectBuilderEventsHandler_delete(ObjectPtr _this) {
    make_this(JsonObjectBuilderEventsHandler, _this);
    REFCDEC(this->_stack);
    REFCDEC(this->_object);
    REFCDEC(this->_list);
    super_delete(JsonEventsHandler, _this);
}

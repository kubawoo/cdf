#include "ooc_array.h"
#include "ooc_collection.h"
#include <stdio.h>

/* ArrayIterator implementation */
typedef struct _ArrayIterator {
    inherits(Iterator);
    Array * array;
    int index;
} ArrayIterator;

static bool ArrayIterator_has_next(ObjectPtr _this) {
    make_this(ArrayIterator, _this);
    return this->index < this->array->length;
}

static ObjectPtr ArrayIterator_next(ObjectPtr _this) {
    make_this(ArrayIterator, _this);
    if (this->index >= this->array->length) {
        return NULL;
    }
    ObjectPtr result = this->array->_values[this->index];
    REFCINC(result);
    this->index++;
    return result;
}

static void ArrayIterator_delete(ObjectPtr _this) {
    make_this(ArrayIterator, _this);
    REFCDEC(this->array);
    super_delete(Iterator, _this);
}

/* ArrayIterator constructor */
static ArrayIterator * ArrayIterator_new1(ArrayIterator * this) {
    super(Object, ArrayIterator);
    ((Object*)this)->_ooc_destructor = ArrayIterator_delete;
    this->array = NULL;
    this->index = 0;
    return this;
}

/* Array iterator method */
static Iterator * Array_iterator(ObjectPtr _this) {
    make_this(Array, _this);
    ArrayIterator * iter = ArrayIterator_new1(NULL);
    iter->array = this;
    REFCINC(this);
    iter->index = 0;
    
    /* Set up the iterator interface */
    Iterator * iter_iface = (Iterator*)iter;
    iter_iface->has_next = ArrayIterator_has_next;
    iter_iface->next = ArrayIterator_next;
    /* The destructor is already set by the constructor */
    
    return iter_iface;
}

void _check_index(Array * this, int index) {
    if(index < 0 || index >= this->length) {
         fputs("index out of bound", stderr);
         abort();
    }
}

void _set(ObjectPtr _this, int index, ObjectPtr o) {
    make_this(Array, _this);
    _check_index(this, index);
    REFCINC(o);
    this->_values[index] = o;
}


ObjectPtr _get(ObjectPtr _this, int index) {
    make_this(Array, _this);
    _check_index(this, index);
    ObjectPtr o = this->_values[index];
    REFCINC(o);
    return o;
}


Array * Array_new1(Array * this, int size) {
    if(size <= 0) {
        return NULL;
    }
    super(Object, Array);
    override(Collection, iterator, Array_iterator);
    this->length = size;
    this->_values = malloc(size * sizeof(Object *));
    for(int i = 0; i < this->length; ++i) {
        this->_values[i] = NULL;
    }
    this->set = _set;
    this->get = _get;
    return this;
}

void Array_delete(ObjectPtr _this) {
    make_this(Array, _this);
    for(int i = 0; i < this->length; ++i) {
        REFCDEC(this->_values[i]);
    }
    free(this->_values);
    super_delete(Object, this);
}
#include "array.h"
#include <stdio.h>

static void _check_index(Array * this, int index) {
    if(index < 0 || index >= this->_length) {
         fputs("index out of bound", stderr);
         abort();
    }
}

static void _set(ObjectPtr _this, int index, ObjectPtr o) {
    make_this(Array, _this);
    _check_index(this, index);
    REFCINC(o);
    this->_values[index] = o;
}


static ObjectPtr _get(ObjectPtr _this, int index) {
    make_this(Array, _this);
    _check_index(this, index);
    ObjectPtr o = this->_values[index];
    REFCINC(o);
    return o;
}

static unsigned int _size(ObjectPtr _this) {
    make_this(Array, _this);
    return this->_length;
}

static Iterator * _iterator(ObjectPtr _this) {
    make_this(Array, _this);
    return new(ArrayIterator, this);
}

Array * Array_new1(Array * this, int size) {
    if(size <= 0) {
        return NULL;
    }
    super(Collection, Array);
    override(Collection, iterator, _iterator);
    override(Collection, size, _size);

    this->_length = size;
    this->_values = malloc(size * sizeof(Object *));
    for(int i = 0; i < this->_length; ++i) {
        this->_values[i] = nullptr;
    }
    this->set = _set;
    this->get = _get;
    return this;
}

void Array_delete(ObjectPtr _this) {
    make_this(Array, _this);
    for(int i = 0; i < this->_length; ++i) {
        REFCDEC(this->_values[i]);
    }
    free(this->_values);
    super_delete(Object, this);
}


static ObjectPtr _next(ObjectPtr _this) {
    make_this(ArrayIterator, _this);
    if(!call(this, hasNext)) {
        return NULL;
    }
    return call(this->_array, get, this->_index++);
}

static bool _hasNext(ObjectPtr _this) {
    make_this(ArrayIterator, _this);
    return this->_index < this->_array->_length;
}


ArrayIterator * ArrayIterator_new1(ArrayIterator * this, Array * array) {
    super(Iterator, ArrayIterator);
    override(Iterator, next, _next);
    override(Iterator, hasNext, _hasNext);
    REFCINC(array);
    this->_array = array;
    this->_index = 0;
    return this;
}

void ArrayIterator_delete(ObjectPtr _this) {
    make_this(ArrayIterator, _this);
    REFCDEC(this->_array);
    super_delete(Iterator, _this);
}

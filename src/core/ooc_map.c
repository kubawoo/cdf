#include "ooc_map.h"
#include <stdio.h>

static void Map_remove(ObjectPtr _this, ObjectPtr key);

static int Map_get_length(ObjectPtr _this) {
    make_this(Map, _this);
    return this->_keys->length;
}

static void Map_put(ObjectPtr _this, ObjectPtr key, ObjectPtr value) {
    make_this(Map, _this);
    for(int i = 0; i < this->_keys->length; ++i) {
        ObjectPtr k = call(this->_keys, get, i);
        REFCDEC(k);
        if(call((Object *)key, equals, k)) {
            call(this->_values, set, i, value);
            return;
        }
    }
    call(this->_keys, add, key);
    call(this->_values, add, value);
}

static ObjectPtr Map_get(ObjectPtr _this, ObjectPtr key) {
    make_this(Map, _this);
    for(int i = 0; i < this->_keys->length; ++i) {
        ObjectPtr k = call(this->_keys, get, i);
        REFCDEC(k);
        if(call((Object *)key, equals, k)) {
            return call(this->_values, get, i);
        }
    }
    return NULL;
}

static void Map_remove(ObjectPtr _this, ObjectPtr key) {
    make_this(Map, _this);
    for(int i = 0; i < this->_keys->length; ++i) {
        ObjectPtr k = call(this->_keys, get, i);
        REFCDEC(k);
        if(call((Object *)key, equals, k)) {
            call(this->_keys, remove, i);
            call(this->_values, remove, i);
            break;
        }
    }
}

static bool Map_contains_key(ObjectPtr _this, ObjectPtr key) {
    make_this(Map, _this);
    return call(this->_keys, contains, key);
}

static bool Map_contains_value(ObjectPtr _this, ObjectPtr value) {
    make_this(Map, _this);
    return call(this->_values, contains, value);
}

static List * Map_get_keys(ObjectPtr _this) {
	make_this(Map, _this);
	REFCINC(this->_keys);
	return this->_keys;
}

static String * Map_to_string(ObjectPtr _this) {
	make_this(Map, _this);
	String * s = new(String, "{");

	for(int i = 0; i < this->_keys->length; ++i) {
		Object * key = call(this->_keys, get, i);
		String * key_string = call(key, to_string);
		REFCDEC(key);
		Object * value = call(this->_values, get, i);
		String * value_string;
		if(value != NULL) {
			value_string = call(value, to_string);
		} else {
			value_string = new(String, "NULL");
		}
		REFCDEC(value);

		call(s, append, key_string);
		call(s, append_char, '=');
		call(s, append, value_string);
		REFCDEC(key_string);
		REFCDEC(value_string);
		if(i < this->_keys->length -1) {
			call(s, append_cstring, "; ");
		}
	}

	call(s, append_char, '}');
	return s;
}

static Iterator * _iterator(ObjectPtr _this) {
    make_this(Map, _this);
    return new(MapIterator, this);
}

Map * Map_new(Map * this) {
    super(Collection, Map);
    override(Object, to_string, Map_to_string);
    override(Collection, iterator, _iterator);
    this->_values = new(List);
    this->_keys = new(List);

    this->put = Map_put;
    this->get = Map_get;
    this->remove = Map_remove;
    this->contains_key = Map_contains_key;
    this->contains_value = Map_contains_value;
    this->get_length = Map_get_length;
    this->get_keys = Map_get_keys;
    return this;
}

void Map_delete(ObjectPtr _this) {
    make_this(Map, _this);
    REFCDEC(this->_values);
    REFCDEC(this->_keys);
    super_delete(Collection, this);
}

static ObjectPtr _next(ObjectPtr _this) {
    make_this(MapIterator, _this);
    if(!call(this, hasNext)) {
        return NULL;
    }
    return call(this->map->_keys, get, this->index++);
}

static bool _hasNext(ObjectPtr _this) {
    make_this(MapIterator, _this);
    return this->index < this->map->_keys->length;
}

MapIterator * MapIterator_new1(MapIterator * this, Map * map) {
    super(Iterator, MapIterator);
    override(Iterator, hasNext, _hasNext);
    override(Iterator, next, _next);
    REFCINC(map);
    this->map = map;
    this->index = 0;
    return this;
}

void MapIterator_delete(ObjectPtr _this) {
    make_this(MapIterator, _this);
    REFCDEC(this->map);
    super_delete(Iterator, _this);
}

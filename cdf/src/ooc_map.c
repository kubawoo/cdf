#include "ooc_map.h"
#include "ooc_collection.h"
#include "ooc_pair.h"
#include <stdio.h>

/* MapIterator implementation - iterates over entries as Pair objects */
typedef struct _MapIterator {
    inherits(Iterator);
    Map * map;
    int index;
} MapIterator;

static bool MapIterator_has_next(ObjectPtr _this) {
    make_this(MapIterator, _this);
    return this->index < this->map->_keys->length;
}

static ObjectPtr MapIterator_next(ObjectPtr _this) {
    make_this(MapIterator, _this);
    if (this->index >= this->map->_keys->length) {
        return NULL;
    }
    ObjectPtr key = call(this->map->_keys, get, this->index);
    ObjectPtr value = call(this->map->_values, get, this->index);
    ObjectPtr pair = Pair_new2(NULL, key, value);
    REFCDEC(key);
    REFCDEC(value);
    this->index++;
    return pair;
}

static void MapIterator_delete(ObjectPtr _this) {
    make_this(MapIterator, _this);
    REFCDEC(this->map);
    super_delete(Iterator, _this);
}

/* MapIterator constructor */
static MapIterator * MapIterator_new1(MapIterator * this) {
    super(Iterator, MapIterator);
    this->map = NULL;
    this->index = 0;
    return this;
}

/* Map iterator method */
static Iterator * Map_iterator(ObjectPtr _this) {
    make_this(Map, _this);
    MapIterator * iter = MapIterator_new1(NULL);
    iter->map = this;
    REFCINC(this);
    iter->index = 0;
    
    /* Set up the iterator interface */
    Iterator * iter_iface = (Iterator*)iter;
    iter_iface->has_next = MapIterator_has_next;
    iter_iface->next = MapIterator_next;
    /* The destructor is already set by the constructor */
    
    return iter_iface;
}

void Map_remove(ObjectPtr _this, ObjectPtr key);

int Map_get_length(ObjectPtr _this) {
    make_this(Map, _this);
    return this->_keys->length;
}

void Map_put(ObjectPtr _this, ObjectPtr key, ObjectPtr value) {
    make_this(Map, _this);
    Map_remove(_this, key);
    call(this->_keys, add, key);
    call(this->_values, add, value);
}

ObjectPtr Map_get(ObjectPtr _this, ObjectPtr key) {
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

void Map_remove(ObjectPtr _this, ObjectPtr key) {
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

bool Map_contains_key(ObjectPtr _this, ObjectPtr key) {
    make_this(Map, _this);
    return call(this->_keys, contains, key);
}

bool Map_contains_value(ObjectPtr _this, ObjectPtr value) {
    make_this(Map, _this);
    return call(this->_values, contains, value);
}

List * Map_get_keys(ObjectPtr _this) {
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


Map * Map_new(Map * this) {
    super(Object, Map);
    override(Object, to_string, Map_to_string);
    override(Collection, iterator, Map_iterator);
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
    super_delete(Object, this);
}
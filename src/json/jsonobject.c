#include "jsonobject.h"
#include <ooc.h>

String * value_to_string(Object * value) {
	String * s = new(String);
	if (value == NULL) {
		call(s, append_cstring, "null");
	} else if (type_equal(value, "String")) {
		call(s, append_char, '"');
		call(s, append_cstring, call((String *) value, to_cstring));
		call(s, append_char, '"');
	} else if (type_equal(value, "List")) {
		List * list = (List *) value;
		call(s, append_char, '[');
		for (int j = 0; j < list->length; ++j) {
			Object * o = call(list, get, j);
			String * buf = value_to_string(o);
			REFCDEC(o);
			call(s, append, buf);
			REFCDEC(buf);
			if (j < list->length - 1) {
				call(s, append_char, ',');
			}
		}
		call(s, append_char, ']');
	} else {
		String * buf = call(value, to_string);
		call(s, append, buf);
		REFCDEC(buf);
	}
	return s;
}

String * JsonObject_to_string(ObjectPtr _this) {
	make_this(JsonObject, _this);
	String * s = new(String, "{");

	List * keys = call(this->_map, get_keys);
	for (int i = 0; i < keys->length; ++i) {
		String * key = call(keys, get, i);
		call(s, append_char, '"');
		call(s, append_cstring, call(key, to_cstring));
		call(s, append_cstring, "\":");

		Object * value = call(this->_map, get, key);
		String * value_string = value_to_string(value);
		call(s, append, value_string);
		REFCDEC(value_string);
		REFCDEC(value);
		REFCDEC(key);

		if (i < keys->length - 1) {
			call(s, append_char, ',');
		}
	}

	REFCDEC(keys);

	call(s, append_cstring, "}");
	return s;
}

void JsonObject_put_value(ObjectPtr _this, String * name, ObjectPtr value) {
	make_this(JsonObject, _this);
	call(this->_map, put, name, value);
}

ObjectPtr JsonObject_get_value(ObjectPtr _this, String * name) {
	make_this(JsonObject, _this);
	return call(this->_map, get, name);
}

Map * JsonObject_get_map(ObjectPtr _this) {
	make_this(JsonObject, _this);
	REFCINC(this->_map);
	return this->_map;
}

JsonObject * JsonObject_new(JsonObject * this) {
	super(Object, JsonObject);
	override(Object, to_string, JsonObject_to_string);
	this->put_value = JsonObject_put_value;
	this->get_value = JsonObject_get_value;
	this->get_map = JsonObject_get_map;
	this->_map = new(Map);
	return this;
}

void JsonObject_delete(ObjectPtr _this) {
	make_this(JsonObject, _this);
	REFCDEC(this->_map);
	super_delete(Object, this);
}


#include "jsonobject.h"
#include <stdio.h>

/* Wraps a string in quotes, escaping what RFC 8259 requires to be escaped.
   Without this a value containing a quote or a newline would produce invalid
   JSON, and the receiver would reject or mis-parse the document. */
static String * _escape_json_string(String * value) {
	String * s = new(String, "\"");
	const char * src = call(value, to_cstring);
	for(const char * p = src; *p != '\0'; ++p) {
		unsigned char c = (unsigned char) *p;
		switch(c) {
			case '"':  call(s, append_cstring, "\\\""); break;
			case '\\': call(s, append_cstring, "\\\\"); break;
			case '\b': call(s, append_cstring, "\\b"); break;
			case '\f': call(s, append_cstring, "\\f"); break;
			case '\n': call(s, append_cstring, "\\n"); break;
			case '\r': call(s, append_cstring, "\\r"); break;
			case '\t': call(s, append_cstring, "\\t"); break;
			default:
				if(c < 0x20) {
					// Remaining control characters have no short form and must
					// go out as \u00XX.
					char esc[7];
					snprintf(esc, sizeof esc, "\\u%04x", c);
					call(s, append_cstring, esc);
				} else {
					// Bytes >= 0x20 pass through, so UTF-8 stays intact.
					call(s, append_char, (char) c);
				}
				break;
		}
	}
	call(s, append_char, '"');
	return s;
}

String * value_to_string(Object * value) {
	String * s = new(String);
	if (value == NULL) {
		call(s, append_cstring, "null");
	} else if (type_equal(value, "String")) {
		String * escaped = _escape_json_string((String *) value);
		call(s, append, escaped);
		REFCDEC(escaped);
	} else if (type_equal(value, "List")) {
		List * list = (List *) value;
		call(s, append_char, '[');
        for (int j = 0; j < call(list, size); ++j) {
			Object * o = call(list, get, j);
			String * buf = value_to_string(o);
			REFCDEC(o);
			call(s, append, buf);
			REFCDEC(buf);
            if (j < call(list, size) - 1) {
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
    for (int i = 0; i < call(keys, size); ++i) {
		String * key = call(keys, get, i);
		String * escaped_key = _escape_json_string(key);
		call(s, append, escaped_key);
		call(s, append_char, ':');
		REFCDEC(escaped_key);

		Object * value = call(this->_map, get, key);
		String * value_string = value_to_string(value);
		call(s, append, value_string);
		REFCDEC(value_string);
		REFCDEC(value);
		REFCDEC(key);

        if (i < call(keys, size) - 1) {
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


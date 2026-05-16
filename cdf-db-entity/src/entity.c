#include "entity.h"
#include <json.h>
#include <stdio.h>


FieldMetadata * FieldMetadata_new4(FieldMetadata * this, Entity * obj, Object** field, String * name, String * type) {
	super(Object, FieldMetadata);
	this->offset = (intptr_t) field - (intptr_t) obj;
	REFCINC(name);
	this->name = name;
	REFCINC(type);
	this->type = type;
	return this;
}

void FieldMetadata_delete(ObjectPtr _this) {
	make_this(FieldMetadata, _this);
	REFCDEC(this->name);
	REFCDEC(this->type);
	super_delete(Object, _this);
}

static String * table_name(ObjectPtr _this) {
	return new(String, ((Object *)_this)->type);
}


static String * to_json(ObjectPtr _this, List * fields) {
    make_this(Entity, _this);
    JsonObject * jo = new(JsonObject);

    Map * entity_map = call(this, to_map, fields);
    List * keys = call(entity_map, get_keys);

    for(int i = 0; i < keys->length; ++i) {
    	String * key = call(keys, get, i);
    	Object * value = call(entity_map, get, key);

    	call(jo, put_value, key, value);

    	REFCDEC(key);
    	REFCDEC(value);
    }

    REFCDEC(keys);
    REFCDEC(entity_map);
    String * s = call(jo, to_string);
    REFCDEC(jo);
    return s;
}

void from_json(ObjectPtr _this, String * json, List * fields) {
    make_this(Entity, _this);
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    InputStream * json_stream = new(StringInputStream, json);

    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);

    if(ret == CJSON_PARSE_SUCCESS) {
		Map * map = call((JsonObject*) handler->object, get_map);
		call(this, from_map, map, fields);
		REFCDEC(map);
    }

    REFCDEC(parser);
}

 List * fields(ObjectPtr _this, bool with_id) {
	make_this(Entity, _this);
	List * list = call(this, _fields);
	if(with_id) {
		FieldMetadata * fm = new(FieldMetadata, this, (Object**)&(this->id),
			REFCTMP(new(String, "id")), REFCTMP(new(String, "Long")));
		call(list, insert, 0, fm);
		REFCDEC(fm);
	}
	return list;
}

static Map * to_map(ObjectPtr _this, List * fields) {
	make_this(Entity, _this);
	Map * map = new(Map);
	if(fields == NULL) {
		fields = call(this, fields, true);
	} else {
		REFCINC(fields);
	}
	for(int i = 0; i < fields->length; ++i) {
		FieldMetadata * fm = call(fields, get, i);
		Object * field = *((Object **) ((intptr_t) this + fm->offset));
		call(map, put, fm->name, field);
		REFCDEC(fm);
	}
	REFCDEC(fields);
	return map;
}

static void from_map(ObjectPtr _this, Map * map, List * fields) {
	make_this(Entity, _this);
	if(fields == NULL) {
		fields = call(this, fields, true);
	} else {
		REFCINC(fields);
	}
	for(int i = 0; i < fields->length; ++i) {
		FieldMetadata * fm = call(fields, get, i);
		Object ** field = ((Object **) ((intptr_t) this + fm->offset));
		REFCDEC(*field);
		Object * value = call(map, get, fm->name);
		*field = value;
		REFCDEC(fm);
	}
	REFCDEC(fields);
}

static List * _fields(ObjectPtr _this) {
	return new(List);
}


Entity * Entity_new(Entity * this) {
	super(Object, Entity);
	this->id = NULL;
	this->table_name = table_name;
	this->to_json = to_json;
	this->from_json = from_json;
	this->to_map = to_map;
	this->from_map = from_map;
	this->_fields = _fields;
	this->fields = fields;

	return this;
}

void Entity_delete(ObjectPtr _this) {
	make_this(Entity, _this);
	REFCDEC(this->id);
	super_delete(Object, _this);
}


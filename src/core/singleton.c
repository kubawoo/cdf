#include "map.h"

static Map * _cdf_singleton_map = NULL;

__attribute__((destructor))
static void _cdf_singleton_cleanup() {
	REFCDEC(_cdf_singleton_map);
}

__attribute__((constructor))
static void _cdf_singleton_init() {
	_cdf_singleton_map = new(Map);
}

ObjectPtr _cdf_get_singleton(const char * name) {
	String * name_string = new(String, name);
	Object * singleton = NULL;

	if(call(_cdf_singleton_map, contains_key, name_string)) {
		singleton = call(_cdf_singleton_map, get, name_string);
		REFCDEC(singleton);
	}
	REFCDEC(name_string);
	return singleton;
}

ObjectPtr _cdf_set_singleton(const char * name, ObjectPtr instance) {
	String * name_string = new(String, name);
	call(_cdf_singleton_map, put, name_string, instance);
	REFCDEC(name_string);
	REFCDEC(instance);
	return instance;
}


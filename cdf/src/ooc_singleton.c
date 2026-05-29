#include "ooc_map.h"

static Map * _cdf_singleton_map = NULL;

static void _cdf_singleton_cleanup() {
	REFCDEC(_cdf_singleton_map);
}

static void _cdf_ensure_initialized() {
	if(_cdf_singleton_map == NULL) {
		_cdf_singleton_map = new(Map);
		atexit(_cdf_singleton_cleanup);
	}
}

ObjectPtr _cdf_get_singleton(const char * name) {
	_cdf_ensure_initialized();
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


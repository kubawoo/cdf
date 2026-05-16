#ifndef OOC_SINGLETON_H
#define OOC_SINGLETON_H

#include "ooc_object.h"

#define singleton(x) (_cdf_get_singleton(#x) == NULL ? _cdf_set_singleton(#x, new(x)) : _cdf_get_singleton(#x))

ObjectPtr _cdf_get_singleton(const char * name);
ObjectPtr _cdf_set_singleton(const char * name, ObjectPtr instance);

#endif

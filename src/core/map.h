#ifndef CDF_CORE_MAP_H
#define CDF_CORE_MAP_H

#include "collection.h"
#include "list.h"

typedef struct {
    inherits(Collection);
    List * _keys;
    List * _values;
    void (*put)(ObjectPtr, ObjectPtr, ObjectPtr);
    ObjectPtr (*get)(ObjectPtr, ObjectPtr);
    void (*remove)(ObjectPtr, ObjectPtr);
    bool (*contains_key)(ObjectPtr, ObjectPtr);
    bool (*contains_value)(ObjectPtr, ObjectPtr);
    List * (*get_keys)(ObjectPtr);
    String * (*to_string)(ObjectPtr);
    Iterator* (*iterator)(ObjectPtr);
    unsigned int (*size)(ObjectPtr);
} Map;

Map * Map_new(Map * this);
void Map_delete(ObjectPtr);

typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    Map * _map;
    int _index;
} MapIterator;

MapIterator * MapIterator_new1(MapIterator * this, Map * map);
void MapIterator_delete(ObjectPtr);

#endif

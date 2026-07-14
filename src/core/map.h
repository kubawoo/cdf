#ifndef CDF_CORE_MAP_H
#define CDF_CORE_MAP_H

#include "collection.h"
#include "list.h"

typedef struct {
    inherits(Collection);
    List * _keys;
    List * _values;
    int (*get_length)(void *);
    void (*put)(void *, ObjectPtr, ObjectPtr);
    ObjectPtr (*get)(void *, ObjectPtr);
    void (*remove)(void *, ObjectPtr);
    bool (*contains_key)(void *, ObjectPtr);
    bool (*contains_value)(void *, ObjectPtr);
    List * (*get_keys)(ObjectPtr);
    String * (*to_string)(ObjectPtr);
    Iterator* (*iterator)(ObjectPtr);
} Map;

Map * Map_new(Map * this);
void Map_delete(ObjectPtr);

typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    Map * map;
    int index;
} MapIterator;

MapIterator * MapIterator_new1(MapIterator * this, Map * map);
void MapIterator_delete(ObjectPtr);

#endif

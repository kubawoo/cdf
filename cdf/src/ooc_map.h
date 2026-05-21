#ifndef OOC_MAP_H
#define OOC_MAP_H

#include "ooc_collection.h"
#include "ooc_list.h"
#include "ooc_iterator.h"

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
} Map;

// Iterator implementation for Map
typedef struct {
    inherits(Iterator);
    Map * map;
    int index;
} MapIterator;

Map * Map_new(Map * this);
void Map_delete(ObjectPtr);
MapIterator * MapIterator_new(MapIterator * this, Map * map);
bool MapIterator_hasNext(ObjectPtr _this);
ObjectPtr MapIterator_next(ObjectPtr _this);

#endif
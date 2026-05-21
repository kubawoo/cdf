#include "ooc_collection.h"
#include "ooc_iterator.h"

Collection * Collection_new(Collection * this) {
    if (!this) {
        this = malloc(sizeof(Collection));
    }
    Object_new((Object*)this);
    // The iterator method must be implemented by subclasses
    this->iterator = NULL;
    return this;
}

void Collection_delete(ObjectPtr _this) {
    // No specific cleanup needed for base Collection
}
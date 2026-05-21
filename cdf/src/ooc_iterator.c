#include "ooc_iterator.h"

Iterator * Iterator_new(Iterator * this) {
    if (!this) {
        this = malloc(sizeof(Iterator));
    }
    Object_new((Object*)this);
    // The hasNext and next methods must be implemented by subclasses
    this->hasNext = NULL;
    this->next = NULL;
    return this;
}

void Iterator_delete(ObjectPtr _this) {
    // No specific cleanup needed for base Iterator
}
#include "iterator.h"

Iterator * Iterator_new(Iterator * this) {
    super(Object, Iterator);
    // The hasNext and next methods must be implemented by subclasses
    this->hasNext = nullptr;
    this->next = nullptr;
    return this;
}

void Iterator_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}
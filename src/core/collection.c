#include "collection.h"
#include "iterator.h"

Collection * Collection_new(Collection * this) {
    super(Object, Collection);
    // The iterator method must be implemented by subclasses
    this->iterator = NULL;
    return this;
}

void Collection_delete(ObjectPtr _this) {
   super_delete(Object, _this);
}
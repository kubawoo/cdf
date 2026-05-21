#include "ooc_collection.h"

void Collection_delete(ObjectPtr _this) {
    make_this(Collection, _this);
    super_delete(Object, _this);
}

void Iterator_delete(ObjectPtr _this) {
    make_this(Iterator, _this);
    super_delete(Object, _this);
}

Iterator * Iterator_new(Iterator * this) {
    if (!this) {
        this = malloc(sizeof(Iterator));
    }
    super(Object, Iterator);
    return this;
}
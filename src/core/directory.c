#include "directory.h"
#include <sys/stat.h>

static bool create(ObjectPtr _this, String * path) {
    return mkdir(call(path, to_cstring), 0755) == 0;
}

Directory * Directory_new(Directory * this) {
    super(Object, Directory);
    this->create = create;
    return this;
}

void Directory_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

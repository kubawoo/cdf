#include "console.h"
#include <stdio.h>

static void print_object(ObjectPtr _this, ObjectPtr o) {
    String * s = call((Object *)o, to_string);
    printf("%s\n", call(s, to_cstring));
    REFCDEC(s);
}

Console * Console_new(Console * this) {
    super(Object, Console);
    this->print_object = print_object;
    return this;
}

void Console_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}


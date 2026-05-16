#include "console.h"
#include <stdio.h>

static void print_object(ObjectPtr _this, ObjectPtr o) {
    String * s = call((Object *)o, to_string);
    printf("%s\n", call(s, to_cstring));
    REFCDEC(s);
}

static void print_cstring(ObjectPtr _this, const char * str) {
    printf("%s\n", str);
}

Console * Console_new(Console * this) {
    super(Object, Console);
    this->print_object = print_object;
    this->print_cstring = print_cstring;
    return this;
}

void Console_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}


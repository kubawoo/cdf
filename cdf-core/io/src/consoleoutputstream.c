#include "consoleoutputstream.h"
#include <stdio.h>

static void write(ObjectPtr _this, char c) {
    printf("%c", c);
}


ConsoleOutputStream * ConsoleOutputStream_new(ConsoleOutputStream * this) {
    super(OutputStream, ConsoleOutputStream);
    override(OutputStream, write, write);
    return this;
}

void ConsoleOutputStream_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}


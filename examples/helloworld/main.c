#include <core.h>
#include <io.h>
#include <stdio.h>

int main(void) {
    String * txt = new(String, "Hello CDF!");
    Console * c = singleton(Console);

    printf("This is one way to call a method: ");
    call(c, print_object, txt);
    call(c, print_cstring, "\n");

    printf("This is another one: ");
    c->print_object(c, txt);
    c->print_cstring(c, "\n");

    REFCDEC(txt);
    return 0;
}

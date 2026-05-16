#include <cdf.h>
#include <stdio.h>

int main(void) {
    String * txt = new(String, "Hello CDF!");
    Console * c = new(Console);

    printf("This is one way to call a method: ");
    call(c, print_object, txt);

    printf("This is another one: ");
    c->print_object(c, txt);

    REFCDEC(c);
    REFCDEC(txt);
    return 0;
}

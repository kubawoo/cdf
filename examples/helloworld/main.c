#include <cdf.h>

int main(void) {
    String * txt = new(String, "Hello CDF!");
    Console * c = new(Console);
    //this two lines are equivalent
    call(c, print_object, txt);
    c->print_object(c, txt);
    REFCDEC(c);
    REFCDEC(txt);
    return 0;
}

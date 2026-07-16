#include "shape.h"
#include "rectangle.h"
#include "square.h"
#include "circle.h"
#include <io.h>

int main(void) {
    Console * c = singleton(Console);

    List * shapes = new(List);
    Shape * shape = new(Shape);
    call(shapes, add, shape);
    REFCDEC(shape);

    shape = new(Rectangle, 2, 3);
    call(shapes, add, shape);
    REFCDEC(shape);

    shape = new(Circle, 4.5);
    call(shapes, add, shape);
    REFCDEC(shape);

    shape = new(Square, 6);
    call(shapes, add, shape);
    REFCDEC(shape);

    for(int i = 0; i < shapes->length; i++) {
        shape = call(shapes, get, i);
        call(c, print_object, shape);
	call(c, print_cstring, "\n");
        String * s = new(String);
        call(s, format, " - area=%f\n - circumference=%f", call(shape, area), call(shape, circumference));
        call(c, print_object, s);
	call(c, print_cstring, "\n\n");
        REFCDEC(s);
        REFCDEC(shape);
    }


    REFCDEC(shapes);
    return 0;
}



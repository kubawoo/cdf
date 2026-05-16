#include "shape.h"
#include "rectangle.h"
#include "square.h"
#include "circle.h"

int main(void) {
    Console * c = new(Console);

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
        String * s = new(String);
        call(s, format, "area=%f, circumference=%f", call(shape, area), call(shape, circumference));
        call(c, print_object, s);
        REFCDEC(s);
        REFCDEC(shape);
    }


    REFCDEC(shapes);
    REFCDEC(c);
    return 0;
}



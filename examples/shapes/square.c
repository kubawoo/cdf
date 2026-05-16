#include "square.h"


static double square_circumference(ObjectPtr _this) {
    make_this(Rectangle, _this);
    return 4 * this->a;
}

static String * square_to_string(ObjectPtr _this) {
    make_this(Rectangle, _this);
    String * s = new(String);
    call(s, format, "Square %f x %f", this->a, this->b);
    return s;
}


Square * Square_new1(Square * this, double a) {
    super(Rectangle, Square);
    override(Rectangle, to_string, square_to_string);
    override(Shape, to_string, square_to_string);
    override(Object, to_string, square_to_string);
    override(Shape, circumference, square_circumference);
    super_method(Rectangle, area);

    ((Rectangle *) this)->a = a;
    ((Rectangle *) this)->b = a;

    return this;
}

void Square_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

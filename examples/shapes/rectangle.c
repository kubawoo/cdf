#include "rectangle.h"

static double rect_area(ObjectPtr _this) {
    make_this(Rectangle, _this);
    return this->a * this->b;
}

static double rect_circumference(ObjectPtr _this) {
    make_this(Rectangle, _this);
    return 2 * this->a + 2 * this->b;
}

static String * rect_to_string(ObjectPtr _this) {
    make_this(Rectangle, _this);
    String * s = new(String);
    call(s, format, "Rectangle %f x %f", this->a, this->b);
    return s;
}

Rectangle * Rectangle_new(Rectangle * this) {
    return this(Rectangle, 0.0 ,0.0);
}

Rectangle * Rectangle_new2(Rectangle * this, double a, double b) {
    super(Shape, Rectangle);
    override(Shape, to_string, rect_to_string);
    override(Object, to_string, rect_to_string);
    override(Shape, area, rect_area);
    override(Shape, circumference, rect_circumference);
    this->a = a;
    this->b = b;
    return this;
}

void Rectangle_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

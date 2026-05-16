#include "circle.h"

static double circle_area(ObjectPtr _this) {
    make_this(Circle, _this);
    return 3.14 * this->r * this->r;
}

static double circle_circumference(ObjectPtr _this) {
    make_this(Circle, _this);
    return 2 * 3.14 * this->r;
}

static String * circle_to_string(ObjectPtr _this) {
    make_this(Circle, _this);
    String * s = new(String);
    call(s, format, "Circle with radius %f", this->r);
    return s;
}

Circle * Circle_new1(Circle * this, double r) {
    super(Shape, Circle);
    override(Shape, to_string, circle_to_string);
    override(Object, to_string, circle_to_string);
    override(Shape, area, circle_area);
    override(Shape, circumference, circle_circumference);
    this->r = r;
    return this;
}

void Circle_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

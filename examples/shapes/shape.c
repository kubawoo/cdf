#include <math.h>
#include "shape.h"

static double area(ObjectPtr _this) {
    return NAN;
}

static double circumference(ObjectPtr _this) {
    return NAN;
}

Shape * Shape_new(Shape * this) {
    super(Object, Shape);
    super_method(Object, to_string);
    this->area = area;
    this->circumference = circumference;
    return this;
}

void Shape_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

typedef struct {
    inherits(Shape);
    String *(*to_string)(ObjectPtr _his);
    double (*area)(ObjectPtr this);
    double (*circumference)(ObjectPtr this);

    double a;
    double b;
} Rectangle;

Rectangle * Rectangle_new(Rectangle * this);
Rectangle * Rectangle_new2(Rectangle * this, double a, double b);
void Rectangle_delete(ObjectPtr this);

#endif

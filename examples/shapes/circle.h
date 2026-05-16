#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

typedef struct {
    inherits(Shape);
    String *(*to_string)(ObjectPtr _his);
    double (*area)(ObjectPtr this);
    double (*circumference)(ObjectPtr this);

    double r;
} Circle;

Circle * Circle_new1(Circle * this, double r);
void Circle_delete(ObjectPtr this);

#endif

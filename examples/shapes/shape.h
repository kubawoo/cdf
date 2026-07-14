#ifndef SHAPE_H
#define SHAPE_H

#include <core.h>

typedef struct {
    inherits(Object);
    String *(*to_string)(ObjectPtr _his);
    double (*area)(ObjectPtr this);
    double (*circumference)(ObjectPtr this);
} Shape;

Shape * Shape_new(Shape * this);
void Shape_delete(ObjectPtr this);

#endif

#ifndef SQUARE_H
#define SQUARE_H

#include "rectangle.h"

typedef struct {
    inherits(Rectangle);
    String *(*to_string)(ObjectPtr _his);
    double (*area)(ObjectPtr this);
    double (*circumference)(ObjectPtr this);
} Square;

Square * Square_new1(Square * this, double a);
void Square_delete(ObjectPtr this);

#endif

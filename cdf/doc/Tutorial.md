### Hello world
It always starts with a hello world...

```c
#include <cdf.h>                                 //(1)

int main(void) {
    String * txt = new(String, "Hello CDF!\n");  //(2)
    Console * c = new(Console);                  //(3)
    //this two lines are equivalent
    call(c, print_object, txt);                  //(4)
    c->print_object(c, txt);                     //(4)
    REFCDEC(c);                                  //(5)
    REFCDEC(txt);                                //(5)
    return 0;
}
```
Important parts of the code:
1. include the whole CDF library (we may also include only selected classes/modules)
2. construct a new String object with initial value
3. construct a console object - note that you use the same _new_ macro for constructing objects with any number of arguments
4. call a method from _Console_ class (the _call_ macro is to make things easier to write)
5. get rid of the objects - decrease its reference counter (without it code would result in memory leak)


### Classes

Defining 'classes' is what CDF was created for. Although CDF provides a set of ready-to-use modules, it may happen than you will need to define your own class. We will concentrate on a rather elaborated example, so that you know all the bolts and nuts.


```c
// file: main.c
#include "shape.h"
#include "rectangle.h"
#include "square.h"
#include "circle.h"

int main(void) {
    Console * c = new(Console);

    List * shapes = new(List);                 //(1)
    Shape * shape = new(Shape);                //(2)
    call(shapes, add, shape);                  //(3)
    REFCDEC(shape);                            //(4)

    shape = new(Rectangle, 2, 3);              //(5)
    call(shapes, add, shape);
    REFCDEC(shape);

    shape = new(Circle, 4.5);                   //(5)
    call(shapes, add, shape);
    REFCDEC(shape);

    shape = new(Square, 6);                     //(5)
    call(shapes, add, shape);
    REFCDEC(shape);

    for(int i = 0; i < shapes->length; i++) {                 //(6)
        shape = call(shapes, get, i);
        call(c, print_object, shape);                         //(7)
        String * s = new(String);
        call(s, format, "area=%f, circumference=%f",
             call(shape, area), call(shape, circumference));  //(7)
        call(c, print_object, s);
        REFCDEC(s);
        REFCDEC(shape);
    }

    REFCDEC(shapes);                                           //(8)
    REFCDEC(c);
    return 0;
}
```

Now let's go through this code:
1. create a list, which will hold our objects
2. create a _Shape_ (well, this is meant to be an interface/abstract class so you shouldn't create it, but sine you can...)
3. put the _Shape_ object into list
4. we are no longer going to use this reference, so we decrease the reference count (note that by adding an object to list, the reference count was increased; decreasing the count at this point does not delete the object)
5. now we create a rectangle, a circle and a square and we put them into our _shapes_ list
6. iterate over list elements
7. that's the most important parts: we call _area_ and _circumference_ methods on a _Shape_ type, but a correct implementation (depending on the actual object type) is being called; the same applies when object is printed to the console - then _to_string_ method is being called
8. decreasing reference count on the _List_ object (on any other collection) results in decreasing reference count on all of the elements; thanks to that all elements get deleted together with the list
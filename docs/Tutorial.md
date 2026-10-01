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


### Terminal UI

The _tui_ module draws to a terminal. It is built directly on POSIX — _termios_
raw mode, `TIOCGWINSZ` for the size, `poll` for input and ANSI escape sequences
for output — so it adds no dependency and no terminfo lookup. It targets the
VT-100/xterm escape set.

There are four classes to know:

| Class | Role |
|---|---|
| `Terminal` | raw mode, terminal size, `SIGWINCH` |
| `Buffer` | a grid of cells you draw into |
| `Renderer` | diffs two buffers and writes the changed cells |
| `EventLoop` | waits for input, calls your handler, repaints |

Drawing is double-buffered. You mutate a `Buffer`, then `present` compares it
against the renderer's front buffer and writes only what changed, so a frame
costs one write rather than a full screen repaint.

```c
// file: main.c
#include <core.h>
#include <tui.h>
#include <stdio.h>

static bool on_key(Event * ev, ObjectPtr user) {     //(1)
    if(ev->key.key == KEY_CHAR && ev->key.ch == L'q') {
        return false;                                 //(2)
    }
    if(ev->key.key == KEY_LEFT) {
        call((Buffer *)user, print, 1, 1, "left");
    }
    return true;                                      //(3)
}

int main(void) {
    Terminal * term = new(Terminal);
    if(!call(term, open)) {                           //(4)
        fprintf(stderr, "not a terminal\n");
        REFCDEC(term);
        return 1;
    }

    int w = 0, h = 0;
    call(term, get_size, &w, &h);                     //(5)

    Renderer * rend = new(Renderer, call(term, out_fd), w, h);
    Buffer   * buf  = new(Buffer, w, h);

    call(rend, enter_alt_screen);                     //(6)
    call(rend, hide_cursor);
    call(buf, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(buf, box, 0, 0, w, h, BOX_ROUNDED, TUI_COLOR_CYAN, TUI_COLOR_BLACK);
    call(rend, present, buf);

    EventLoop * loop = new(EventLoop);
    call(loop, run, term, rend, buf, on_key, buf);    //(7)

    call(rend, leave_alt_screen);                     //(8)
    call(term, close);

    REFCDEC(buf);
    REFCDEC(loop);
    REFCDEC(rend);
    REFCDEC(term);
    return 0;
}
```

1. the handler receives every event; return `false` to stop the loop
2. quit on _q_
3. returning `true` keeps the loop running
4. `open` returns false when stdin or stdout is not a terminal, so a piped run
   fails cleanly instead of leaving the terminal in a broken state
5. falls back to `$COLUMNS`/`$LINES`, then 80x24, when the ioctl is unavailable
6. the alternate screen keeps the user's scrollback intact
7. the loop dispatches to the handler, then calls `present` once per event
8. always leave the alternate screen and close the terminal

Colours and attributes are set with a pen, which the drawing primitives use:

```c
call(buf, set_pen, TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_ATTR_BOLD);
call(buf, print, 2, 3, "status: %d", count);
```

Drawing outside the buffer, or outside the current clip rect, is discarded
rather than written past the edge — so a routine that draws a full row is safe
to call when less room is available. Use `set_clip` when you need a sub-view.

A complete program is in [`examples/tui`](../examples/tui). Note that
`tui_init_utf8()` is worth calling before drawing if you use `Buffer` on its
own: `Terminal.open` already does it, and it is what makes double-width glyphs
such as CJK characters occupy two columns instead of one.
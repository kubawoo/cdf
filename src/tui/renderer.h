#ifndef CDF_TUI_RENDERER_H
#define CDF_TUI_RENDERER_H

#include "../core/core.h"
#include "buffer.h"
#include "bufferdiff.h"

typedef struct {
    inherits(Object);

    /* Compares back against front, writes only the cells that changed, then
       copies back into front so the next call diffs against what is on
       screen. Returns the number of cells written. */
    int (*present)(ObjectPtr, Buffer * back);

    /* Marks the next present as a full repaint. Needed after a resize, when
       the terminal's own contents no longer match our idea of the front. */
    void (*invalidate)(ObjectPtr);
    void (*set_cursor)(ObjectPtr, int, int);
    void (*show_cursor)(ObjectPtr);
    void (*hide_cursor)(ObjectPtr);
    void (*enter_alt_screen)(ObjectPtr);
    void (*leave_alt_screen)(ObjectPtr);
    void (*clear_screen)(ObjectPtr);
    void (*set_title)(ObjectPtr, String *);

    int (*width)(ObjectPtr);
    int (*height)(ObjectPtr);

    //'private'
    int _fd;
    Buffer * _front;
    BufferDiff * _diff;
    bool _force_full;
    bool _cursor_visible;
    int _cursor_x;
    int _cursor_y;
} Renderer;

Renderer * Renderer_new3(Renderer * this, int fd, int width, int height);
void Renderer_delete(ObjectPtr);

#endif

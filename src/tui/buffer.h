#ifndef CDF_TUI_BUFFER_H
#define CDF_TUI_BUFFER_H

#include "../core/core.h"
#include "cell.h"

typedef enum {
    BOX_SINGLE,
    BOX_DOUBLE,
    BOX_ROUNDED
} BoxStyle;

typedef struct {
    inherits(Object);

    /* All coordinates are buffer-relative, x across and y down. A zero size is
       legal and renders nothing, so callers never need to special-case it. */
    int (*width)(ObjectPtr);
    int (*height)(ObjectPtr);
    void (*resize)(ObjectPtr, int, int);

    void (*clear)(ObjectPtr, uint16_t fg, uint16_t bg);
    void (*set_cell)(ObjectPtr, int, int, Cell);
    Cell (*get_cell)(ObjectPtr, int, int);

    /* Returns the number of cells advanced, so a caller can chain a cursor
       forward. A double-width glyph advances two, or one at the right edge. */
    int (*print)(ObjectPtr, int, int, const char * fmt, ...);
    int (*print_string)(ObjectPtr, int, int, String * text);

    void (*fill_rect)(ObjectPtr, int, int, int, int, Cell);
    void (*hline)(ObjectPtr, int, int, int, wchar_t);
    void (*vline)(ObjectPtr, int, int, int, wchar_t);
    void (*box)(ObjectPtr, int, int, int, int, BoxStyle, uint16_t, uint16_t);

    /* Drawing outside the clip rect is discarded rather than written past the
       grid edge, which is what lets a caller draw a full-screen frame once and
       clip the contents. */
    void (*set_clip)(ObjectPtr, int, int, int, int);
    void (*clear_clip)(ObjectPtr);

    /* The attributes that print/hline/vline use. Explicit box() calls carry
       their own colour, so these only affect the convenience primitives. */
    void (*set_pen)(ObjectPtr, uint16_t, uint16_t, uint8_t);

    Cell * (*cells)(ObjectPtr);
    bool (*in_bounds)(ObjectPtr, int, int);

    //'private'
    Cell * _cells;
    int _width;
    int _height;
    int _clip_x;
    int _clip_y;
    int _clip_w;
    int _clip_h;
    uint16_t _pen_fg;
    uint16_t _pen_bg;
    uint8_t _pen_attrs;
} Buffer;

/* new(Buffer, width, height) dispatches to _new2: the new() macro names the
   constructor after the argument count, not the index. */
Buffer * Buffer_new2(Buffer * this, int width, int height);
void Buffer_delete(ObjectPtr);

#endif

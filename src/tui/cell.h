#ifndef CDF_TUI_CELL_H
#define CDF_TUI_CELL_H

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "attributes.h"

/* A single screen cell. This is a value type stored by value inside the cell
   grid, deliberately not a CDF Object: a vtable and a refcount per cell would
   cost more than the payload and there is no behaviour to dispatch on. */
typedef struct {
    wchar_t ch;        /* the glyph; L' ' for blank */
    uint16_t fg;       /* TuiColor, or TUI_COLOR_DEFAULT */
    uint16_t bg;       /* TuiColor, or TUI_COLOR_DEFAULT */
    uint8_t attrs;     /* bitwise or of TuiAttr */
    bool wide_lead;    /* true on the first half of a double-width glyph */
    bool wide_tail;    /* true on the blank half that follows it */
} Cell;

#define CELL_FILL_CHAR L' '

static inline Cell cell_make(wchar_t ch, uint16_t fg, uint16_t bg, uint8_t attrs) {
    Cell c = { ch, fg, bg, attrs, false, false };
    return c;
}

static inline Cell cell_blank(uint16_t fg, uint16_t bg) {
    return cell_make(CELL_FILL_CHAR, fg, bg, TUI_ATTR_NONE);
}

/* A cell carrying no content at all, used to initialise the front buffer so
   the first diff emits the whole screen. */
static inline Cell cell_empty(void) {
    Cell c = { 0, TUI_COLOR_DEFAULT, TUI_COLOR_DEFAULT, TUI_ATTR_NONE, false, false };
    return c;
}

bool cell_equal(const Cell * a, const Cell * b);

/* Selects a UTF-8 locale so wcwidth reports real column widths; without it
   glibc returns -1 for every non-ASCII glyph and double-width characters
   silently collapse to a single cell. Terminal.open() calls this, but code
   that uses a Buffer on its own must call it too. Returns true when a UTF-8
   locale was selected. */
bool tui_init_utf8(void);

#endif

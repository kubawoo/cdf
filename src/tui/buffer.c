#include "buffer.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static int _wcwidth_of(wchar_t ch) {
    int w = wcwidth(ch);
    return w < 0 ? 1 : w;   /* treat an unprintable as one cell, never zero */
}

static int _width(ObjectPtr _this) {
    make_this(Buffer, _this);
    return this->_width;
}

static int _height(ObjectPtr _this) {
    make_this(Buffer, _this);
    return this->_height;
}

static bool _in_bounds(ObjectPtr _this, int x, int y) {
    make_this(Buffer, _this);
    return x >= 0 && y >= 0 && x < this->_width && y < this->_height;
}

static bool _in_clip(Buffer * this, int x, int y) {
    return x >= this->_clip_x
        && y >= this->_clip_y
        && x < this->_clip_x + this->_clip_w
        && y < this->_clip_y + this->_clip_h;
}

/* Writes one glyph, pairing it with a blank tail when it is double-width so the
   grid never leaves half a character behind for the next write to straddle. */
static void _put(Buffer * this, int x, int y, wchar_t ch, uint16_t fg, uint16_t bg, uint8_t attrs) {
    if(!_in_clip(this, x, y)) return;

    /* Overwriting either half of an existing wide glyph clears its partner, so
       a stale tail can never survive to be rendered. */
    Cell * cur = &this->_cells[y * this->_width + x];
    if(cur->wide_lead && _in_clip(this, x + 1, y)) {
        this->_cells[y * this->_width + x + 1] = cell_blank(fg, bg);
    } else if(cur->wide_tail && _in_clip(this, x - 1, y)) {
        this->_cells[y * this->_width + x - 1] = cell_blank(fg, bg);
    }

    int w = _wcwidth_of(ch);
    *cur = cell_make(ch, fg, bg, attrs);

    if(w == 2) {
        if(x + 1 >= this->_width || !_in_clip(this, x + 1, y)) {
            /* No room for the second half: fall back to a single cell so the
               glyph does not smear into whatever is on the right. */
            *cur = cell_make(CELL_FILL_CHAR, fg, bg, attrs);
            return;
        }
        cur->wide_lead = true;
        Cell * tail = &this->_cells[y * this->_width + x + 1];
        *tail = cell_blank(fg, bg);
        tail->wide_tail = true;
    }
}

/* Advances x past a glyph the caller has already drawn, honouring the clip so a
   string that runs off the edge stops advancing at the boundary. */
static int _advance(Buffer * this, int x, int y, wchar_t ch) {
    int w = _wcwidth_of(ch);
    if(w == 2 && x + 1 >= this->_width) w = 1;
    if(x + w > this->_clip_x + this->_clip_w) return 0;
    return w;
}

static void _resize(ObjectPtr _this, int width, int height) {
    make_this(Buffer, _this);
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    if(width == this->_width && height == this->_height) return;

    size_t want = (size_t)width * (size_t)height;
    Cell * fresh = want ? pool_alloc(want * sizeof(Cell)) : NULL;
    if(want && fresh == NULL) return;   /* out of memory: keep the old grid */

    /* Copy the overlapping region across so a resize preserves what was drawn;
       every other cell starts empty, which the diff then repaints. */
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            Cell c;
            if(x < this->_width && y < this->_height) {
                c = this->_cells[y * this->_width + x];
            } else {
                c = cell_empty();
            }
            fresh[y * width + x] = c;
        }
    }
    if(this->_cells) pool_free(this->_cells);
    this->_cells = fresh;
    this->_width = width;
    this->_height = height;
    this->_clip_x = 0;
    this->_clip_y = 0;
    this->_clip_w = width;
    this->_clip_h = height;
}

static void _clear(ObjectPtr _this, uint16_t fg, uint16_t bg) {
    make_this(Buffer, _this);
    Cell blank = cell_blank(fg, bg);
    for(int y = 0; y < this->_height; ++y) {
        for(int x = 0; x < this->_width; ++x) {
            this->_cells[y * this->_width + x] = blank;
        }
    }
}

static void _set_cell(ObjectPtr _this, int x, int y, Cell c) {
    make_this(Buffer, _this);
    if(!_in_bounds(this, x, y)) return;   /* out of range is a no-op, not a fault */
    _put(this, x, y, c.ch, c.fg, c.bg, c.attrs);
}

static Cell _get_cell(ObjectPtr _this, int x, int y) {
    make_this(Buffer, _this);
    if(!_in_bounds(this, x, y)) return cell_empty();
    return this->_cells[y * this->_width + x];
}

/* Decodes one code point from UTF-8. Returns bytes consumed; a malformed or
   truncated sequence is consumed as one U+FFFD so a bad byte cannot stall the
   rest of the string. */
static int _decode_utf8(const char * s, int len, wchar_t * out) {
    unsigned char c0 = (unsigned char)s[0];
    if(c0 < 0x80) {
        *out = (wchar_t)c0;
        return 1;
    }
    int need;
    wchar_t v;
    if((c0 & 0xE0) == 0xC0)      { need = 1; v = c0 & 0x1Fu; }
    else if((c0 & 0xF0) == 0xE0) { need = 2; v = c0 & 0x0Fu; }
    else if((c0 & 0xF8) == 0xF0) { need = 3; v = c0 & 0x07u; }
    else                         { *out = 0xFFFD; return 1; }

    if(len < need + 1) { *out = 0xFFFD; return 1; }
    for(int i = 1; i <= need; ++i) {
        unsigned char ci = (unsigned char)s[i];
        if((ci & 0xC0) != 0x80) { *out = 0xFFFD; return 1; }
        v = (wchar_t)((v << 6) | (ci & 0x3Fu));
    }
    /* Reject overlong forms and surrogates, which would otherwise round-trip
       to a different code point than the input bytes describe. */
    static const wchar_t lowest[4] = { 0, 0x80, 0x800, 0x10000 };
    if(v < lowest[need] || (v >= 0xD800 && v <= 0xDFFF)) v = 0xFFFD;
    *out = v;
    return need + 1;
}

/* Walks a UTF-8 string, drawing one code point at a time with the pen set. */
static int _print_utf8(Buffer * this, int x, int y, const char * s, int len) {
    int i = 0;
    while(i < len) {
        wchar_t ch;
        i += _decode_utf8(s + i, len - i, &ch);
        int w = _advance(this, x, y, ch);
        if(w == 0) break;
        _put(this, x, y, ch, this->_pen_fg, this->_pen_bg, this->_pen_attrs);
        x += w;
    }
    return x;
}

static int _print_string(ObjectPtr _this, int x, int y, String * text) {
    make_this(Buffer, _this);
    if(text == NULL) return x;
    const char * s = call(text, to_cstring);
    return _print_utf8(this, x, y, s, (int)text->length);
}

static int _print(ObjectPtr _this, int x, int y, const char * fmt, ...) {
    make_this(Buffer, _this);
    char stack[512];
    va_list ap;
    va_start(ap, fmt);
    int need = vsnprintf(stack, sizeof stack, fmt, ap);
    va_end(ap);
    if(need < 0) return x;

    char * heap = NULL;
    const char * text = stack;
    if((size_t)need >= sizeof stack) {
        heap = malloc((size_t)need + 1);
        if(heap == NULL) return x;
        va_start(ap, fmt);
        vsnprintf(heap, (size_t)need + 1, fmt, ap);
        va_end(ap);
        text = heap;
    }
    int end = _print_utf8(this, x, y, text, need);
    if(heap) free(heap);
    return end;
}

static void _fill_rect(ObjectPtr _this, int x, int y, int w, int h, Cell c) {
    make_this(Buffer, _this);
    for(int j = 0; j < h; ++j) {
        for(int i = 0; i < w; ++i) {
            _put(this, x + i, y + j, c.ch, c.fg, c.bg, c.attrs);
        }
    }
}

static void _hline(ObjectPtr _this, int x, int y, int len, wchar_t ch) {
    make_this(Buffer, _this);
    for(int i = 0; i < len; ++i) {
        int w = _advance(this, x + i, y, ch);
        if(w == 0) break;
        _put(this, x + i, y, ch, this->_pen_fg, this->_pen_bg, this->_pen_attrs);
        i += w - 1;
    }
}

static void _vline(ObjectPtr _this, int x, int y, int len, wchar_t ch) {
    make_this(Buffer, _this);
    for(int i = 0; i < len; ++i) {
        _put(this, x, y + i, ch, this->_pen_fg, this->_pen_bg, this->_pen_attrs);
    }
}

static void _box(ObjectPtr _this, int x, int y, int w, int h, BoxStyle style, uint16_t fg, uint16_t bg) {
    make_this(Buffer, _this);
    if(w < 2 || h < 2) return;

    wchar_t tl, tr, bl, br, hz, vt;
    switch(style) {
        case BOX_DOUBLE:
            tl = 0x2554; tr = 0x2557; bl = 0x255A; br = 0x255D; hz = 0x2550; vt = 0x2551;
            break;
        case BOX_ROUNDED:
            tl = 0x256D; tr = 0x256E; bl = 0x2570; br = 0x256F; hz = 0x2500; vt = 0x2502;
            break;
        case BOX_SINGLE:
        default:
            tl = 0x250C; tr = 0x2510; bl = 0x2514; br = 0x2518; hz = 0x2500; vt = 0x2502;
            break;
    }

    int save_fg = this->_pen_fg, save_bg = this->_pen_bg, save_at = this->_pen_attrs;
    this->_pen_fg = fg;
    this->_pen_bg = bg;
    this->_pen_attrs = TUI_ATTR_NONE;

    _put(this, x,         y,         tl, fg, bg, TUI_ATTR_NONE);
    _put(this, x + w - 1, y,         tr, fg, bg, TUI_ATTR_NONE);
    _put(this, x,         y + h - 1, bl, fg, bg, TUI_ATTR_NONE);
    _put(this, x + w - 1, y + h - 1, br, fg, bg, TUI_ATTR_NONE);
    for(int i = 1; i < w - 1; ++i) {
        _put(this, x + i, y,         hz, fg, bg, TUI_ATTR_NONE);
        _put(this, x + i, y + h - 1, hz, fg, bg, TUI_ATTR_NONE);
    }
    for(int i = 1; i < h - 1; ++i) {
        _put(this, x,         y + i, vt, fg, bg, TUI_ATTR_NONE);
        _put(this, x + w - 1, y + i, vt, fg, bg, TUI_ATTR_NONE);
    }

    this->_pen_fg = save_fg;
    this->_pen_bg = save_bg;
    this->_pen_attrs = save_at;
}

static void _set_pen(ObjectPtr _this, uint16_t fg, uint16_t bg, uint8_t attrs) {
    make_this(Buffer, _this);
    this->_pen_fg = fg;
    this->_pen_bg = bg;
    this->_pen_attrs = attrs;
}

static void _set_clip(ObjectPtr _this, int x, int y, int w, int h) {
    make_this(Buffer, _this);
    /* Clamp to the grid so a caller can pass a generous rect without the
       renderer ever being told to address a cell that does not exist. */
    if(x < 0) { w += x; x = 0; }
    if(y < 0) { h += y; y = 0; }
    if(x + w > this->_width)  w = this->_width - x;
    if(y + h > this->_height) h = this->_height - y;
    if(w < 0) w = 0;
    if(h < 0) h = 0;
    this->_clip_x = x;
    this->_clip_y = y;
    this->_clip_w = w;
    this->_clip_h = h;
}

static void _clear_clip(ObjectPtr _this) {
    make_this(Buffer, _this);
    this->_clip_x = 0;
    this->_clip_y = 0;
    this->_clip_w = this->_width;
    this->_clip_h = this->_height;
}

static Cell * _cells(ObjectPtr _this) {
    make_this(Buffer, _this);
    return this->_cells;
}

Buffer * Buffer_new2(Buffer * this, int width, int height) {
    super(Object, Buffer);
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    this->_width = width;
    this->_height = height;
    this->_cells = NULL;
    this->_pen_fg = TUI_COLOR_DEFAULT;
    this->_pen_bg = TUI_COLOR_DEFAULT;
    this->_pen_attrs = TUI_ATTR_NONE;
    size_t want = (size_t)width * (size_t)height;
    if(want) this->_cells = pool_alloc(want * sizeof(Cell));
    /* pool_alloc does not zero, and _put inspects the wide-glyph flags of the
       cell it is about to overwrite, so every cell starts as a known blank. */
    if(this->_cells) {
        Cell blank = cell_blank(TUI_COLOR_DEFAULT, TUI_COLOR_DEFAULT);
        for(size_t i = 0; i < want; ++i) {
            this->_cells[i] = blank;
        }
    }
    _clear_clip((ObjectPtr)this);

    this->width = _width;
    this->height = _height;
    this->resize = _resize;
    this->clear = _clear;
    this->set_cell = _set_cell;
    this->get_cell = _get_cell;
    this->print = _print;
    this->print_string = _print_string;
    this->fill_rect = _fill_rect;
    this->hline = _hline;
    this->vline = _vline;
    this->box = _box;
    this->set_clip = _set_clip;
    this->clear_clip = _clear_clip;
    this->set_pen = _set_pen;
    this->cells = _cells;
    this->in_bounds = _in_bounds;
    return this;
}

void Buffer_delete(ObjectPtr _this) {
    make_this(Buffer, _this);
    if(this->_cells) {
        pool_free(this->_cells);
        this->_cells = NULL;
    }
    super_delete(Object, this);
}

#include "renderer.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

/* Output is assembled into a growing byte buffer and written once per present.
   One write per frame keeps the update atomic enough that the screen does not
   show a half-painted state, and it avoids a syscall per cell. */
typedef struct {
    char * data;
    size_t len;
    size_t cap;
    bool overflowed;
} OutBuf;

static bool _out_reserve(OutBuf * b, size_t extra) {
    if(b->overflowed) return false;
    if(b->len + extra + 1 <= b->cap) return true;
    size_t cap = b->cap ? b->cap : 4096;
    while(cap < b->len + extra + 1) {
        if(cap > (1u << 26)) {           /* refuse rather than grow without bound */
            b->overflowed = true;
            return false;
        }
        cap *= 2;
    }
    char * fresh = realloc(b->data, cap);
    if(fresh == NULL) {
        b->overflowed = true;
        return false;
    }
    b->data = fresh;
    b->cap = cap;
    return true;
}

static void _out_bytes(OutBuf * b, const char * s, size_t n) {
    if(!_out_reserve(b, n)) return;
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
}

static void _out_str(OutBuf * b, const char * s) {
    _out_bytes(b, s, strlen(s));
}

/* Moves the cursor using CUP, 1-based as the protocol requires. */
static void _out_cursor(OutBuf * b, int x, int y) {
    char seq[32];
    int n = snprintf(seq, sizeof seq, "\033[%d;%dH", y + 1, x + 1);
    if(n > 0) _out_bytes(b, seq, (size_t)n);
}

/* Emits the SGR run for a cell. Only the attributes that differ from the
   previous cell are sent, since a full run per cell is the bulk of the bytes
   in a coloured screen. */
static void _out_sgr(OutBuf * b, const Cell * c, Cell * last) {
    if(cell_equal(c, last)) return;
    char seq[TUI_SGR_MAX];
    size_t n;
    bool need = false;
    if(c->fg != last->fg) { n = tui_sgr_fg(seq, sizeof seq, c->fg);  _out_bytes(b, seq, n); need = true; }
    if(c->bg != last->bg) { n = tui_sgr_bg(seq, sizeof seq, c->bg);  _out_bytes(b, seq, n); need = true; }
    if(c->attrs != last->attrs) { n = tui_sgr_attrs(seq, sizeof seq, c->attrs); _out_bytes(b, seq, n); need = true; }
    if(need) {
        *last = *c;
    }
}

/* Appends one cell's glyph as UTF-8. A control or unencodable code point is
   emitted as a space so a stray byte cannot corrupt the cursor position. */
static void _out_glyph(OutBuf * b, wchar_t ch) {
    char utf8[4];
    if(ch <= 0) ch = CELL_FILL_CHAR;
    if(ch < 0x80) {
        utf8[0] = (char)ch;
        _out_bytes(b, utf8, 1);
        return;
    }
    if(ch < 0x800) {
        utf8[0] = (char)(0xC0 | (ch >> 6));
        utf8[1] = (char)(0x80 | (ch & 0x3F));
        _out_bytes(b, utf8, 2);
        return;
    }
    if(ch < 0x10000) {
        utf8[0] = (char)(0xE0 | (ch >> 12));
        utf8[1] = (char)(0x80 | ((ch >> 6) & 0x3F));
        utf8[2] = (char)(0x80 | (ch & 0x3F));
        _out_bytes(b, utf8, 3);
        return;
    }
    utf8[0] = (char)(0xF0 | (ch >> 18));
    utf8[1] = (char)(0x80 | ((ch >> 12) & 0x3F));
    utf8[2] = (char)(0x80 | ((ch >> 6) & 0x3F));
    utf8[3] = (char)(0x80 | (ch & 0x3F));
    _out_bytes(b, utf8, 4);
}

static void _flush(int fd, OutBuf * b) {
    if(b->data == NULL || b->len == 0) return;
    size_t off = 0;
    while(off < b->len) {
        ssize_t n = write(fd, b->data + off, b->len - off);
        if(n <= 0) break;                 /* a failed or partial write ends the frame */
        off += (size_t)n;
    }
}

static int _present(ObjectPtr _this, Buffer * back) {
    make_this(Renderer, _this);
    if(back == NULL) return 0;

    int w = call(back, width);
    int h = call(back, height);
    if(w <= 0 || h <= 0) return 0;

    /* A size change invalidates our record of the screen, so force a full
       repaint rather than diffing against a grid of the wrong shape. */
    if(w != call(this->_front, width) || h != call(this->_front, height)) {
        call(this->_front, resize, w, h);
        this->_force_full = true;
    }

    call(this->_diff, compute, this->_front, back, this->_force_full);
    int n = call(this->_diff, count);
    this->_force_full = false;

    if(n == 0) return 0;

    OutBuf out = { NULL, 0, 0, false };
    DiffPoint * pts = call(this->_diff, points);
    Cell * cells = call(this->_diff, cells);

    /* Cells are emitted in reading order so the cursor only ever moves
       forward, which avoids a wrap-around artefact at the right margin. */
    Cell last = cell_empty();
    last.fg = TUI_COLOR_DEFAULT;
    last.bg = TUI_COLOR_DEFAULT;
    int cur_x = -1, cur_y = -1;

    for(int i = 0; i < n; ++i) {
        int x = pts[i].x, y = pts[i].y;
        if(x != cur_x || y != cur_y) {
            _out_cursor(&out, x, y);
            cur_x = x;
            cur_y = y;
        }
        _out_sgr(&out, &cells[i], &last);
        _out_glyph(&out, cells[i].ch);
        /* A double-width glyph occupies the next cell too; the terminal
           advances the cursor by itself, so skip it to stay in sync. */
        cur_x = x + (cells[i].wide_lead ? 2 : 1);
    }
    _out_str(&out, "\033[0m");           /* leave a clean SGR state behind */

    _flush(this->_fd, &out);
    free(out.data);

    /* Adopt the new contents as the front so the next diff is against what the
       terminal now actually shows. */
    Cell * dst = call(this->_front, cells);
    Cell * src = call(back, cells);
    memcpy(dst, src, (size_t)w * (size_t)h * sizeof(Cell));

    return n;
}

static void _invalidate(ObjectPtr _this) {
    make_this(Renderer, _this);
    this->_force_full = true;
}

static void _set_cursor(ObjectPtr _this, int x, int y) {
    make_this(Renderer, _this);
    this->_cursor_x = x;
    this->_cursor_y = y;
    OutBuf out = { NULL, 0, 0, false };
    _out_cursor(&out, x, y);
    _flush(this->_fd, &out);
    free(out.data);
}

static void _show_cursor(ObjectPtr _this) {
    make_this(Renderer, _this);
    this->_cursor_visible = true;
    OutBuf out = { NULL, 0, 0, false };
    _out_str(&out, "\033[?25h");
    _flush(this->_fd, &out);
    free(out.data);
}

static void _hide_cursor(ObjectPtr _this) {
    make_this(Renderer, _this);
    this->_cursor_visible = false;
    OutBuf out = { NULL, 0, 0, false };
    _out_str(&out, "\033[?25l");
    _flush(this->_fd, &out);
    free(out.data);
}

static void _enter_alt_screen(ObjectPtr _this) {
    make_this(Renderer, _this);
    OutBuf out = { NULL, 0, 0, false };
    _out_str(&out, "\033[?1049h");       /* alternate screen buffer */
    _out_str(&out, "\033[2J\033[H");     /* clear and home */
    _flush(this->_fd, &out);
    free(out.data);
    this->_force_full = true;
}

static void _leave_alt_screen(ObjectPtr _this) {
    make_this(Renderer, _this);
    OutBuf out = { NULL, 0, 0, false };
    _out_str(&out, "\033[0m");           /* reset SGR before leaving */
    _out_str(&out, "\033[?25h");         /* never leave the cursor hidden */
    _out_str(&out, "\033[?1049l");       /* back to the primary screen */
    _flush(this->_fd, &out);
    free(out.data);
}

static void _clear_screen(ObjectPtr _this) {
    make_this(Renderer, _this);
    OutBuf out = { NULL, 0, 0, false };
    _out_str(&out, "\033[2J");
    _flush(this->_fd, &out);
    free(out.data);
    this->_force_full = true;
}

static void _set_title(ObjectPtr _this, String * title) {
    make_this(Renderer, _this);
    if(title == NULL) return;
    OutBuf out = { NULL, 0, 0, false };
    _out_str(&out, "\033]0;");
    _out_str(&out, call(title, to_cstring));
    _out_str(&out, "\007");
    _flush(this->_fd, &out);
    free(out.data);
}

static int _width(ObjectPtr _this) {
    make_this(Renderer, _this);
    return call(this->_front, width);
}

static int _height(ObjectPtr _this) {
    make_this(Renderer, _this);
    return call(this->_front, height);
}

Renderer * Renderer_new3(Renderer * this, int fd, int width, int height) {
    super(Object, Renderer);
    this->_fd = fd;
    this->_force_full = true;            /* the first present paints everything */
    this->_cursor_visible = true;
    this->_cursor_x = 0;
    this->_cursor_y = 0;
    this->_front = new(Buffer, width, height);
    this->_diff = new(BufferDiff);

    this->present = _present;
    this->invalidate = _invalidate;
    this->set_cursor = _set_cursor;
    this->show_cursor = _show_cursor;
    this->hide_cursor = _hide_cursor;
    this->enter_alt_screen = _enter_alt_screen;
    this->leave_alt_screen = _leave_alt_screen;
    this->clear_screen = _clear_screen;
    this->set_title = _set_title;
    this->width = _width;
    this->height = _height;
    return this;
}

void Renderer_delete(ObjectPtr _this) {
    make_this(Renderer, _this);
    REFCDEC(this->_front);
    REFCDEC(this->_diff);
    super_delete(Object, this);
}

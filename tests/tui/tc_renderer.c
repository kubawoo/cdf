#include "renderer.h"
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

/* The renderer writes to a descriptor, so a pipe gives a real end-to-end check
   without a terminal: bytes go in one end and can be asserted on the other. */
static int pipe_out[2];

/* Reads whatever has been written so far. The read end is non-blocking, so a
   check that expects no output returns an empty string instead of hanging. */
static void drain(char * out, size_t cap) {
    size_t total = 0;
    ssize_t n;
    while(total + 1 < cap && (n = read(pipe_out[0], out + total, cap - total - 1)) > 0) {
        total += (size_t)n;
    }
    out[total] = '\0';
}

static bool contains(const char * hay, const char * needle) {
    return strstr(hay, needle) != NULL;
}

static void renderer_writes_glyph_and_cursor(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 4, 2);
    Buffer * b = new(Buffer, 4, 2);
    call(b, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(b, set_cell, 1, 1, cell_make(L'A', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));

    int n = call(r, present, b);
    /* The first present is a full repaint: nothing is known to be on screen. */
    assert(n == 8);

    drain(buf, sizeof buf);
    /* A full repaint addresses each row once and lets the glyphs advance
       across it, so cell (1,1) is the second character of the second row. */
    assert(contains(buf, "\033[1;1H"));
    assert(contains(buf, "\033[2;1H"));
    assert(contains(buf, " A "));
    /* Every frame ends with an SGR reset. */
    assert(contains(buf, "\033[0m"));

    REFCDEC(b);
    REFCDEC(r);
}

static void renderer_second_present_writes_nothing(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 4, 2);
    Buffer * b = new(Buffer, 4, 2);
    call(b, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(b, set_cell, 0, 0, cell_make(L'B', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));

    assert(call(r, present, b) == 8);
    drain(buf, sizeof buf);

    /* Nothing changed, so the second frame must be empty: this is the whole
       point of keeping a front buffer. */
    assert(call(r, present, b) == 0);
    drain(buf, sizeof buf);
    assert(buf[0] == '\0');

    REFCDEC(b);
    REFCDEC(r);
}

static void renderer_emits_only_changed_cell(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 6, 2);
    Buffer * b = new(Buffer, 6, 2);
    call(b, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(r, present, b);
    drain(buf, sizeof buf);

    /* One edit on a settled screen produces one cell's worth of output. */
    call(b, set_cell, 4, 0, cell_make(L'C', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    assert(call(r, present, b) == 1);
    drain(buf, sizeof buf);
    assert(contains(buf, "\033[1;5H"));
    assert(contains(buf, "C"));

    REFCDEC(b);
    REFCDEC(r);
}

static void renderer_invalidate_forces_full_repaint(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 4, 1);
    Buffer * b = new(Buffer, 4, 1);
    call(b, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(r, present, b);
    drain(buf, sizeof buf);

    /* Without an invalidate the screen is considered settled. */
    assert(call(r, present, b) == 0);
    drain(buf, sizeof buf);

    call(r, invalidate);
    assert(call(r, present, b) == 4);

    REFCDEC(b);
    REFCDEC(r);
}

static void renderer_emits_colour_sequences(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 3, 1);
    Buffer * b = new(Buffer, 3, 1);
    call(b, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(b, set_cell, 0, 0, cell_make(L'Z', TUI_COLOR_RED, TUI_COLOR_BLUE, TUI_ATTR_BOLD));

    call(r, present, b);
    drain(buf, sizeof buf);
    /* Foreground, background and attributes each get their own SGR run. */
    assert(contains(buf, "\033[38;5;1m"));
    assert(contains(buf, "\033[48;5;4m"));
    assert(contains(buf, "\033[1m"));
    assert(contains(buf, "Z"));

    REFCDEC(b);
    REFCDEC(r);
}

static void renderer_encodes_utf8_glyphs(void)
{
    tui_init_utf8();
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 3, 1);
    Buffer * b = new(Buffer, 3, 1);
    call(b, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(b, set_cell, 0, 0, cell_make(0x2500, TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));

    call(r, present, b);
    drain(buf, sizeof buf);
    /* U+2500 BOX DRAWINGS LIGHT HORIZONTAL as three UTF-8 bytes. */
    assert(contains(buf, "\342\224\200"));

    REFCDEC(b);
    REFCDEC(r);
}

static void renderer_alt_screen_and_cursor(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 4, 2);

    call(r, enter_alt_screen);
    drain(buf, sizeof buf);
    assert(contains(buf, "\033[?1049h"));
    assert(contains(buf, "\033[2J"));

    call(r, hide_cursor);
    drain(buf, sizeof buf);
    assert(contains(buf, "\033[?25l"));

    /* Leaving the alternate screen must also restore the cursor, so a caller
       that forgot cannot leave the user with an invisible prompt. */
    call(r, leave_alt_screen);
    drain(buf, sizeof buf);
    assert(contains(buf, "\033[?25h"));
    assert(contains(buf, "\033[?1049l"));
    assert(contains(buf, "\033[0m"));

    REFCDEC(r);
}

static void renderer_set_cursor_position(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 10, 5);
    call(r, set_cursor, 3, 2);
    drain(buf, sizeof buf);
    assert(contains(buf, "\033[3;4H"));
    REFCDEC(r);
}

static void renderer_resize_forces_repaint(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 4, 1);
    Buffer * b = new(Buffer, 4, 1);
    call(b, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(r, present, b);
    drain(buf, sizeof buf);

    /* A buffer that changed size cannot be diffed against the old front, so the
       whole new area is emitted. */
    call(b, resize, 6, 2);
    assert(call(r, present, b) == 12);
    assert(call(r, width) == 6);
    assert(call(r, height) == 2);

    REFCDEC(b);
    REFCDEC(r);
}

static void renderer_null_buffer_is_safe(void)
{
    Renderer * r = new(Renderer, pipe_out[1], 4, 2);
    assert(call(r, present, NULL) == 0);
    REFCDEC(r);
}

static void renderer_set_title(void)
{
    char buf[4096];
    Renderer * r = new(Renderer, pipe_out[1], 4, 2);
    String * t = new(String, "demo");
    call(r, set_title, t);
    drain(buf, sizeof buf);
    assert(contains(buf, "\033]0;demo\007"));
    REFCDEC(t);
    REFCDEC(r);
}

int main(void)
{
    tui_init_utf8();
    if(pipe(pipe_out) != 0) return 1;
    if(fcntl(pipe_out[0], F_SETFL, O_NONBLOCK) != 0) return 1;

    renderer_writes_glyph_and_cursor();
    renderer_second_present_writes_nothing();
    renderer_emits_only_changed_cell();
    renderer_invalidate_forces_full_repaint();
    renderer_emits_colour_sequences();
    renderer_encodes_utf8_glyphs();
    renderer_alt_screen_and_cursor();
    renderer_set_cursor_position();
    renderer_resize_forces_repaint();
    renderer_null_buffer_is_safe();
    renderer_set_title();

    close(pipe_out[0]);
    close(pipe_out[1]);
    return 0;
}

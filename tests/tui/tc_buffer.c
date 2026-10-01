#include "buffer.h"
#include <assert.h>

static void buffer_dimensions(void)
{
    Buffer * b = new(Buffer, 10, 5);
    assert(call(b, width) == 10);
    assert(call(b, height) == 5);
    assert(type_equal(b, "Buffer"));
    REFCDEC(b);
}

static void buffer_set_and_get(void)
{
    Buffer * b = new(Buffer, 4, 3);
    Cell c = cell_make(L'X', TUI_COLOR_RED, TUI_COLOR_BLUE, TUI_ATTR_BOLD);
    call(b, set_cell, 2, 1, c);

    Cell got = call(b, get_cell, 2, 1);
    assert(got.ch == L'X');
    assert(got.fg == TUI_COLOR_RED);
    assert(got.bg == TUI_COLOR_BLUE);
    assert(got.attrs == TUI_ATTR_BOLD);
    REFCDEC(b);
}

static void buffer_out_of_range_is_noop(void)
{
    Buffer * b = new(Buffer, 4, 3);
    Cell c = cell_make(L'X', TUI_COLOR_RED, TUI_COLOR_BLACK, TUI_ATTR_NONE);

    /* Out-of-range writes are ignored rather than corrupting memory, and
       out-of-range reads return an empty cell. */
    call(b, set_cell, -1, 0, c);
    call(b, set_cell, 0, -1, c);
    call(b, set_cell, 99, 0, c);
    call(b, set_cell, 0, 99, c);

    /* An out-of-range read has no cell to return, so it yields an empty one
       rather than whatever happens to be in the grid. */
    assert(call(b, get_cell, 99, 99).ch == 0);
    assert(call(b, in_bounds, 3, 2));
    assert(!call(b, in_bounds, 4, 2));
    assert(!call(b, in_bounds, -1, 0));
    REFCDEC(b);
}

static void buffer_print_advances(void)
{
    Buffer * b = new(Buffer, 10, 2);
    int end = call(b, print, 0, 0, "abc");
    assert(end == 3);
    assert(call(b, get_cell, 0, 0).ch == L'a');
    assert(call(b, get_cell, 1, 0).ch == L'b');
    assert(call(b, get_cell, 2, 0).ch == L'c');
    REFCDEC(b);
}

static void buffer_print_formats(void)
{
    Buffer * b = new(Buffer, 20, 1);
    call(b, print, 0, 0, "%s=%d", "n", 42);
    assert(call(b, get_cell, 0, 0).ch == L'n');
    assert(call(b, get_cell, 1, 0).ch == L'=');
    assert(call(b, get_cell, 2, 0).ch == L'4');
    assert(call(b, get_cell, 3, 0).ch == L'2');
    REFCDEC(b);
}

static void buffer_print_string(void)
{
    Buffer * b = new(Buffer, 20, 1);
    String * s = new(String, "hey");
    int end = call(b, print_string, 1, 0, s);
    assert(end == 4);
    assert(call(b, get_cell, 1, 0).ch == L'h');
    assert(call(b, get_cell, 3, 0).ch == L'y');
    REFCDEC(s);
    REFCDEC(b);
}

static void buffer_print_long_string_overflows_to_heap(void)
{
    /* Longer than the internal stack buffer, so it exercises the heap path. */
    Buffer * b = new(Buffer, 400, 1);
    int end = call(b, print, 0, 0, "%090d", 7);
    assert(end == 90);
    assert(call(b, get_cell, 0, 0).ch == L'0');
    assert(call(b, get_cell, 89, 0).ch == L'7');
    REFCDEC(b);
}

static void buffer_print_clips_at_edge(void)
{
    Buffer * b = new(Buffer, 5, 1);
    /* The string runs past the right edge; the visible part is kept and the
       rest is discarded instead of writing out of bounds. */
    call(b, print, 3, 0, "abcdefgh");
    assert(call(b, get_cell, 3, 0).ch == L'a');
    assert(call(b, get_cell, 4, 0).ch == L'b');
    REFCDEC(b);
}

static void buffer_clip_rect(void)
{
    Buffer * b = new(Buffer, 10, 5);
    call(b, set_clip, 2, 1, 3, 2);

    /* Inside the clip: written. */
    call(b, set_cell, 3, 2, cell_make(L'i', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    assert(call(b, get_cell, 3, 2).ch == L'i');

    /* Outside the clip but inside the grid: silently dropped, so the cell
       still reads as the blank it started as. */
    call(b, set_cell, 0, 0, cell_make(L'o', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    assert(call(b, get_cell, 0, 0).ch == CELL_FILL_CHAR);

    /* Just past the clip edge: dropped too. */
    call(b, set_cell, 5, 2, cell_make(L'o', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    assert(call(b, get_cell, 5, 2).ch == CELL_FILL_CHAR);

    call(b, clear_clip);
    call(b, set_cell, 0, 0, cell_make(L'o', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    assert(call(b, get_cell, 0, 0).ch == L'o');
    REFCDEC(b);
}

static void buffer_clip_is_clamped_to_grid(void)
{
    Buffer * b = new(Buffer, 10, 5);
    /* A generous rect must be clamped so the renderer is never told to
       address a cell outside the grid. */
    call(b, set_clip, -5, -5, 1000, 1000);
    call(b, set_cell, 9, 4, cell_make(L'c', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    assert(call(b, get_cell, 9, 4).ch == L'c');
    REFCDEC(b);
}

static void buffer_resize_preserves_content(void)
{
    Buffer * b = new(Buffer, 6, 3);
    call(b, set_cell, 1, 1, cell_make(L'k', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    call(b, resize, 4, 2);
    assert(call(b, width) == 4);
    assert(call(b, height) == 2);
    /* The cell inside the overlapping region survives. */
    assert(call(b, get_cell, 1, 1).ch == L'k');
    REFCDEC(b);
}

static void buffer_clear(void)
{
    Buffer * b = new(Buffer, 3, 2);
    call(b, print, 0, 0, "xyz");
    call(b, clear, TUI_COLOR_GREEN, TUI_COLOR_BLACK);
    for(int y = 0; y < 2; ++y) {
        for(int x = 0; x < 3; ++x) {
            Cell c = call(b, get_cell, x, y);
            assert(c.ch == CELL_FILL_CHAR);
            assert(c.fg == TUI_COLOR_GREEN);
            assert(c.bg == TUI_COLOR_BLACK);
        }
    }
    REFCDEC(b);
}

static void buffer_lines_and_box(void)
{
    Buffer * b = new(Buffer, 6, 4);
    call(b, hline, 0, 0, 6, 0x2500);
    assert(call(b, get_cell, 0, 0).ch == 0x2500);
    assert(call(b, get_cell, 5, 0).ch == 0x2500);

    call(b, vline, 0, 1, 3, 0x2502);
    assert(call(b, get_cell, 0, 1).ch == 0x2502);
    assert(call(b, get_cell, 0, 3).ch == 0x2502);

    call(b, box, 0, 0, 4, 3, BOX_SINGLE, TUI_COLOR_CYAN, TUI_COLOR_BLACK);
    assert(call(b, get_cell, 0, 0).ch == 0x250C);      /* top-left */
    assert(call(b, get_cell, 3, 0).ch == 0x2510);      /* top-right */
    assert(call(b, get_cell, 0, 2).ch == 0x2514);      /* bottom-left */
    assert(call(b, get_cell, 3, 2).ch == 0x2518);      /* bottom-right */
    assert(call(b, get_cell, 0, 0).fg == TUI_COLOR_CYAN);
    REFCDEC(b);
}

static void buffer_box_too_small_is_ignored(void)
{
    Buffer * b = new(Buffer, 4, 4);
    call(b, box, 1, 1, 1, 1, BOX_DOUBLE, TUI_COLOR_RED, TUI_COLOR_BLACK);
    assert(call(b, get_cell, 1, 1).ch != 0x2554);
    REFCDEC(b);
}

static void buffer_fill_rect(void)
{
    Buffer * b = new(Buffer, 6, 3);
    call(b, fill_rect, 1, 1, 2, 2, cell_make(L'#', TUI_COLOR_YELLOW, TUI_COLOR_BLUE, TUI_ATTR_DIM));
    assert(call(b, get_cell, 1, 1).ch == L'#');
    assert(call(b, get_cell, 2, 2).ch == L'#');
    /* Cells outside the filled area are untouched. */
    assert(call(b, get_cell, 0, 0).ch == CELL_FILL_CHAR);
    assert(call(b, get_cell, 3, 2).ch == CELL_FILL_CHAR);
    REFCDEC(b);
}

static void buffer_wide_char_occupies_two_cells(void)
{
    /* A double-width glyph takes a lead cell plus a blank tail, so the
       following character lands two columns along. The locale is required for
       wcwidth to report a width of 2. */
    tui_init_utf8();
    Buffer * b = new(Buffer, 6, 1);
    int end = call(b, print, 0, 0, "\344\275\240\344\275\240");
    assert(end == 4);
    Cell lead = call(b, get_cell, 0, 0);
    Cell tail = call(b, get_cell, 1, 0);
    assert(lead.ch == 0x4F60);
    assert(lead.wide_lead);
    assert(tail.wide_tail);
    REFCDEC(b);
}

static void buffer_wide_char_at_right_edge_falls_back(void)
{
    /* There is no room for the second half at the edge, so the glyph degrades
       to a single blank cell rather than spilling over. */
    tui_init_utf8();
    Buffer * b = new(Buffer, 3, 1);
    call(b, print, 2, 0, "\344\275\240");
    Cell c = call(b, get_cell, 2, 0);
    assert(c.ch == CELL_FILL_CHAR);
    assert(!c.wide_lead);
    REFCDEC(b);
}

static void buffer_overwriting_wide_char_clears_partner(void)
{
    tui_init_utf8();
    Buffer * b = new(Buffer, 6, 1);
    call(b, print, 0, 0, "\344\275\240");
    assert(call(b, get_cell, 1, 0).wide_tail);

    /* Overwriting the tail must clear the lead, leaving no orphan. */
    call(b, set_cell, 1, 0, cell_make(L'z', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    assert(!call(b, get_cell, 0, 0).wide_lead);
    assert(call(b, get_cell, 1, 0).ch == L'z');
    REFCDEC(b);
}

static void buffer_pen_controls_print_colour(void)
{
    Buffer * b = new(Buffer, 5, 1);
    call(b, set_pen, TUI_COLOR_MAGENTA, TUI_COLOR_WHITE, TUI_ATTR_UNDERLINE);
    call(b, print, 0, 0, "hi");
    Cell c = call(b, get_cell, 0, 0);
    assert(c.fg == TUI_COLOR_MAGENTA);
    assert(c.bg == TUI_COLOR_WHITE);
    assert(c.attrs == TUI_ATTR_UNDERLINE);
    REFCDEC(b);
}

static void buffer_zero_size_is_legal(void)
{
    Buffer * b = new(Buffer, 0, 0);
    assert(call(b, width) == 0);
    assert(call(b, height) == 0);
    /* Drawing into an empty buffer is a no-op, not a fault. */
    call(b, print, 0, 0, "nothing");
    call(b, clear, TUI_COLOR_RED, TUI_COLOR_BLACK);
    call(b, resize, 3, 2);
    assert(call(b, width) == 3);
    REFCDEC(b);
}

int main(void)
{
    buffer_dimensions();
    buffer_set_and_get();
    buffer_out_of_range_is_noop();
    buffer_print_advances();
    buffer_print_formats();
    buffer_print_string();
    buffer_print_long_string_overflows_to_heap();
    buffer_print_clips_at_edge();
    buffer_clip_rect();
    buffer_clip_is_clamped_to_grid();
    buffer_resize_preserves_content();
    buffer_clear();
    buffer_lines_and_box();
    buffer_box_too_small_is_ignored();
    buffer_fill_rect();
    buffer_wide_char_occupies_two_cells();
    buffer_wide_char_at_right_edge_falls_back();
    buffer_overwriting_wide_char_clears_partner();
    buffer_pen_controls_print_colour();
    buffer_zero_size_is_legal();
    return 0;
}

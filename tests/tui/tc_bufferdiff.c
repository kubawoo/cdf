#include "bufferdiff.h"
#include <assert.h>

static void diff_identical_yields_nothing(void)
{
    Buffer * front = new(Buffer, 8, 4);
    Buffer * back  = new(Buffer, 8, 4);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    BufferDiff * d = new(BufferDiff);
    call(d, compute, front, back, false);
    assert(call(d, count) == 0);
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

static void diff_single_edit_yields_one_cell(void)
{
    Buffer * front = new(Buffer, 8, 4);
    Buffer * back  = new(Buffer, 8, 4);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    call(back, set_cell, 3, 2, cell_make(L'Q', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));

    BufferDiff * d = new(BufferDiff);
    call(d, compute, front, back, false);
    assert(call(d, count) == 1);

    DiffPoint * pts = call(d, points);
    Cell * cells = call(d, cells);
    assert(pts[0].x == 3);
    assert(pts[0].y == 2);
    assert(cells[0].ch == L'Q');
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

static void diff_colour_only_change_is_detected(void)
{
    Buffer * front = new(Buffer, 4, 2);
    Buffer * back  = new(Buffer, 4, 2);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    /* Same glyph, different colour: still a change that has to be emitted. */
    call(back, set_cell, 1, 0, cell_make(CELL_FILL_CHAR, TUI_COLOR_RED, TUI_COLOR_BLACK, TUI_ATTR_NONE));

    BufferDiff * d = new(BufferDiff);
    call(d, compute, front, back, false);
    assert(call(d, count) == 1);
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

static void diff_force_reports_every_cell(void)
{
    Buffer * front = new(Buffer, 5, 3);
    Buffer * back  = new(Buffer, 5, 3);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    /* A forced diff is what a resize needs, since the terminal's contents are
       no longer known to match either buffer. */
    BufferDiff * d = new(BufferDiff);
    call(d, compute, front, back, true);
    assert(call(d, count) == 15);
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

static void diff_size_mismatch_forces_full(void)
{
    Buffer * front = new(Buffer, 4, 2);
    Buffer * back  = new(Buffer, 6, 3);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    BufferDiff * d = new(BufferDiff);
    /* Differing geometry cannot be compared cell by cell, so the whole back
       buffer is reported rather than a partial, wrong set of cells. */
    call(d, compute, front, back, false);
    assert(call(d, count) == 18);
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

static void diff_recompute_resets_previous_result(void)
{
    Buffer * front = new(Buffer, 6, 2);
    Buffer * back  = new(Buffer, 6, 2);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    BufferDiff * d = new(BufferDiff);
    call(back, set_cell, 0, 0, cell_make(L'a', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    call(back, set_cell, 1, 0, cell_make(L'b', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
    call(d, compute, front, back, false);
    assert(call(d, count) == 2);

    /* A second identical diff must find nothing, not repeat the first result. */
    call(d, compute, front, back, false);
    assert(call(d, count) == 2);
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

static void diff_skips_wide_tails(void)
{
    tui_init_utf8();
    Buffer * front = new(Buffer, 6, 1);
    Buffer * back  = new(Buffer, 6, 1);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    call(back, print, 0, 0, "\344\275\240");

    BufferDiff * d = new(BufferDiff);
    call(d, compute, front, back, false);
    /* The lead cell is emitted; its blank tail would double-write. */
    assert(call(d, count) == 1);
    Cell * cells = call(d, cells);
    assert(cells[0].ch == 0x4F60);
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

static void diff_null_back_is_empty(void)
{
    BufferDiff * d = new(BufferDiff);
    call(d, compute, NULL, NULL, false);
    assert(call(d, count) == 0);
    REFCDEC(d);
}

static void diff_grows_beyond_initial_capacity(void)
{
    /* Larger than the initial allocation, so the growth path is exercised. */
    Buffer * front = new(Buffer, 64, 64);
    Buffer * back  = new(Buffer, 64, 64);
    call(front, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back,  clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(back, set_cell, 60, 60, cell_make(L'Z', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));

    BufferDiff * d = new(BufferDiff);
    call(d, compute, front, back, false);
    assert(call(d, count) == 1);
    DiffPoint * pts = call(d, points);
    assert(pts[0].x == 60);
    assert(pts[0].y == 60);
    REFCDEC(d);
    REFCDEC(front);
    REFCDEC(back);
}

int main(void)
{
    diff_identical_yields_nothing();
    diff_single_edit_yields_one_cell();
    diff_colour_only_change_is_detected();
    diff_force_reports_every_cell();
    diff_size_mismatch_forces_full();
    diff_recompute_resets_previous_result();
    diff_skips_wide_tails();
    diff_null_back_is_empty();
    diff_grows_beyond_initial_capacity();
    return 0;
}

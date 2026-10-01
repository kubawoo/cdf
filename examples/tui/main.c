/* A small demo of the cdf tui module: a bordered panel with a movable marker.
   Arrow keys move it, 'q' or Escape quits. Nothing here touches a terminal
   directly — everything goes through the module. */

#include <core.h>
#include <tui.h>
#include <stdio.h>

typedef struct {
    Buffer * screen;
    int x;
    int y;
    int keys;
} Demo;

static void draw(Demo * d) {
    int w = call(d->screen, width);
    int h = call(d->screen, height);

    call(d->screen, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    /* A frame inset by one cell so the border is never clipped. */
    call(d->screen, box, 0, 0, w, h, BOX_ROUNDED, TUI_COLOR_CYAN, TUI_COLOR_BLACK);

    call(d->screen, set_pen, TUI_COLOR_BRIGHT_WHITE, TUI_COLOR_BLACK, TUI_ATTR_BOLD);
    call(d->screen, print, 2, 1, " cdf tui demo ");

    call(d->screen, set_pen, TUI_COLOR_GREEN, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(d->screen, print, 2, 3, "Arrows move the marker, q quits.");
    call(d->screen, print, 2, 4, "Keys pressed: %d", d->keys);

    /* A wide glyph, to show the double-width handling. */
    call(d->screen, set_pen, TUI_COLOR_YELLOW, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(d->screen, print, 2, 6, "CJK: \344\275\240\344\275\240\344\275\240");

    call(d->screen, set_pen, TUI_COLOR_BRIGHT_MAGENTA, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(d->screen, set_cell, d->x, d->y,
         cell_make(0x25CF, TUI_COLOR_BRIGHT_MAGENTA, TUI_COLOR_BLACK, TUI_ATTR_NONE));

    call(d->screen, set_pen, TUI_COLOR_BRIGHT_BLACK, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(d->screen, print, 2, h - 2, "%dx%d", w, h);
}

static bool on_event(Event * ev, ObjectPtr user_data) {
    Demo * d = (Demo *)user_data;

    if(ev->type == EVENT_RESIZE) {
        /* The loop has already resized the buffer and invalidated the renderer,
           so the frame must be rebuilt from scratch. */
        draw(d);
        return true;
    }
    if(ev->type == EVENT_QUIT) {
        return false;
    }
    if(ev->key.key == KEY_CHAR) {
        d->keys++;
        if(ev->key.ch == L'q' || ev->key.ch == L'Q') {
            return false;
        }
    }

    int w = call(d->screen, width);
    int h = call(d->screen, height);
    switch(ev->key.key) {
        case KEY_LEFT:  if(d->x > 1) d->x--; break;
        case KEY_RIGHT: if(d->x < w - 2) d->x++; break;
        case KEY_UP:    if(d->y > 7) d->y--; break;
        case KEY_DOWN:  if(d->y < h - 3) d->y++; break;
        case KEY_ESCAPE: return false;
        case KEY_RESIZE: return true;
        default: break;
    }
    /* Redraw here rather than after the loop returns: the loop presents the
       buffer once per event, so whatever this handler leaves in the buffer is
       what reaches the screen. */
    draw(d);
    return true;
}

int main(void) {
    Terminal * term = new(Terminal);
    if(!call(term, open)) {
        fprintf(stderr, "tui demo: not a terminal, nothing to do\n");
        REFCDEC(term);
        return 1;
    }

    int w = 0, h = 0;
    call(term, get_size, &w, &h);
    if(w < 20 || h < 10) {
        w = 80;
        h = 24;
    }

    Renderer * rend = new(Renderer, call(term, out_fd), w, h);
    call(rend, set_title, new(String, "cdf tui"));
    call(rend, enter_alt_screen);
    call(rend, hide_cursor);

    Demo demo = { new(Buffer, w, h), w / 2, h / 2, 0 };
    draw(&demo);
    call(rend, present, demo.screen);

    EventLoop * loop = new(EventLoop);
    call(loop, run, term, rend, demo.screen, on_event, &demo);

    /* Always put the terminal back, whatever happened in the loop. */
    call(rend, leave_alt_screen);
    call(term, close);

    REFCDEC(demo.screen);
    REFCDEC(loop);
    REFCDEC(rend);
    REFCDEC(term);
    return 0;
}

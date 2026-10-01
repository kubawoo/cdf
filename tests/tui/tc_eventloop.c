/* End-to-end test of the event loop driving a renderer over a pipe. No terminal
   is involved: bytes are written into one end of a pipe and the loop reads them
   as though they were keystrokes. */

#include "eventloop.h"
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static int in_pipe[2];      /* keystrokes go in here */
static int out_pipe[2];     /* frames come out here  */
static char last_frame[8192];

static void drain(void) {
    size_t total = 0;
    ssize_t n;
    while(total + 1 < sizeof last_frame
            && (n = read(out_pipe[0], last_frame + total, sizeof last_frame - total - 1)) > 0) {
        total += (size_t)n;
    }
    last_frame[total] = '\0';
}

static bool contains(const char * hay, const char * needle) {
    return strstr(hay, needle) != NULL;
}

typedef struct {
    Buffer * screen;
    int x;
    int arrows;
} App;

static void draw(App * a) {
    call(a->screen, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    call(a->screen, set_cell, a->x, 0,
         cell_make(0x25CF, TUI_COLOR_RED, TUI_COLOR_BLACK, TUI_ATTR_NONE));
}

/* Mirrors the demo's handler: move the marker, then redraw. The redraw is the
   part that matters -- the loop only calls present(), so a handler that moves
   state without redrawing leaves nothing for the screen to change. */
static bool on_event(Event * ev, ObjectPtr user_data) {
    App * a = (App *)user_data;

    if(ev->type == EVENT_RESIZE) {
        draw(a);
        return true;
    }

    switch(ev->key.key) {
        case KEY_RIGHT: a->arrows++; if(a->x < 2) a->x++; break;
        case KEY_ESCAPE: return false;
        default: break;
    }
    draw(a);
    return true;
}

static void loop_delivers_arrow_keys_and_quits(void)
{
    App a = { new(Buffer, 4, 1), 0, 0 };

    Terminal * term = new(Terminal, in_pipe[0], out_pipe[1]);
    Renderer * rend = new(Renderer, out_pipe[1], 4, 1);
    EventLoop * loop = new(EventLoop);

    draw(&a);
    call(rend, present, a.screen);
    drain();

    /* Three right arrows and an escape: the escape stops the loop. */
    static const char keys[] = "\033[C\033[C\033[C\033";
    assert(write(in_pipe[1], keys, sizeof keys - 1) == (ssize_t)(sizeof keys - 1));

    int handled = call(loop, run, term, rend, a.screen, on_event, &a);
    drain();

    assert(handled == 4);              /* three arrows plus the escape */
    assert(a.arrows == 3);              /* the arrows were decoded, not dropped */
    assert(a.x == 2);                   /* movement was applied */

    /* U+25CF BLACK CIRCLE, encoded by the renderer as UTF-8. */
    static const char marker[] = "\342\227\217";
    assert(contains(last_frame, marker));

    REFCDEC(loop);
    REFCDEC(rend);
    REFCDEC(term);
    REFCDEC(a.screen);
}

static void loop_reports_end_of_input_as_quit(void)
{
    App a = { new(Buffer, 4, 1), 0, 0 };

    Terminal * term = new(Terminal, in_pipe[0], out_pipe[1]);
    Renderer * rend = new(Renderer, out_pipe[1], 4, 1);
    EventLoop * loop = new(EventLoop);

    /* The write end is closed, so the read end signals EOF. The loop must turn
       that into a quit rather than spinning. */
    close(in_pipe[1]);
    int handled = call(loop, run, term, rend, a.screen, on_event, &a);
    assert(handled == 1);

    REFCDEC(loop);
    REFCDEC(rend);
    REFCDEC(term);
    REFCDEC(a.screen);
}

int main(void)
{
    tui_init_utf8();
    if(pipe(in_pipe) != 0) return 1;
    if(pipe(out_pipe) != 0) return 1;
    if(fcntl(out_pipe[0], F_SETFL, O_NONBLOCK) != 0) return 1;

    loop_delivers_arrow_keys_and_quits();
    loop_reports_end_of_input_as_quit();

    return 0;
}

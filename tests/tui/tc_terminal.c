#include "terminal.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

static void terminal_constructs_with_explicit_fds(void)
{
    Terminal * t = new(Terminal, 3, 4);
    assert(call(t, in_fd) == 3);
    assert(call(t, out_fd) == 4);
    assert(type_equal(t, "Terminal"));
    REFCDEC(t);
}

static void terminal_default_fds(void)
{
    Terminal * t = new(Terminal);
    assert(call(t, in_fd) == STDIN_FILENO);
    assert(call(t, out_fd) == STDOUT_FILENO);
    REFCDEC(t);
}

static void terminal_open_refuses_non_tty(void)
{
    /* Pointing at a pipe means there is no terminal mode to set. open() must
       report that rather than half-initialising, and the rest of the module
       must stay usable against the same descriptors. */
    int fds[2];
    assert(pipe(fds) == 0);

    Terminal * t = new(Terminal, fds[0], fds[1]);
    assert(call(t, open) == false);
    assert(call(t, is_open) == false);

    /* close() on a terminal that never opened is still safe. */
    call(t, close);
    assert(call(t, is_open) == false);

    REFCDEC(t);
    close(fds[0]);
    close(fds[1]);
}

static void terminal_size_falls_back_to_environment(void)
{
    int fds[2];
    assert(pipe(fds) == 0);

    setenv("COLUMNS", "132", 1);
    setenv("LINES", "43", 1);

    Terminal * t = new(Terminal, fds[0], fds[1]);
    /* A pipe has no winsize, so the environment supplies the answer. */
    int w = 0, h = 0;
    call(t, get_size, &w, &h);
    assert(w == 132);
    assert(h == 43);
    assert(call(t, get_width) == 132);
    assert(call(t, get_height) == 43);

    REFCDEC(t);
    unsetenv("COLUMNS");
    unsetenv("LINES");
    close(fds[0]);
    close(fds[1]);
}

static void terminal_size_falls_back_to_default(void)
{
    int fds[2];
    assert(pipe(fds) == 0);

    unsetenv("COLUMNS");
    unsetenv("LINES");

    Terminal * t = new(Terminal, fds[0], fds[1]);
    int w = 0, h = 0;
    call(t, get_size, &w, &h);
    assert(w == TUI_DEFAULT_WIDTH);
    assert(h == TUI_DEFAULT_HEIGHT);

    REFCDEC(t);
    close(fds[0]);
    close(fds[1]);
}

static void terminal_size_ignores_nonsense_environment(void)
{
    int fds[2];
    assert(pipe(fds) == 0);

    /* A non-numeric or non-positive value must not be taken as a size. */
    setenv("COLUMNS", "not-a-number", 1);
    setenv("LINES", "-5", 1);

    Terminal * t = new(Terminal, fds[0], fds[1]);
    int w = 0, h = 0;
    call(t, get_size, &w, &h);
    assert(w == TUI_DEFAULT_WIDTH);
    assert(h == TUI_DEFAULT_HEIGHT);

    REFCDEC(t);
    unsetenv("COLUMNS");
    unsetenv("LINES");
    close(fds[0]);
    close(fds[1]);
}

static void terminal_resize_flag(void)
{
    Terminal * t = new(Terminal);
    /* Nothing has signalled yet. */
    assert(call(t, size_changed) == false);
    /* Clearing an unset flag is a no-op, not a fault. */
    call(t, clear_resize);
    assert(call(t, size_changed) == false);
    REFCDEC(t);
}

static void terminal_resize_handler_is_idempotent(void)
{
    /* Installing twice must not lose the previous handler or leak state. */
    tui_terminal_install_resize_handler();
    tui_terminal_install_resize_handler();
    tui_terminal_restore_resize_handler();
    tui_terminal_restore_resize_handler();
}

static void terminal_delete_is_safe_when_never_opened(void)
{
    Terminal * t = new(Terminal, 99, 98);
    /* The destructor restores the terminal state; with nothing to restore it
       must not touch the invalid descriptors. */
    REFCDEC(t);
}

int main(void)
{
    terminal_constructs_with_explicit_fds();
    terminal_default_fds();
    terminal_open_refuses_non_tty();
    terminal_size_falls_back_to_environment();
    terminal_size_falls_back_to_default();
    terminal_size_ignores_nonsense_environment();
    terminal_resize_flag();
    terminal_resize_handler_is_idempotent();
    terminal_delete_is_safe_when_never_opened();
    return 0;
}

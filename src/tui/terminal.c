#include "terminal.h"
#include "cell.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/* Saved so close() can put the user's terminal back exactly as it was. */
static struct termios g_saved_termios;
static bool g_saved_valid = false;
static volatile sig_atomic_t g_resize_flag = 0;
static struct sigaction g_prev_winch;
static bool g_winch_installed = false;

static void _winch_handler(int signum) {
    (void)signum;
    /* Only a flag is set here. Doing real work in signal context is unsafe, so
       the event loop picks the resize up between iterations. */
    g_resize_flag = 1;
}

void tui_terminal_install_resize_handler(void) {
    if(g_winch_installed) return;
    struct sigaction sa;
    sa.sa_handler = _winch_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;                 /* no SA_RESTART: poll should return EINTR */
    if(sigaction(SIGWINCH, &sa, &g_prev_winch) == 0) {
        g_winch_installed = true;
    }
}

void tui_terminal_restore_resize_handler(void) {
    if(!g_winch_installed) return;
    sigaction(SIGWINCH, &g_prev_winch, NULL);
    g_winch_installed = false;
}

static int _env_int(const char * name, int fallback) {
    const char * v = getenv(name);
    if(v == NULL || *v == '\0') return fallback;
    char * end = NULL;
    long n = strtol(v, &end, 10);
    if(end == v || n <= 0 || n > 100000) return fallback;
    return (int)n;
}

static bool _open(ObjectPtr _this) {
    make_this(Terminal, _this);
    if(this->_is_open) return true;

    /* Double-width glyphs only occupy the right number of columns once a UTF-8
       locale is active, so select one before anything is measured or drawn. */
    tui_init_utf8();

    /* Without a real terminal there is no mode to set and no size to query, so
       refuse rather than half-initialising. Callers check is_open() and can
       still use the renderer against a pipe or a file. */
    if(!isatty(this->_in_fd) || !isatty(this->_out_fd)) {
        return false;
    }
    if(tcgetattr(this->_in_fd, &g_saved_termios) != 0) {
        return false;
    }
    g_saved_valid = true;

    struct termios raw = g_saved_termios;
    cfmakeraw(&raw);
    /* Keep output post-processing so a newline still returns the carriage,
       which many TUI callers rely on for simple line output. */
    raw.c_oflag |= OPOST | ONLCR;
    if(tcsetattr(this->_in_fd, TCSANOW, &raw) != 0) {
        g_saved_valid = false;
        return false;
    }
    this->_raw_active = true;
    this->_is_open = true;
    tui_terminal_install_resize_handler();
    return true;
}

static void _close(ObjectPtr _this) {
    make_this(Terminal, _this);
    if(this->_raw_active && g_saved_valid) {
        tcsetattr(this->_in_fd, TCSANOW, &g_saved_termios);
    }
    this->_raw_active = false;
    this->_is_open = false;
    tui_terminal_restore_resize_handler();
}

static bool _is_open(ObjectPtr _this) {
    make_this(Terminal, _this);
    return this->_is_open;
}

static bool _get_size(ObjectPtr _this, int * w, int * h) {
    make_this(Terminal, _this);
    struct winsize ws;
    if(ioctl(this->_out_fd, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0 && ws.ws_row > 0) {
        if(w) *w = ws.ws_col;
        if(h) *h = ws.ws_row;
        return true;
    }
    /* No ioctl answer: fall back to the environment, then to a conventional
       80x24, so a headless run still has a usable grid. */
    int ew = _env_int("COLUMNS", TUI_DEFAULT_WIDTH);
    int eh = _env_int("LINES", TUI_DEFAULT_HEIGHT);
    if(w) *w = ew;
    if(h) *h = eh;
    return false;
}

static int _get_width(ObjectPtr _this) {
    int w = TUI_DEFAULT_WIDTH, h = TUI_DEFAULT_HEIGHT;
    _get_size(_this, &w, &h);
    return w;
}

static int _get_height(ObjectPtr _this) {
    int w = TUI_DEFAULT_WIDTH, h = TUI_DEFAULT_HEIGHT;
    _get_size(_this, &w, &h);
    return h;
}

static bool _size_changed(ObjectPtr _this) {
    (void)_this;
    return g_resize_flag != 0;
}

static void _clear_resize(ObjectPtr _this) {
    make_this(Terminal, _this);
    g_resize_flag = 0;
    this->_size_dirty = false;
}

static int _in_fd(ObjectPtr _this) {
    make_this(Terminal, _this);
    return this->_in_fd;
}

static int _out_fd(ObjectPtr _this) {
    make_this(Terminal, _this);
    return this->_out_fd;
}

Terminal * Terminal_new2(Terminal * this, int in_fd, int out_fd) {
    super(Object, Terminal);
    this->_in_fd = in_fd;
    this->_out_fd = out_fd;
    this->_is_open = false;
    this->_raw_active = false;
    this->_size_dirty = false;

    this->open = _open;
    this->close = _close;
    this->is_open = _is_open;
    this->get_size = _get_size;
    this->get_width = _get_width;
    this->get_height = _get_height;
    this->size_changed = _size_changed;
    this->clear_resize = _clear_resize;
    this->in_fd = _in_fd;
    this->out_fd = _out_fd;
    return this;
}

Terminal * Terminal_new(Terminal * this) {
    return Terminal_new2(this, STDIN_FILENO, STDOUT_FILENO);
}

void Terminal_delete(ObjectPtr _this) {
    make_this(Terminal, _this);
    _close(_this);
    super_delete(Object, this);
}

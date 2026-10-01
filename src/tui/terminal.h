#ifndef CDF_TUI_TERMINAL_H
#define CDF_TUI_TERMINAL_H

#include "../core/core.h"

#define TUI_DEFAULT_WIDTH  80
#define TUI_DEFAULT_HEIGHT 24

typedef struct {
    inherits(Object);

    /* Takes the descriptors to drive rather than assuming stdin/stdout, which
       is what lets a test point the terminal at a pair of pipes. */
    bool (*open)(ObjectPtr);
    void (*close)(ObjectPtr);
    bool (*is_open)(ObjectPtr);

    /* Returns false when the size could not be determined from either the
       ioctl or the environment, in which case w/h are left untouched. */
    bool (*get_size)(ObjectPtr, int * w, int * h);
    int (*get_width)(ObjectPtr);
    int (*get_height)(ObjectPtr);

    /* True once a SIGWINCH has arrived; the event loop clears it with
       clear_resize() and resizes at a safe point rather than in the handler. */
    bool (*size_changed)(ObjectPtr);
    void (*clear_resize)(ObjectPtr);

    int (*in_fd)(ObjectPtr);
    int (*out_fd)(ObjectPtr);

    //'private'
    int _in_fd;
    int _out_fd;
    bool _is_open;
    bool _raw_active;
    bool _size_dirty;
} Terminal;

Terminal * Terminal_new(Terminal * this);
Terminal * Terminal_new2(Terminal * this, int in_fd, int out_fd);
void Terminal_delete(ObjectPtr);

/* Process-wide, because a terminal is shared state: the SIGWINCH handler has
   to set a flag the loop reads, and only one Terminal may be open at a time. */
void tui_terminal_install_resize_handler(void);
void tui_terminal_restore_resize_handler(void);

#endif

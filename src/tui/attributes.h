#ifndef CDF_TUI_ATTRIBUTES_H
#define CDF_TUI_ATTRIBUTES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* The xterm 256-colour palette: 0-7 standard, 8-15 bright, 16-255 extended.
   TUI_COLOR_DEFAULT leaves the terminal's own foreground/background alone. */
#define TUI_COLOR_DEFAULT 0xFFFF

typedef enum {
    TUI_COLOR_BLACK = 0,
    TUI_COLOR_RED,
    TUI_COLOR_GREEN,
    TUI_COLOR_YELLOW,
    TUI_COLOR_BLUE,
    TUI_COLOR_MAGENTA,
    TUI_COLOR_CYAN,
    TUI_COLOR_WHITE,
    TUI_COLOR_BRIGHT_BLACK = 8,
    TUI_COLOR_BRIGHT_RED,
    TUI_COLOR_BRIGHT_GREEN,
    TUI_COLOR_BRIGHT_YELLOW,
    TUI_COLOR_BRIGHT_BLUE,
    TUI_COLOR_BRIGHT_MAGENTA,
    TUI_COLOR_BRIGHT_CYAN,
    TUI_COLOR_BRIGHT_WHITE
} TuiColor;

typedef enum {
    TUI_ATTR_NONE     = 0,
    TUI_ATTR_BOLD     = 1 << 0,
    TUI_ATTR_DIM      = 1 << 1,
    TUI_ATTR_ITALIC   = 1 << 2,
    TUI_ATTR_UNDERLINE= 1 << 3,
    TUI_ATTR_BLINK    = 1 << 4,
    TUI_ATTR_REVERSE  = 1 << 5
} TuiAttr;

/* Each emitter appends an SGR sequence to out, NUL-terminating when cap > 0.
   Returns the number of bytes written, not counting the terminator, so a
   caller can size a buffer from the return values without guessing. */
size_t tui_sgr_fg(char * out, size_t cap, uint16_t fg);
size_t tui_sgr_bg(char * out, size_t cap, uint16_t bg);
size_t tui_sgr_attrs(char * out, size_t cap, uint8_t attrs);
size_t tui_sgr_reset(char * out, size_t cap);

/* Worst-case bytes for one complete SGR run, including the terminator. */
#define TUI_SGR_MAX 48

#endif

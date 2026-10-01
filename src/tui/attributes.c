#include "attributes.h"
#include <stdio.h>
#include <string.h>

/* Builds one SGR body ("38;5;n" and friends) into tmp, then appends it wrapped
   in ESC[ ... m to out. Both steps go through a bounded copy so a short
   destination is truncated rather than overflowed. */
static size_t _emit(char * out, size_t cap, const char * body) {
    char seq[TUI_SGR_MAX];
    int n = snprintf(seq, sizeof seq, "\033[%sm", body);
    if(n < 0) return 0;
    size_t len = (size_t)n;
    if(len >= sizeof seq) len = sizeof seq - 1;
    if(out == NULL || cap == 0) return len;
    size_t copy = len < cap - 1 ? len : cap - 1;
    memcpy(out, seq, copy);
    out[copy] = '\0';
    return len;
}

size_t tui_sgr_fg(char * out, size_t cap, uint16_t fg) {
    if(fg == TUI_COLOR_DEFAULT) return 0;   /* terminal's own foreground */
    char body[16];
    snprintf(body, sizeof body, "38;5;%u", (unsigned)fg);
    return _emit(out, cap, body);
}

size_t tui_sgr_bg(char * out, size_t cap, uint16_t bg) {
    if(bg == TUI_COLOR_DEFAULT) return 0;
    char body[16];
    snprintf(body, sizeof body, "48;5;%u", (unsigned)bg);
    return _emit(out, cap, body);
}

size_t tui_sgr_attrs(char * out, size_t cap, uint8_t attrs) {
    /* One SGR run carrying every flag at once, rather than a sequence per bit:
       fewer bytes on the wire and a single reset point for the renderer. */
    char body[32];
    int n = 0;
    body[0] = '\0';
    if(attrs & TUI_ATTR_BOLD)      n += snprintf(body + n, sizeof body - n, "%s1",      n ? ";" : "");
    if(attrs & TUI_ATTR_DIM)       n += snprintf(body + n, sizeof body - n, "%s2",      n ? ";" : "");
    if(attrs & TUI_ATTR_ITALIC)    n += snprintf(body + n, sizeof body - n, "%s3",      n ? ";" : "");
    if(attrs & TUI_ATTR_UNDERLINE) n += snprintf(body + n, sizeof body - n, "%s4",      n ? ";" : "");
    if(attrs & TUI_ATTR_BLINK)     n += snprintf(body + n, sizeof body - n, "%s5",      n ? ";" : "");
    if(attrs & TUI_ATTR_REVERSE)   n += snprintf(body + n, sizeof body - n, "%s7",      n ? ";" : "");
    if(n == 0) return 0;
    return _emit(out, cap, body);
}

size_t tui_sgr_reset(char * out, size_t cap) {
    return _emit(out, cap, "0");
}

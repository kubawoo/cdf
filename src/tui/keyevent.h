#ifndef CDF_TUI_KEYEVENT_H
#define CDF_TUI_KEYEVENT_H

#include "../core/core.h"

typedef enum {
    KEY_NONE = 0,
    KEY_CHAR,      /* a printable code point, carried in KeyEvent.ch */
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_HOME,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_INSERT,
    KEY_DELETE,
    KEY_BACKSPACE,
    KEY_ENTER,
    KEY_TAB,
    KEY_BACK_TAB,
    KEY_ESCAPE,
    KEY_RESIZE,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12
} Key;

typedef enum {
    MOD_NONE  = 0,
    MOD_SHIFT = 1 << 0,
    MOD_ALT   = 1 << 1,
    MOD_CTRL  = 1 << 2
} KeyModifiers;

typedef struct {
    Key key;
    KeyModifiers mods;
    wchar_t ch;    /* the code point for KEY_CHAR, else L'\0' */
} KeyEvent;

typedef enum {
    EVENT_KEY,
    EVENT_RESIZE,
    EVENT_QUIT
} EventType;

typedef struct {
    EventType type;
    KeyEvent key;
    int width;      /* EVENT_RESIZE only */
    int height;
} Event;

/* Decodes the first event in buf.
   Returns the number of bytes consumed: 0 when the bytes are an incomplete
   sequence and the caller should read more before trying again, or -1 when
   buf is empty. Leaving the caller to retain unconsumed bytes is what lets an
   escape sequence split across two reads still be recognised. */
int key_decode(const char * buf, int len, Event * out);
const char * key_name(Key key);

#endif

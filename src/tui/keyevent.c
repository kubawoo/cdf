#include "keyevent.h"
#include <string.h>

#define ESC '\033'

static int _digits_value(const char * s, int n) {
    int v = 0;
    for(int i = 0; i < n; ++i) {
        v = v * 10 + (s[i] - '0');
    }
    return v;
}

/* Maps the final byte of a CSI sequence to a key, given the leading numeric
   parameter. For a tilde sequence that is the key code ("3~" is Delete); for a
   letter final xterm sends "1;<mods><final>", where the code is unused.
   Returns KEY_NONE if unrecognised. */
static Key _csi_final(char final, int code) {
    if(final == '~') {
        switch(code) {
            case 1:  return KEY_HOME;
            case 2:  return KEY_INSERT;
            case 3:  return KEY_DELETE;
            case 4:  return KEY_END;
            case 5:  return KEY_PAGE_UP;
            case 6:  return KEY_PAGE_DOWN;
            case 7:  return KEY_HOME;
            case 8:  return KEY_END;
            case 11: case 12: case 13: case 14:
                      return (Key)(KEY_F1 + (code - 11));
            case 15: return KEY_F5;
            case 17: case 18: case 19: case 20: case 21:
                      return (Key)(KEY_F6 + (code - 17));
            case 23: case 24: return KEY_F11 + (code - 23);
            default: return KEY_NONE;
        }
    }
    switch(final) {
        case 'A': return KEY_UP;
        case 'B': return KEY_DOWN;
        case 'C': return KEY_RIGHT;
        case 'D': return KEY_LEFT;
        case 'H': return KEY_HOME;
        case 'F': return KEY_END;
        case 'P': return KEY_F1;
        case 'Q': return KEY_F2;
        case 'R': return KEY_F3;
        case 'S': return KEY_F4;
        case 'Z': return KEY_BACK_TAB;
        default:  return KEY_NONE;
    }
}

/* Decodes the xterm modifier mask, where bit 0 is shift, 1 alt and 2 ctrl.
   Applied only to letter finals, which is where xterm sends "1;<mask><final>".
   The parameter is one more than the bitmask, so the 1 is removed here. */
static KeyModifiers _decode_mods(int param) {
    int m = param - 1;
    KeyModifiers r = MOD_NONE;
    if(m & 1) r |= MOD_SHIFT;
    if(m & 2) r |= MOD_ALT;
    if(m & 4) r |= MOD_CTRL;
    return r;
}

/* Decodes one UTF-8 code point. Returns bytes consumed, or 0 when the buffer
   ends mid-sequence, which the caller must treat as "need more input". */
static int _utf8(const char * s, int len, wchar_t * out) {
    unsigned char c0 = (unsigned char)s[0];
    if(c0 < 0x80) {
        *out = (wchar_t)c0;
        return 1;
    }
    int need;
    wchar_t v;
    if((c0 & 0xE0) == 0xC0)      { need = 1; v = c0 & 0x1Fu; }
    else if((c0 & 0xF0) == 0xE0) { need = 2; v = c0 & 0x0Fu; }
    else if((c0 & 0xF8) == 0xF0) { need = 3; v = c0 & 0x07u; }
    else                         { *out = 0xFFFD; return 1; }
    if(len < need + 1) return 0;
    for(int i = 1; i <= need; ++i) {
        unsigned char ci = (unsigned char)s[i];
        if((ci & 0xC0) != 0x80) { *out = 0xFFFD; return 1; }
        v = (wchar_t)((v << 6) | (ci & 0x3Fu));
    }
    *out = v;
    return need + 1;
}

int key_decode(const char * buf, int len, Event * out) {
    if(buf == NULL || len <= 0) return -1;

    unsigned char c0 = (unsigned char)buf[0];

    if(c0 == ESC) {
        /* A lone ESC with nothing after it yet may be either the Escape key or
           the start of a sequence. We consume it as Escape immediately: waiting
           would make a bare Escape key indistinguishable from a split sequence,
           and callers that care can debounce on their own. */
        if(len == 1) {
            out->type = EVENT_KEY;
            out->key.key = KEY_ESCAPE;
            out->key.mods = MOD_NONE;
            out->key.ch = 0;
            return 1;
        }
        if(buf[1] == '[') {
            /* CSI: ESC [ params final. The parameter field is bounded so that
               corrupt input cannot make us wait forever for a terminator. */
            int i = 2;
            while(i < len && ((buf[i] >= '0' && buf[i] <= '9') || buf[i] == ';')) {
                i++;
                if(i > 18) break;
            }
            if(i >= len) return 0;          /* sequence not complete yet */
            char final = buf[i];

            /* Parameters are ';' separated: "5~" is key code 5, while "1;5A"
               is a plain arrow with the modifier mask 5. Only a letter final
               treats the second parameter as a modifier. */
            int code = 0;
            int mask = -1;
            int run_start = 2;
            int first = 0;
            bool have_first = false;
            while(run_start <= i) {
                int run_end = run_start;
                while(run_end < i && buf[run_end] >= '0' && buf[run_end] <= '9') run_end++;
                if(run_end == run_start) break;   /* no digits in this run */
                if(!have_first) {
                    first = _digits_value(buf + run_start, run_end - run_start);
                    have_first = true;
                } else {
                    int v = _digits_value(buf + run_start, run_end - run_start);
                    if(mask < 0) mask = v;
                }
                if(run_end < i && buf[run_end] == ';') { run_start = run_end + 1; continue; }
                break;
            }
            code = first;

            KeyModifiers mods = MOD_NONE;
            Key k;
            if(final == '~') {
                k = _csi_final(final, code);
            } else {
                mods = (mask >= 0) ? _decode_mods(mask) : MOD_NONE;
                k = _csi_final(final, code);
            }
            out->type = EVENT_KEY;
            out->key.key = k;
            out->key.mods = mods;
            out->key.ch = 0;
            return i + 1;
        }
        if(buf[1] == 'O') {
            /* SS3, used by the application cursor mode: ESC O <final>. */
            if(len < 3) return 0;
            out->type = EVENT_KEY;
            out->key.key = _csi_final(buf[2], 0);
            out->key.mods = MOD_NONE;
            out->key.ch = 0;
            return 3;
        }
        /* ESC followed by a printable byte is Alt+that key. */
        wchar_t ch;
        int used = _utf8(buf + 1, len - 1, &ch);
        if(used == 0) return 0;
        out->type = EVENT_KEY;
        out->key.key = KEY_CHAR;
        out->key.mods = MOD_ALT;
        out->key.ch = ch;
        return 1 + used;
    }

    switch(c0) {
        case '\r':
        case '\n':
            out->type = EVENT_KEY;
            out->key.key = KEY_ENTER;
            out->key.mods = MOD_NONE;
            out->key.ch = 0;
            return 1;
        case '\t':
            out->type = EVENT_KEY;
            out->key.key = KEY_TAB;
            out->key.mods = MOD_NONE;
            out->key.ch = 0;
            return 1;
        case 0x7F:
        case '\b':
            out->type = EVENT_KEY;
            out->key.key = KEY_BACKSPACE;
            out->key.mods = MOD_NONE;
            out->key.ch = 0;
            return 1;
        default:
            break;
    }

    if(c0 < 0x20) {
        /* Remaining C0 controls map onto Ctrl+letter. */
        out->type = EVENT_KEY;
        out->key.key = KEY_CHAR;
        out->key.mods = MOD_CTRL;
        out->key.ch = (wchar_t)(c0 + 'a' - 1);
        return 1;
    }

    wchar_t ch;
    int used = _utf8(buf, len, &ch);
    if(used == 0) return 0;      /* partial UTF-8: wait for the rest */

    KeyModifiers mods = MOD_NONE;
    if(ch >= L'A' && ch <= L'Z') {
        mods = MOD_SHIFT;
        ch = (wchar_t)(ch - L'A' + L'a');
    }
    out->type = EVENT_KEY;
    out->key.key = KEY_CHAR;
    out->key.mods = mods;
    out->key.ch = ch;
    return used;
}

const char * key_name(Key key) {
    switch(key) {
        case KEY_NONE:      return "none";
        case KEY_CHAR:      return "char";
        case KEY_UP:        return "up";
        case KEY_DOWN:      return "down";
        case KEY_LEFT:      return "left";
        case KEY_RIGHT:     return "right";
        case KEY_HOME:      return "home";
        case KEY_END:       return "end";
        case KEY_PAGE_UP:   return "pageup";
        case KEY_PAGE_DOWN: return "pagedown";
        case KEY_INSERT:    return "insert";
        case KEY_DELETE:    return "delete";
        case KEY_BACKSPACE: return "backspace";
        case KEY_ENTER:     return "enter";
        case KEY_TAB:       return "tab";
        case KEY_BACK_TAB:  return "backtab";
        case KEY_ESCAPE:    return "escape";
        case KEY_RESIZE:    return "resize";
        case KEY_F1:  return "f1";
        case KEY_F2:  return "f2";
        case KEY_F3:  return "f3";
        case KEY_F4:  return "f4";
        case KEY_F5:  return "f5";
        case KEY_F6:  return "f6";
        case KEY_F7:  return "f7";
        case KEY_F8:  return "f8";
        case KEY_F9:  return "f9";
        case KEY_F10: return "f10";
        case KEY_F11: return "f11";
        case KEY_F12: return "f12";
        default:      return "?";
    }
}

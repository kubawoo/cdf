#include "keyevent.h"
#include <assert.h>
#include <string.h>

static void key_plain_char(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    int n = key_decode("a", 1, &e);
    assert(n == 1);
    assert(e.type == EVENT_KEY);
    assert(e.key.key == KEY_CHAR);
    assert(e.key.mods == MOD_NONE);
    assert(e.key.ch == L'a');
}

static void key_shift_is_normalised(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    /* An uppercase letter arrives as itself; the decoder reports it as
       lowercase with the shift modifier set. */
    int n = key_decode("Q", 1, &e);
    assert(n == 1);
    assert(e.key.key == KEY_CHAR);
    assert(e.key.mods == MOD_SHIFT);
    assert(e.key.ch == L'q');
}

static void key_arrows(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    assert(key_decode("\033[A", 3, &e) == 3);
    assert(e.key.key == KEY_UP);
    assert(key_decode("\033[B", 3, &e) == 3);
    assert(e.key.key == KEY_DOWN);
    assert(key_decode("\033[C", 3, &e) == 3);
    assert(e.key.key == KEY_RIGHT);
    assert(key_decode("\033[D", 3, &e) == 3);
    assert(e.key.key == KEY_LEFT);
    assert(e.key.mods == MOD_NONE);
}

static void key_tilde_sequences(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    assert(key_decode("\033[5~", 4, &e) == 4);
    assert(e.key.key == KEY_PAGE_UP);
    assert(e.key.mods == MOD_NONE);

    assert(key_decode("\033[6~", 4, &e) == 4);
    assert(e.key.key == KEY_PAGE_DOWN);

    /* The numeric parameter here is a key code, not a modifier mask. */
    assert(key_decode("\033[3~", 4, &e) == 4);
    assert(e.key.key == KEY_DELETE);
    assert(e.key.mods == MOD_NONE);

    assert(key_decode("\033[2~", 4, &e) == 4);
    assert(e.key.key == KEY_INSERT);

    assert(key_decode("\033[4~", 4, &e) == 4);
    assert(e.key.key == KEY_END);
}

static void key_modified_arrows(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    /* "1;<mask><final>": the mask is a bitfield, shift=1 alt=2 ctrl=4. */
    assert(key_decode("\033[1;2A", 6, &e) == 6);
    assert(e.key.key == KEY_UP);
    assert(e.key.mods == MOD_SHIFT);

    assert(key_decode("\033[1;5C", 6, &e) == 6);
    assert(e.key.key == KEY_RIGHT);
    assert(e.key.mods == MOD_CTRL);

    assert(key_decode("\033[1;3B", 6, &e) == 6);
    assert(e.key.key == KEY_DOWN);
    assert(e.key.mods == MOD_ALT);
}

static void key_function_keys(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    assert(key_decode("\033[11~", 5, &e) == 5);
    assert(e.key.key == KEY_F1);
    assert(key_decode("\033[15~", 5, &e) == 5);
    assert(e.key.key == KEY_F5);

    /* SS3 form used by the application cursor mode. */
    assert(key_decode("\033OP", 3, &e) == 3);
    assert(e.key.key == KEY_F1);
}

static void key_control_chars(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    assert(key_decode("\r", 1, &e) == 1);
    assert(e.key.key == KEY_ENTER);

    assert(key_decode("\t", 1, &e) == 1);
    assert(e.key.key == KEY_TAB);

    assert(key_decode("\033[Z", 3, &e) == 3);
    assert(e.key.key == KEY_BACK_TAB);

    assert(key_decode("\177", 1, &e) == 1);
    assert(e.key.key == KEY_BACKSPACE);

    /* A bare C0 control becomes Ctrl+letter. */
    assert(key_decode("\003", 1, &e) == 1);
    assert(e.key.key == KEY_CHAR);
    assert(e.key.mods == MOD_CTRL);
    assert(e.key.ch == L'c');
}

static void key_lone_escape(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    assert(key_decode("\033", 1, &e) == 1);
    assert(e.key.key == KEY_ESCAPE);
}

static void key_alt_prefix(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    assert(key_decode("\033a", 2, &e) == 2);
    assert(e.key.key == KEY_CHAR);
    assert(e.key.mods == MOD_ALT);
    assert(e.key.ch == L'a');
}

static void key_incomplete_waits_for_more(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    /* A partial sequence must report "need more input" rather than decoding
       a truncated key, so the caller retains the bytes. */
    assert(key_decode("\033[", 2, &e) == 0);
    assert(key_decode("\033[5", 3, &e) == 0);
    assert(key_decode("\033O", 2, &e) == 0);

    /* A partial UTF-8 sequence behaves the same way. */
    assert(key_decode("\342\202", 2, &e) == 0);

    /* Empty input is distinct: there is nothing to wait for. */
    assert(key_decode("", 0, &e) == -1);
    assert(key_decode(NULL, 0, &e) == -1);
}

static void key_utf8_multibyte(void)
{
    Event e;
    memset(&e, 0, sizeof e);

    /* U+20AC EURO SIGN, encoded in three bytes. */
    assert(key_decode("\342\202\254", 3, &e) == 3);
    assert(e.key.key == KEY_CHAR);
    assert(e.key.ch == 0x20AC);
}

static void key_names(void)
{
    assert(strcmp(key_name(KEY_UP), "up") == 0);
    assert(strcmp(key_name(KEY_ESCAPE), "escape") == 0);
    assert(strcmp(key_name(KEY_F12), "f12") == 0);
    assert(strcmp(key_name(KEY_NONE), "none") == 0);
}

int main(void)
{
    key_plain_char();
    key_shift_is_normalised();
    key_arrows();
    key_tilde_sequences();
    key_modified_arrows();
    key_function_keys();
    key_control_chars();
    key_lone_escape();
    key_alt_prefix();
    key_incomplete_waits_for_more();
    key_utf8_multibyte();
    key_names();
    return 0;
}

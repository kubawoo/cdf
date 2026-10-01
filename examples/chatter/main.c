/* chatter — a terminal chat client for an OpenAI-compatible endpoint.

   Type a message and press Enter to send it. The request runs on a worker
   thread, so the interface keeps redrawing and Ctrl-C keeps working while the
   model is thinking; the reply is appended when it arrives whole.

   Uses the tui, http and json modules. Only the terminal handling comes from
   the tui module — the worker thread is plain <threads.h>. */

#include <core.h>
#include <tui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include "chat.h"

#define DEFAULT_HOST "http://localhost:8080/"
#define MAX_TOKENS  1024

/* A line of the conversation, already wrapped for display. */
typedef enum {
    LINE_USER,
    LINE_ASSISTANT,
    LINE_SYSTEM
} LineRole;

typedef struct {
    LineRole role;
    char * text;
} Line;

typedef struct {
    Buffer * screen;
    ChatClient * chat;

    Line * lines;
    int line_count;
    int line_capacity;

    /* Offset of the first visible line, for scrolling back through history. */
    int scroll;

    char * input;
    int input_len;
    int input_capacity;

    bool busy;             /* a request is in flight */
    bool thinking;          /* let the model deliberate (set by -t) */
    mtx_t lock;            /* guards busy/reply/error between the threads */
    String * reply;        /* handed over by the worker */
    String * error;

    thrd_t worker;
    bool worker_running;
    char * url;
    char * model;
} App;

/* ---- line storage ------------------------------------------------------- */

/* Appends len bytes as one display row. Taking an explicit length matters: a
   row may contain a space in the middle but never an embedded newline. */
static void lines_push_n(App * app, LineRole role, const char * text, int len) {
    if(app->line_count == app->line_capacity) {
        app->line_capacity = app->line_capacity ? app->line_capacity * 2 : 32;
        app->lines = realloc(app->lines, sizeof(Line) * app->line_capacity);
    }
    Line * line = &app->lines[app->line_count++];
    line->role = role;
    line->text = malloc((size_t) len + 1);
    memcpy(line->text, text, (size_t) len);
    line->text[len] = '\0';
}

/* Splits text into display lines no wider than width, breaking on spaces so a
   word is not cut in half. Newlines in the text start a new display line, and
   each one is wrapped on its own: a row may only be as wide as the screen, or
   the terminal wraps it again and leaves fragments of the message behind. */
static void lines_push_wrapped(App * app, LineRole role, const char * text, int width) {
    if(width < 8) width = 8;

    const char * line_start = text;
    for(const char * p = text; ; ++p) {
        if(*p != '\n' && *p != '\0') continue;

        /* p is the end of one source line, or the end of the text. */
        int len = (int) (p - line_start);
        const char * q = line_start;

        while(true) {
            if(len <= width) {
                lines_push_n(app, role, q, len);
                break;
            }

            /* Break on the last space that fits, so a word is not split. */
            int take = width;
            for(int i = width; i > 0; --i) {
                if(q[i] == ' ') {
                    take = i;
                    break;
                }
            }
            lines_push_n(app, role, q, take);
            q += take;
            len -= take;
            while(len > 0 && *q == ' ') {
                q++;
                len--;
            }
        }

        if(*p == '\0') break;
        line_start = p + 1;
    }
}

static void lines_free(App * app) {
    for(int i = 0; i < app->line_count; ++i) free(app->lines[i].text);
    free(app->lines);
    app->lines = NULL;
    app->line_count = 0;
    app->line_capacity = 0;
}

/* ---- worker thread ------------------------------------------------------ */

static int worker_main(void * arg) {
    App * app = arg;

    String * err = new(String);
    String * reply = chat_send(app->chat, MAX_TOKENS, err);

    mtx_lock(&app->lock);
    REFCDEC(app->error);
    /* Only a failed request has something to report. Handing over an empty
       string on success would add a blank line to the transcript every time. */
    app->error = reply == NULL ? err : NULL;
    if(reply != NULL) REFCDEC(err);
    app->reply = reply;
    app->busy = false;
    mtx_unlock(&app->lock);

    return 0;
}

/* Reads a field the worker also writes. Going through the lock is what makes
   the read meaningful: without it the compiler may hoist the read out of the
   poll loop, and the UI would never notice the reply arriving. */
static bool app_busy(App * app) {
    mtx_lock(&app->lock);
    bool busy = app->busy;
    mtx_unlock(&app->lock);
    return busy;
}

static void start_request(App * app) {
    mtx_lock(&app->lock);
    REFCDEC(app->reply);
    app->reply = NULL;
    REFCDEC(app->error);
    app->error = NULL;
    app->busy = true;
    mtx_unlock(&app->lock);

    if(thrd_create(&app->worker, worker_main, app) == thrd_success) {
        app->worker_running = true;
    } else {
        mtx_lock(&app->lock);
        app->busy = false;
        String * msg = new(String, "Could not start the request thread.");
        REFCDEC(app->error);
        app->error = msg;
        mtx_unlock(&app->lock);
    }
}

/* Moves anything the worker produced into the transcript. Called from the UI
   thread only, so no drawing races with the worker. */
static void collect_result(App * app, int width) {
    if(app_busy(app)) return;

    if(app->worker_running) {
        thrd_join(app->worker, NULL);
        app->worker_running = false;
    }

    if(app->reply != NULL) {
        chat_add_turn(app->chat, "assistant", call(app->reply, to_cstring));
        lines_push_wrapped(app, LINE_ASSISTANT, call(app->reply, to_cstring), width);
        REFCDEC(app->reply);
        app->reply = NULL;
    }
    if(app->error != NULL) {
        lines_push_wrapped(app, LINE_SYSTEM, call(app->error, to_cstring), width);
        REFCDEC(app->error);
        app->error = NULL;
    }
}

/* ---- input line --------------------------------------------------------- */

static void input_push(App * app, wchar_t ch) {
    char utf8[8];
    int len = 0;
    /* Re-encode the code point the decoder produced. */
    if(ch < 0x80) {
        utf8[len++] = (char) ch;
    } else if(ch < 0x800) {
        utf8[len++] = (char) (0xC0 | (ch >> 6));
        utf8[len++] = (char) (0x80 | (ch & 0x3F));
    } else if(ch < 0x10000) {
        utf8[len++] = (char) (0xE0 | (ch >> 12));
        utf8[len++] = (char) (0x80 | ((ch >> 6) & 0x3F));
        utf8[len++] = (char) (0x80 | (ch & 0x3F));
    } else {
        utf8[len++] = (char) (0xF0 | (ch >> 18));
        utf8[len++] = (char) (0x80 | ((ch >> 12) & 0x3F));
        utf8[len++] = (char) (0x80 | ((ch >> 6) & 0x3F));
        utf8[len++] = (char) (0x80 | (ch & 0x3F));
    }

    if(app->input_len + len + 1 > app->input_capacity) {
        app->input_capacity = (app->input_len + len + 1) * 2;
        app->input = realloc(app->input, (size_t) app->input_capacity);
    }
    memcpy(app->input + app->input_len, utf8, (size_t) len);
    app->input_len += len;
    app->input[app->input_len] = '\0';
}

/* Removes one UTF-8 character, so a backspace deletes a whole code point
   rather than one byte. */
static void input_backspace(App * app) {
    if(app->input_len == 0) return;
    int end = app->input_len - 1;
    while(end > 0 && ((unsigned char) app->input[end] & 0xC0) == 0x80) end--;
    app->input_len = end;
    app->input[end] = '\0';
}

static void input_clear(App * app) {
    app->input_len = 0;
    if(app->input) app->input[0] = '\0';
}

/* ---- drawing ------------------------------------------------------------ */

/* Rows reserved for the rule, the hint and the input prompt. */
#define FOOTER_ROWS 3
#define MARGIN      2

/* Usable width for the transcript, given the screen the app is drawing on. */
static int body_width(App * app) {
    int w = call(app->screen, width);
    int body_w = w - MARGIN * 2;
    return body_w < 8 ? 8 : body_w;
}

static void draw(App * app) {
    int w = call(app->screen, width);
    int h = call(app->screen, height);

    call(app->screen, clear, TUI_COLOR_WHITE, TUI_COLOR_BLACK);

    call(app->screen, set_pen, TUI_COLOR_BRIGHT_CYAN, TUI_COLOR_BLACK, TUI_ATTR_BOLD);
    call(app->screen, print, MARGIN, 0, " chatter ");

    call(app->screen, set_pen, TUI_COLOR_BRIGHT_BLACK, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(app->screen, print, MARGIN + 10, 0, "%s  %s%s", app->model, app->url,
         app->thinking ? "  (thinking)" : "");

    call(app->screen, hline, MARGIN, 1, w - MARGIN - 1, 0x2500);

    int body_top = 2;
    int body_rows = h - body_top - FOOTER_ROWS;
    if(body_rows < 1) body_rows = 1;

    /* Show the tail of the transcript, or the scrolled window if the user has
       paged up. */
    int first = app->line_count - body_rows;
    if(app->scroll > 0) {
        first = app->line_count - body_rows - app->scroll;
    }
    if(first < 0) first = 0;

    int row = body_top;
    for(int i = first; i < app->line_count && row < body_top + body_rows; ++i) {
        uint16_t fg = TUI_COLOR_WHITE;
        const char * label = "";
        switch(app->lines[i].role) {
            case LINE_USER:      fg = TUI_COLOR_BRIGHT_GREEN; label = "you  "; break;
            case LINE_ASSISTANT: fg = TUI_COLOR_BRIGHT_BLUE;  label = "bot  "; break;
            case LINE_SYSTEM:    fg = TUI_COLOR_BRIGHT_RED;   label = "!!   "; break;
        }
        call(app->screen, set_pen, fg, TUI_COLOR_BLACK, TUI_ATTR_NONE);
        call(app->screen, print, MARGIN, row, "%s%s", label, app->lines[i].text);
        row++;
    }

    /* A reply in flight shows a placeholder so the wait is legible. */
    bool busy = app_busy(app);
    if(busy && row < body_top + body_rows) {
        call(app->screen, set_pen, TUI_COLOR_BRIGHT_YELLOW, TUI_COLOR_BLACK, TUI_ATTR_ITALIC);
        call(app->screen, print, MARGIN, row, "bot  thinking...");
    }

    int input_row = h - FOOTER_ROWS;
    call(app->screen, hline, MARGIN, input_row, w - MARGIN - 1, 0x2500);

    const char * hint = busy
        ? "Enter sends  |  Ctrl-C quits  (a reply is on its way)"
        : "Enter sends  |  PgUp/PgDn scroll  |  Ctrl-U clears input  |  Ctrl-C quits";
    call(app->screen, set_pen, TUI_COLOR_BRIGHT_BLACK, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(app->screen, print, MARGIN, input_row + 1, "%s", hint);
    /* The prompt reflects state: you cannot send while a request is running. */
    const char * prompt = busy ? "... " : "> ";
    uint16_t pfg = busy ? TUI_COLOR_BRIGHT_YELLOW : TUI_COLOR_BRIGHT_GREEN;
    call(app->screen, set_pen, pfg, TUI_COLOR_BLACK, TUI_ATTR_BOLD);
    int used = call(app->screen, print, MARGIN, input_row + 2, "%s", prompt);

    /* Keep the cursor inside the visible part of the line. */
    int avail = w - MARGIN - used - 1;
    const char * text = app->input ? app->input : "";
    int len = (int) strlen(text);
    int start = 0;
    if(avail > 0 && len > avail) start = len - avail;

    call(app->screen, set_pen, TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(app->screen, print, MARGIN + used, input_row + 2, "%s", text + start);

    /* Park the real cursor after the last character of the input. */
    int col = MARGIN + used + (len - start);
    if(col > w - 1) col = w - 1;
    int cursor_y = input_row + 2;
    call(app->screen, set_pen, TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE);
    call(app->screen, set_cell, col, cursor_y,
         cell_make(' ', TUI_COLOR_WHITE, TUI_COLOR_BLACK, TUI_ATTR_NONE));
}

/* ---- event handling ----------------------------------------------------- */

static bool on_event(Event * ev, ObjectPtr user_data) {
    App * app = user_data;

    if(ev->type == EVENT_QUIT) return false;

    if(ev->key.mods & MOD_CTRL) {
        switch(ev->key.ch) {
            case 'c': return false;
            case 'u': input_clear(app); draw(app); return true;
            case 'k': lines_free(app); app->scroll = 0; draw(app); return true;
            default: return true;
        }
    }

    int body_w = body_width(app);

    /* Results may have arrived since the last key. */
    collect_result(app, body_w);

    switch(ev->key.key) {
        case KEY_ESCAPE:
            return false;
        case KEY_PAGE_UP:
            app->scroll += 5;
            if(app->scroll > app->line_count) app->scroll = app->line_count;
            draw(app);
            return true;
        case KEY_PAGE_DOWN:
            app->scroll -= 5;
            if(app->scroll < 0) app->scroll = 0;
            draw(app);
            return true;
        case KEY_UP:
            app->scroll += 1;
            if(app->scroll > app->line_count) app->scroll = app->line_count;
            draw(app);
            return true;
        case KEY_DOWN:
            app->scroll -= 1;
            if(app->scroll < 0) app->scroll = 0;
            draw(app);
            return true;
        case KEY_BACKSPACE:
            input_backspace(app);
            draw(app);
            return true;
        case KEY_ENTER: {
            if(app_busy(app)) {
                draw(app);
                return true;
            }
            const char * text = app->input ? app->input : "";
            if(text[0] == '\0') {
                draw(app);
                return true;
            }
            chat_add_turn(app->chat, "user", text);
            lines_push_wrapped(app, LINE_USER, text, body_w);
            app->scroll = 0;
            input_clear(app);
            start_request(app);
            draw(app);
            return true;
        }
        case KEY_CHAR:
            input_push(app, ev->key.ch);
            draw(app);
            return true;
        default:
            break;
    }
    return true;
}

/* ---- main loop ----------------------------------------------------------- */

/* EventLoop.run() waits for a key with no timeout, so the screen would sit
   showing "thinking..." until the user happened to press a key. A chat request
   is a long background wait, so the loop is driven here instead: poll for a
   short while, and repaint between events so a reply that arrives on its own
   shows up straight away. */
#define POLL_TIMEOUT_MS 100

static void on_resize(App * app, Terminal * term, Buffer * screen, Renderer * rend) {
    int w = 0, h = 0;
    call(term, get_size, &w, &h);
    if(w < 40) w = 40;
    if(h < 10) h = 10;
    call(screen, resize, w, h);
    call(rend, invalidate);

    int body_w = body_width(app);

    /* Re-wrap the transcript at the new width, keeping the text intact. */
    LineRole * roles = malloc(sizeof(LineRole) * (size_t) app->line_count);
    char ** texts = malloc(sizeof(char *) * (size_t) app->line_count);
    int n = app->line_count;
    for(int i = 0; i < n; ++i) {
        roles[i] = app->lines[i].role;
        texts[i] = app->lines[i].text;
    }
    app->line_count = 0;
    for(int i = 0; i < n; ++i) {
        lines_push_wrapped(app, roles[i], texts[i], body_w);
    }
    for(int i = 0; i < n; ++i) free(texts[i]);
    free(texts);
    free(roles);

    draw(app);
}

/* ---- command line -------------------------------------------------------- */

/* Settings gathered from the command line before anything is opened. */
typedef struct {
    const char * host;
    const char * model;      /* NULL until the user names one */
    const char * token;      /* NULL when the server needs no authentication */
    bool thinking;
} Options;

static void print_usage(FILE * out, const char * program) {
    fprintf(out,
        "Usage: %s [options]\n"
        "\n"
        "A terminal chat client for an OpenAI-compatible server, such as llama.cpp.\n"
        "\n"
        "  --host <url>    Server to talk to (default: %s)\n"
        "  --model <name>  Model to chat with. Without it the models the server\n"
        "                  serves are listed and the program exits.\n"
        "  --token <tok>   API token, sent as a bearer token when the server\n"
        "                  requires one\n"
        "  -t              Let a reasoning model deliberate before answering\n"
        "  -h, --help      Show this help\n",
        program, DEFAULT_HOST);
}

/* Reads the value of a flag that requires one. Returns NULL and reports the
   problem when the flag is missing its argument or followed by another flag. */
static const char * option_value(int argc, char * argv[], int * i, const char * flag) {
    if(*i + 1 >= argc || strncmp(argv[*i + 1], "--", 2) == 0) {
        fprintf(stderr, "chatter: %s needs a value\n", flag);
        return NULL;
    }
    (*i)++;
    return argv[*i];
}

/* Returns false when parsing failed, so main can exit before touching the
   terminal. */
static bool parse_options(int argc, char * argv[], Options * opts) {
    opts->host = DEFAULT_HOST;
    opts->model = NULL;
    opts->token = NULL;
    opts->thinking = false;

    for(int i = 1; i < argc; ++i) {
        const char * arg = argv[i];
        if(strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            print_usage(stdout, argv[0]);
            exit(0);
        } else if(strcmp(arg, "-t") == 0) {
            opts->thinking = true;
        } else if(strcmp(arg, "--host") == 0 || strcmp(arg, "-H") == 0) {
            const char * value = option_value(argc, argv, &i, arg);
            if(value == NULL) return false;
            opts->host = value;
        } else if(strcmp(arg, "--model") == 0 || strcmp(arg, "-m") == 0) {
            const char * value = option_value(argc, argv, &i, arg);
            if(value == NULL) return false;
            opts->model = value;
        } else if(strcmp(arg, "--token") == 0) {
            const char * value = option_value(argc, argv, &i, arg);
            if(value == NULL) return false;
            opts->token = value;
        } else {
            fprintf(stderr, "chatter: unknown option '%s'\n", arg);
            fprintf(stderr, "Try '%s --help'.\n", argv[0]);
            return false;
        }
    }
    return true;
}

/* Asks the server what it serves and prints the answer. Used when no model was
   named: the model names are whatever the server was started with, so there is
   nothing sensible to assume here. */
static bool show_available_models(const Options * opts) {
    ChatClient * chat = new_chat_client(opts->host, "", opts->token, opts->thinking);

    String * err = new(String);
    List * models = chat_list_models(chat, err);

    if(models == NULL) {
        fprintf(stderr, "chatter: %s\n", call(err, to_cstring));
        REFCDEC(err);
        chat_client_delete(chat);
        return false;
    }

    if(call(models, size) == 0) {
        fprintf(stderr, "chatter: %s serves no models.\n", opts->host);
    } else {
        printf("Models served by %s:\n", opts->host);
        for(int i = 0; i < call(models, size); ++i) {
            String * model = call(models, get, i);
            printf("  %s\n", call(model, to_cstring));
            REFCDEC(model);
        }
        printf("\nStart again with --model <name> to chat with one of these.\n");
    }

    REFCDEC(models);
    REFCDEC(err);
    chat_client_delete(chat);
    return true;
}

int main(int argc, char * argv[]) {
    Options opts;
    if(!parse_options(argc, argv, &opts)) {
        return 2;
    }

    /* Without a model there is nothing to chat with, so this run exists only to
       report what the server offers. Doing it before the terminal is opened
       keeps the output ordinary text rather than something the user has to
       scroll back through a screen to find. */
    if(opts.model == NULL) {
        return show_available_models(&opts) ? 0 : 1;
    }

    const char * url = opts.host;
    const char * model = opts.model;

    Terminal * term = new(Terminal);
    if(!call(term, open)) {
        fprintf(stderr, "chatter: not a terminal, nothing to do\n");
        REFCDEC(term);
        return 1;
    }

    int w = 0, h = 0;
    call(term, get_size, &w, &h);
    if(w < 40 || h < 10) {
        w = 80;
        h = 24;
    }

    Renderer * rend = new(Renderer, call(term, out_fd), w, h);
    call(rend, set_title, new(String, "chatter"));
    call(rend, enter_alt_screen);
    call(rend, hide_cursor);

    App app = {0};
    app.screen = new(Buffer, w, h);
    app.chat = new_chat_client(url, model, opts.token, opts.thinking);
    app.url = strdup(url);
    app.model = strdup(model);
    app.thinking = opts.thinking;
    mtx_init(&app.lock, mtx_plain);

    lines_push_wrapped(&app, LINE_SYSTEM,
        "Ask something and press Enter. Ctrl-C quits.", w - MARGIN * 2 - 5);

    draw(&app);
    call(rend, present, app.screen);

    EventLoop * loop = new(EventLoop);

    bool running = true;
    while(running) {
        /* A resize is handled here rather than in the signal handler, so the
           buffer can be reallocated and repainted safely. */
        if(call(term, size_changed)) {
            call(term, clear_resize);
            on_resize(&app, term, app.screen, rend);
            call(rend, present, app.screen);
        }

        Event ev;
        memset(&ev, 0, sizeof ev);
        if(!call(loop, poll_once, term, &ev, POLL_TIMEOUT_MS)) {
            /* Nothing was typed. Pick up a reply that finished in the meantime
               and repaint only when something actually changed, so an idle
               screen does not flicker. */
            int before = app.line_count;
            collect_result(&app, body_width(&app));
            if(app.line_count != before) {
                draw(&app);
                call(rend, present, app.screen);
            }
            continue;
        }

        if(ev.type == EVENT_RESIZE) {
            on_resize(&app, term, app.screen, rend);
        } else if(!on_event(&ev, &app)) {
            running = false;
            continue;
        }
        call(rend, present, app.screen);
    }

    /* A request may still be running: wait for it before tearing down the
       objects it reads, or it would touch freed memory. */
    if(app.worker_running) {
        thrd_join(app.worker, NULL);
        app.worker_running = false;
    }

    call(rend, leave_alt_screen);
    call(term, close);

    REFCDEC(app.reply);
    REFCDEC(app.error);
    mtx_destroy(&app.lock);
    lines_free(&app);
    free(app.input);
    free(app.url);
    free(app.model);
    chat_client_delete(app.chat);
    REFCDEC(app.screen);
    REFCDEC(loop);
    REFCDEC(rend);
    REFCDEC(term);
    return 0;
}
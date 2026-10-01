#ifndef CDF_TUI_EVENTLOOP_H
#define CDF_TUI_EVENTLOOP_H

#include "../core/core.h"
#include "buffer.h"
#include "keyevent.h"
#include "renderer.h"
#include "terminal.h"

/* Return false from the handler to stop the loop. */
typedef bool (*EventHandler)(Event * ev, ObjectPtr user_data);

/* Undecoded bytes carried between reads. An escape sequence or a UTF-8 code
   point can arrive split across two reads, so whatever the decoder does not
   consume is kept until the rest shows up. */
#define TUI_INBUF_CAP 256

typedef struct {
    inherits(Object);

    /* Polls the terminal, dispatches to the handler, and repaints whenever the
       handler marks the buffer dirty. Returns the number of events handled. */
    int (*run)(ObjectPtr, Terminal *, Renderer *, Buffer *, EventHandler, ObjectPtr);

    /* Waits up to timeout_ms for a single event without running the loop.
       Returns true when an event was produced, false on timeout. */
    bool (*poll_once)(ObjectPtr, Terminal *, Event *, int timeout_ms);

    /* Stops the innermost run() at the next iteration. */
    void (*stop)(ObjectPtr);
    bool (*running)(ObjectPtr);

    //'private'
    bool _running;
    char _inbuf[TUI_INBUF_CAP];
    int _inlen;
} EventLoop;

EventLoop * EventLoop_new(EventLoop * this);
void EventLoop_delete(ObjectPtr);

#endif

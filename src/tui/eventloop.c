#include "eventloop.h"
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

static void _stop(ObjectPtr _this) {
    make_this(EventLoop, _this);
    this->_running = false;
}

static bool _running(ObjectPtr _this) {
    make_this(EventLoop, _this);
    return this->_running;
}

/* Reads whatever is available and decodes one event. Returns true when ev was
   filled, false on timeout. */
static bool _poll_once(ObjectPtr _this, Terminal * term, Event * ev, int timeout_ms) {
    make_this(EventLoop, _this);

    struct pollfd pfd;
    pfd.fd = call(term, in_fd);
    pfd.events = POLLIN;
    pfd.revents = 0;

    /* Drain everything already buffered before waiting on the fd, so a burst of
       input does not cost one poll per event. */
    if(this->_inlen > 0) {
        pfd.revents = POLLIN;
    } else {
        int r = poll(&pfd, 1, timeout_ms);
        if(r <= 0) return false;              /* timeout or interrupted */
        if(!(pfd.revents & (POLLIN | POLLHUP | POLLERR))) return false;

        ssize_t n = read(pfd.fd, this->_inbuf, sizeof this->_inbuf);
        if(n <= 0) {
            if(n < 0 && (errno == EINTR || errno == EAGAIN)) return false;
            /* End of input: report it once as a quit so the loop can exit. */
            ev->type = EVENT_QUIT;
            ev->key.key = KEY_NONE;
            ev->key.mods = MOD_NONE;
            ev->key.ch = 0;
            ev->width = 0;
            ev->height = 0;
            return true;
        }
        this->_inlen = (int)n;
    }

    int used = key_decode(this->_inbuf, this->_inlen, ev);
    if(used < 0) return false;
    if(used == 0) {
        /* Incomplete sequence and the buffer is full: the input is not a key
           we can interpret, so drop it rather than wedge the loop forever. */
        if(this->_inlen == (int)sizeof this->_inbuf) this->_inlen = 0;
        return false;
    }
    int rest = this->_inlen - used;
    if(rest > 0) memmove(this->_inbuf, this->_inbuf + used, (size_t)rest);
    this->_inlen = rest;
    return true;
}

static int _run(ObjectPtr _this, Terminal * term, Renderer * rend, Buffer * buf,
                EventHandler handler, ObjectPtr user_data) {
    make_this(EventLoop, _this);
    this->_running = true;
    int handled = 0;

    while(this->_running) {
        /* A resize is handled here rather than in the signal handler, so the
           buffer can be reallocated and repainted safely. */
        if(call(term, size_changed)) {
            call(term, clear_resize);
            int w = 0, h = 0;
            call(term, get_size, &w, &h);
            if(w > 0 && h > 0) {
                call(buf, resize, w, h);
                call(rend, invalidate);
            }
            Event ev;
            memset(&ev, 0, sizeof ev);
            ev.type = EVENT_RESIZE;
            ev.width = w;
            ev.height = h;
            if(handler && !handler(&ev, user_data)) break;
            handled++;
            /* Present here: invalidate() marked the whole screen dirty, and if
               no key follows the resize the repaint would otherwise not happen
               until one does. The handler has just rebuilt the buffer. */
            call(rend, present, buf);
        }

        Event ev;
        if(!_poll_once(_this, term, &ev, -1)) continue;

        if(ev.type == EVENT_QUIT) {
            handled++;
            break;
        }
        if(handler && !handler(&ev, user_data)) {
            handled++;
            break;
        }
        handled++;
        /* Repaint after the handler so a frame is emitted at most once per
           event, however many cells the handler changed. */
        call(rend, present, buf);
    }

    this->_running = false;
    return handled;
}

EventLoop * EventLoop_new(EventLoop * this) {
    super(Object, EventLoop);
    this->_running = false;
    this->_inlen = 0;
    this->_inbuf[0] = '\0';
    this->run = _run;
    this->poll_once = _poll_once;
    this->stop = _stop;
    this->running = _running;
    return this;
}

void EventLoop_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

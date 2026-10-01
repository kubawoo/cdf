#include "bufferdiff.h"
#include <string.h>

static void _reset(BufferDiff * this) {
    this->_count = 0;
}

static bool _ensure(BufferDiff * this, int need) {
    if(need <= this->_capacity) return true;
    int cap = this->_capacity ? this->_capacity : 256;
    while(cap < need) {
        if(cap > (1 << 28)) return false;   /* refuse rather than overflow */
        cap *= 2;
    }
    DiffPoint * p = pool_alloc((size_t)cap * sizeof(DiffPoint));
    if(p == NULL) return false;
    Cell * c = pool_alloc((size_t)cap * sizeof(Cell));
    if(c == NULL) {
        pool_free(p);
        return false;
    }
    if(this->_capacity) {
        memcpy(p, this->_points, (size_t)this->_count * sizeof(DiffPoint));
        memcpy(c, this->_cells, (size_t)this->_count * sizeof(Cell));
        pool_free(this->_points);
        pool_free(this->_cells);
    }
    this->_points = p;
    this->_cells = c;
    this->_capacity = cap;
    return true;
}

static void _compute(ObjectPtr _this, Buffer * front, Buffer * back, bool force) {
    make_this(BufferDiff, _this);
    _reset(this);
    if(back == NULL) return;

    int w = call(back, width);
    int h = call(back, height);
    if(w <= 0 || h <= 0) return;

    /* Only the overlapping region can be compared. When the grids differ in
       size the caller forces a full repaint, so the mismatch is handled there
       rather than here. */
    if(front != NULL) {
        int fw = call(front, width);
        int fh = call(front, height);
        if(fw != w || fh != h) force = true;
    }

    if(!_ensure(this, w * h)) return;

    Cell * back_cells = call(back, cells);
    Cell * front_cells = front ? call(front, cells) : NULL;

    for(int y = 0; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            Cell * b = &back_cells[y * w + x];
            bool changed = true;
            if(!force && front_cells != NULL) {
                changed = !cell_equal(&front_cells[y * w + x], b);
            }
            /* A wide tail carries no glyph of its own; it is emitted as part of
               the lead cell, so recording it separately would double-write. */
            if(!changed || b->wide_tail) continue;
            this->_points[this->_count].x = x;
            this->_points[this->_count].y = y;
            this->_cells[this->_count] = *b;
            this->_count++;
        }
    }
}

static int _count(ObjectPtr _this) {
    make_this(BufferDiff, _this);
    return this->_count;
}

static DiffPoint * _points(ObjectPtr _this) {
    make_this(BufferDiff, _this);
    return this->_points;
}

static Cell * _cells(ObjectPtr _this) {
    make_this(BufferDiff, _this);
    return this->_cells;
}

BufferDiff * BufferDiff_new(BufferDiff * this) {
    super(Object, BufferDiff);
    this->_points = NULL;
    this->_cells = NULL;
    this->_count = 0;
    this->_capacity = 0;

    this->compute = _compute;
    this->count = _count;
    this->points = _points;
    this->cells = _cells;
    return this;
}

void BufferDiff_delete(ObjectPtr _this) {
    make_this(BufferDiff, _this);
    if(this->_points) {
        pool_free(this->_points);
        this->_points = NULL;
    }
    if(this->_cells) {
        pool_free(this->_cells);
        this->_cells = NULL;
    }
    this->_capacity = 0;
    this->_count = 0;
    super_delete(Object, this);
}

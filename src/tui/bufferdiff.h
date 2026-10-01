#ifndef CDF_TUI_BUFFERDIFF_H
#define CDF_TUI_BUFFERDIFF_H

#include "../core/core.h"
#include "buffer.h"
#include "cell.h"

typedef struct {
    int x;
    int y;
} DiffPoint;

typedef struct {
    inherits(Object);

    /* Compares front against back and records every cell that differs. Passing
       force treats all cells as changed, which is what a resize needs since the
       terminal's own contents are unknown after it. */
    void (*compute)(ObjectPtr, Buffer * front, Buffer * back, bool force);
    int (*count)(ObjectPtr);
    DiffPoint * (*points)(ObjectPtr);
    Cell * (*cells)(ObjectPtr);

    //'private'
    DiffPoint * _points;
    Cell * _cells;
    int _count;
    int _capacity;
} BufferDiff;

BufferDiff * BufferDiff_new(BufferDiff * this);
void BufferDiff_delete(ObjectPtr);

#endif

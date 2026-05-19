#ifndef TODO_ITEM_H
#define TODO_ITEM_H

#include <cdf.h>
#include <json.h>

typedef struct {
    inherits(Object);
    long id;
    String * title;
    String * description;
    String * due_date;
    String * assignee;
    String * status;
} TodoItem;

TodoItem * TodoItem_new6(TodoItem * this, long id, String * title, String * description, String * due_date, String * assignee, String * status);
void TodoItem_delete(ObjectPtr this);
JsonObject * todo_item_to_json(TodoItem * this);
void todo_item_set_status(TodoItem * this, String * status);

#endif

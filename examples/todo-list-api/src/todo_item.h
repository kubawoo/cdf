#ifndef TODO_ITEM_H
#define TODO_ITEM_H

#include <cdf.h>
#include <json.h>
#include <entity.h>

typedef struct {
    inherits(Entity);
    List * (*_fields)(ObjectPtr);
    String * title;
    String * description;
    String * due_date;
    String * assignee;
    String * status;
} TodoItem;

TodoItem * TodoItem_new6(TodoItem * this, long id, String * title, String * description, String * due_date, String * assignee, String * status);
TodoItem * TodoItem_new(TodoItem * this);
void TodoItem_delete(ObjectPtr this);
JsonObject * todo_item_to_json(TodoItem * this);
void todo_item_set_status(TodoItem * this, String * status);
List * TodoItem_fields(ObjectPtr _this);

#endif

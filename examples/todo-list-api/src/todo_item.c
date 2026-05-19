#include "todo_item.h"

TodoItem * TodoItem_new6(TodoItem * this, long id, String * title, String * description, String * due_date, String * assignee, String * status) {
    super(Object, TodoItem);
    this->id = id;
    this->title = title;
    REFCINC(title);
    this->description = description;
    REFCINC(description);
    this->due_date = due_date;
    REFCINC(due_date);
    this->assignee = assignee;
    REFCINC(assignee);
    this->status = status;
    REFCINC(status);
    return this;
}

void TodoItem_delete(ObjectPtr _this) {
    make_this(TodoItem, _this);
    REFCDEC(this->title);
    REFCDEC(this->description);
    REFCDEC(this->due_date);
    REFCDEC(this->assignee);
    REFCDEC(this->status);
    super_delete(Object, _this);
}

JsonObject * todo_item_to_json(TodoItem * this) {
    JsonObject * obj = new(JsonObject);
    call(obj, put_value, REFCTMP(new(String, "id")), REFCTMP(new(Long, this->id)));
    call(obj, put_value, REFCTMP(new(String, "title")), this->title);
    call(obj, put_value, REFCTMP(new(String, "description")), this->description);
    call(obj, put_value, REFCTMP(new(String, "due_date")), this->due_date);
    call(obj, put_value, REFCTMP(new(String, "assignee")), this->assignee);
    call(obj, put_value, REFCTMP(new(String, "status")), this->status);
    return obj;
}

void todo_item_set_status(TodoItem * this, String * status) {
    REFCINC(status);
    REFCDEC(this->status);
    this->status = status;
}

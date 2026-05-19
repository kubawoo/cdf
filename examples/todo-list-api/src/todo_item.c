#include "todo_item.h"

List * TodoItem_fields(ObjectPtr _this) {
    make_this(TodoItem, _this);
    List * fields = new(List);

    String * name = new(String, "title");
    String * type = new(String, "String");
    FieldMetadata * fm = new(FieldMetadata, (Entity *)this, (Object**)&(this->title), name, type);
    call(fields, add, fm);
    REFCDEC(name);
    REFCDEC(type);
    REFCDEC(fm);

    name = new(String, "description");
    type = new(String, "String");
    fm = new(FieldMetadata, (Entity *)this, (Object**)&(this->description), name, type);
    call(fields, add, fm);
    REFCDEC(name);
    REFCDEC(type);
    REFCDEC(fm);

    name = new(String, "due_date");
    type = new(String, "String");
    fm = new(FieldMetadata, (Entity *)this, (Object**)&(this->due_date), name, type);
    call(fields, add, fm);
    REFCDEC(name);
    REFCDEC(type);
    REFCDEC(fm);

    name = new(String, "assignee");
    type = new(String, "String");
    fm = new(FieldMetadata, (Entity *)this, (Object**)&(this->assignee), name, type);
    call(fields, add, fm);
    REFCDEC(name);
    REFCDEC(type);
    REFCDEC(fm);

    name = new(String, "status");
    type = new(String, "String");
    fm = new(FieldMetadata, (Entity *)this, (Object**)&(this->status), name, type);
    call(fields, add, fm);
    REFCDEC(name);
    REFCDEC(type);
    REFCDEC(fm);

    return fields;
}

TodoItem * TodoItem_new6(TodoItem * this, long id, String * title, String * description, String * due_date, String * assignee, String * status) {
    super(Entity, TodoItem);
    override(Entity, _fields, TodoItem_fields);
    if (id > 0)
        ((Entity *)this)->id = new(Long, id);
    REFCINC(title);
    this->title = title;
    REFCINC(description);
    this->description = description;
    REFCINC(due_date);
    this->due_date = due_date;
    REFCINC(assignee);
    this->assignee = assignee;
    REFCINC(status);
    this->status = status;
    return this;
}

TodoItem * TodoItem_new(TodoItem * this) {
    this(TodoItem, 0L, NULL, NULL, NULL, NULL, NULL);
    return this;
}

void TodoItem_delete(ObjectPtr _this) {
    make_this(TodoItem, _this);
    REFCDEC(this->title);
    REFCDEC(this->description);
    REFCDEC(this->due_date);
    REFCDEC(this->assignee);
    REFCDEC(this->status);
    super_delete(Entity, _this);
}

JsonObject * todo_item_to_json(TodoItem * this) {
    JsonObject * obj = new(JsonObject);
    if (((Entity *)this)->id)
        call(obj, put_value, REFCTMP(new(String, "id")), ((Entity *)this)->id);
    if (this->title)
        call(obj, put_value, REFCTMP(new(String, "title")), this->title);
    if (this->description)
        call(obj, put_value, REFCTMP(new(String, "description")), this->description);
    if (this->due_date)
        call(obj, put_value, REFCTMP(new(String, "due_date")), this->due_date);
    if (this->assignee)
        call(obj, put_value, REFCTMP(new(String, "assignee")), this->assignee);
    if (this->status)
        call(obj, put_value, REFCTMP(new(String, "status")), this->status);
    return obj;
}

void todo_item_set_status(TodoItem * this, String * status) {
    REFCINC(status);
    REFCDEC(this->status);
    this->status = status;
}

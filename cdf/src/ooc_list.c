#include "ooc_list.h"

void _ListItem_delete(ObjectPtr _this) {
    make_this(_ListItem, _this);
    REFCDEC(this->item);
    super_delete(Object, _this);
}

_ListItem * _ListItem_new3(_ListItem * this, Object * item, _ListItem * prev, _ListItem * next) {
    super(Object, _ListItem);
    this->item = item;
    this->prev = prev;
    this->next = next;
    return this;
}

_ListItem * _ListItem_new2(_ListItem * this, Object * item, _ListItem * prev) {
    this(_ListItem, item, prev, NULL);
    return this;
}

_ListItem * _ListItem_new1(_ListItem * this, Object * item) {
    this(_ListItem, item, NULL, NULL);
    return this;
}


void List_delete(ObjectPtr _this) {
    make_this(List, _this);
    _ListItem * li = this->_first;
    for(int i = 0; i < this->length; ++i) {
         _ListItem * next = li->next;
         REFCDEC(li);
         li = next;
    }
    super_delete(Object, _this);
}

void List_add(ObjectPtr _this, ObjectPtr e) {
    REFCINC(e);
    make_this(List, _this);
    if(this->length == 0) {
        this->_first = new(_ListItem, e);
        this->_last = this->_first;
    } else {
        _ListItem * item = new(_ListItem, e, this->_last);
        this->_last->next = item;
        this->_last = item;
    }
    this->length++;
}

void List_remove(ObjectPtr _this, int i) {
    make_this(List, _this);
    if(i < 0 || i >= this->length) {
        return;
    }

    _ListItem * it = this->_first;

    for(int k = 0; k < i; ++k) {
        it = it->next;
    }
    _ListItem * prev = it->prev;
    _ListItem * next = it->next;

    REFCDEC(it);
    if(prev != NULL) {
        prev->next = next;
    } else {
        this->_first = next;
    }
    if(next != NULL) {
        next->prev = prev;
    } else {
        this->_last = prev;
    }
    this->length--;
}

ObjectPtr List_get(ObjectPtr _this, int i) {
    make_this(List, _this);
    if(i < 0 || i >= this->length) {
        return NULL;
    }

    _ListItem * it = this->_first;
    for(int k = 0; k < i; ++k) {
        it = it->next;
    }
    ObjectPtr o = it->item;
    REFCINC(o);
    return o;
}

void List_set(ObjectPtr _this, int i, ObjectPtr item) {
    make_this(List, _this);
    if(i < 0 || i >= this->length) {
        return;
    }
    REFCINC(item);
    _ListItem * it = this->_first;
    for(int k = 0; k < i; ++k) {
        it = it->next;
    }
    REFCDEC(it->item);
    it->item = item;
}

void List_insert(ObjectPtr _this, int i, ObjectPtr e) {
    make_this(List, _this);
    if(i < 0 || i > this->length) {
        return;
    }

    if(i == this->length) {
        List_add(_this, e);
        return;
    }

    _ListItem * it = this->_first;
    for(int k = 0; k < i; ++k) {
        it = it->next;
    }

    _ListItem * prev = it->prev;
    _ListItem * item = new(_ListItem, e, prev, it);
    if(prev != NULL) {
        prev->next = item;
    } else {
        this->_first = item;
    }
    REFCINC(e);

    it->prev = item;
    this->length++;
}

static bool List_contains(ObjectPtr _this, ObjectPtr element) {
	make_this(List, _this);
	for(int i = 0; i < this->length; ++i) {
	        ObjectPtr e = call(this, get, i);
	        REFCDEC(e);
	        if(call((Object*)element, equals, e)) {
	            return true;
	        }
	    }
	    return false;
}

static String * List_to_string(ObjectPtr _this) {
	make_this(List, _this);
	String * s = new(String, "[");

	for(int i = 0; i < this->length; ++i) {
		Object * o = call(this, get, i);
		String * os;
		if(o != NULL) {
			os = call(o, to_string);
		} else {
			os = new(String, "NULL");
		}
		call(s, append, os);
		REFCDEC(o);
		REFCDEC(os);
		if(i < this->length -1) {
			call(s, append_cstring, "; ");
		}
	}

	call(s, append_char, ']');
	return s;
}

static Iterator * _iterator(ObjectPtr _this) {
    make_this(List, _this);
    return new(ListIterator, this);
}

List * List_new(List * this) {
    super(Collection, List);
    override(Object, to_string, List_to_string);
    override(Collection, iterator, _iterator);
    this->length = 0;
    this->_first = NULL;
    this->_last = NULL;
    this->add = List_add;
    this->remove = List_remove;
    this->get = List_get;
    this->set = List_set;
    this->insert = List_insert;
    this->contains = List_contains;
    // Implement the iterator method from Collection
    return this;
}


static ObjectPtr _next(ObjectPtr _this) {
    make_this(ListIterator, _this);
    if(!call(this, hasNext)) {
        return NULL;
    }
    return call(this->list, get, this->index++);
}

static bool _hasNext(ObjectPtr _this) {
    make_this(ListIterator, _this);
    return this->index < this->list->length;
}


// ListIterator implementation
ListIterator * ListIterator_new1(ListIterator * this, List * list) {
    super(Iterator, ListIterator);
    override(Iterator, hasNext, _hasNext);
    override(Iterator, next, _next);
    REFCINC(list);

    this->list = list;
    this->index = 0;
    return this;
}


void ListIterator_delete(ObjectPtr _this) {
    make_this(ListIterator, _this);
    REFCDEC(this->list);
    super_delete(Iterator, _this);
}
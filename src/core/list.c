#include "list.h"

void _ListItem_delete(ObjectPtr _this) {
    make_this(_ListItem, _this);
    REFCDEC(this->_item);
    super_delete(Object, _this);
}

_ListItem * _ListItem_new3(_ListItem * this, Object * item, _ListItem * prev, _ListItem * next) {
    super(Object, _ListItem);
    this->_item = item;
    this->_prev = prev;
    this->_next = next;
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
    for(int i = 0; i < this->_length; ++i) {
         _ListItem * next = li->_next;
         REFCDEC(li);
         li = next;
    }
    super_delete(Object, _this);
}

static void List_add(ObjectPtr _this, ObjectPtr e) {
    REFCINC(e);
    make_this(List, _this);
    if(this->_length == 0) {
        this->_first = new(_ListItem, e);
        this->_last = this->_first;
    } else {
        _ListItem * item = new(_ListItem, e, this->_last);
        this->_last->_next = item;
        this->_last = item;
    }
    this->_length++;
}

static void List_remove(ObjectPtr _this, int i) {
    make_this(List, _this);
    if(i < 0 || i >= this->_length) {
        return;
    }

    _ListItem * it = this->_first;

    for(int k = 0; k < i; ++k) {
        it = it->_next;
    }
    _ListItem * prev = it->_prev;
    _ListItem * next = it->_next;

    REFCDEC(it);
    if(prev != NULL) {
        prev->_next = next;
    } else {
        this->_first = next;
    }
    if(next != NULL) {
        next->_prev = prev;
    } else {
        this->_last = prev;
    }
    this->_length--;
}

static ObjectPtr List_get(ObjectPtr _this, int i) {
    make_this(List, _this);
    if(i < 0 || i >= this->_length) {
        return NULL;
    }

    _ListItem * it = this->_first;
    for(int k = 0; k < i; ++k) {
        it = it->_next;
    }
    ObjectPtr o = it->_item;
    REFCINC(o);
    return o;
}

static void List_set(ObjectPtr _this, int i, ObjectPtr item) {
    make_this(List, _this);
    if(i < 0 || i >= this->_length) {
        return;
    }
    REFCINC(item);
    _ListItem * it = this->_first;
    for(int k = 0; k < i; ++k) {
        it = it->_next;
    }
    REFCDEC(it->_item);
    it->_item = item;
}

static void List_insert(ObjectPtr _this, int i, ObjectPtr e) {
    make_this(List, _this);
    if(i < 0 || i > this->_length) {
        return;
    }

    if(i == this->_length) {
        List_add(_this, e);
        return;
    }

    _ListItem * it = this->_first;
    for(int k = 0; k < i; ++k) {
        it = it->_next;
    }

    _ListItem * prev = it->_prev;
    _ListItem * item = new(_ListItem, e, prev, it);
    if(prev != NULL) {
        prev->_next = item;
    } else {
        this->_first = item;
    }
    REFCINC(e);

    it->_prev = item;
    this->_length++;
}

static bool List_contains(ObjectPtr _this, ObjectPtr element) {
	make_this(List, _this);
    for(int i = 0; i < this->_length; ++i) {
	        ObjectPtr e = call(this, get, i);
	        REFCDEC(e);
	        if(call((Object*)element, equals, e)) {
	            return true;
	        }
	    }
	    return false;
}

static String * _to_string(ObjectPtr _this) {
	make_this(List, _this);
	String * s = new(String, "[");

    for(int i = 0; i < this->_length; ++i) {
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
        if(i < this->_length - 1) {
			call(s, append_cstring, "; ");
		}
	}

	call(s, append_char, ']');
	return s;
}

static unsigned int _size(ObjectPtr _this) {
    make_this(List, _this);
    return this->_length;
}

static Iterator * _iterator(ObjectPtr _this) {
    make_this(List, _this);
    return new(ListIterator, this);
}

List * List_new(List * this) {
    super(Collection, List);
    override(Object, to_string, _to_string);
    override(Collection, iterator, _iterator);
    override(Collection, size, _size);

    this->_length = 0;
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
    if(!this->_curr) {
        return NULL;
    }
    ObjectPtr o = this->_curr->_item;
    REFCINC(o);
    this->_curr = this->_curr->_next;
    return o;
}

static bool _hasNext(ObjectPtr _this) {
    make_this(ListIterator, _this);
    return this->_curr != NULL;
}


// ListIterator implementation
ListIterator * ListIterator_new1(ListIterator * this, List * list) {
    super(Iterator, ListIterator);
    override(Iterator, hasNext, _hasNext);
    override(Iterator, next, _next);
    REFCINC(list);

    this->_list = list;
    this->_curr = list->_first;
    return this;
}


void ListIterator_delete(ObjectPtr _this) {
    make_this(ListIterator, _this);
    REFCDEC(this->_list);
    super_delete(Iterator, _this);
}
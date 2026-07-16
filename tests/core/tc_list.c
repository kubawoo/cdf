#include "list.h"
#include <assert.h>


static void list_test(void)
{
    List * list = new(List);
    assert(type_equal(list, "List"));
    assert((list->length) == (0));
    assert(list->_first == NULL);
    assert(list->_last == NULL);

    String * s = new(String, "hello");
    call(list, add, s);
    REFCDEC(s);

    assert((list->length) == (1));
    assert(list->_first != NULL);
    assert((void*)(list->_first) == (void*)(list->_last));
    assert((void*)(list->_first->item) == (void*)(s));

    String * s2 = new(String, "world");
    call(list, add, s2);
    REFCDEC(s2);
    assert((list->length) == (2));
    assert(list->_first != NULL);
    assert(list->_last != NULL);
    assert((void*)(list->_first) != (void*)(list->_last));
    assert((void*)(list->_first->item) == (void*)(s));
    assert((void*)(list->_last->item) == (void*)(s2));
    REFCDEC(list);
}


static void list_get(void)
{
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "0")));
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "2")));
    call(list, add, REFCTMP(new(String, "3")));
    call(list, add, REFCTMP(new(String, "4")));

    assert((list->length) == (5));
    for(int i = 0; i < 5; ++i) {
        char buffer[2] = "x";
        buffer[0] = '0' + i;
        String * s = call(list, get, i);
        assert(strcmp(call(s, to_cstring), buffer) == 0);
        REFCDEC(s);
    }

    assert(call(list, get, -1) == NULL);
    assert(call(list, get, 5) == NULL);
    assert(call(list, get, 1234) == NULL);

    assert((list->length) == (5));
    REFCDEC(list);
}

static void list_set(void)
{
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "0")));
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "2")));
    call(list, add, REFCTMP(new(String, "3")));
    call(list, add, REFCTMP(new(String, "4")));

    assert((list->length) == (5));

    call(list, set, 0, REFCTMP(new(String, "a")));
    call(list, set, 2, REFCTMP(new(String, "c")));
    call(list, set, 4, REFCTMP(new(String, "e")));
    for(int i = 0; i < 5; ++i) {
        char buffer[2] = "x";
        if(i % 2) {
            buffer[0] = '0' + i;
        } else {
            buffer[0] = 'a' + i;
        }
        String * s = call(list, get, i);
        assert(strcmp(call(s, to_cstring), buffer) == 0);
        REFCDEC(s);
    }

    assert((list->length) == (5));
    REFCDEC(list);
}


static void list_remove(void)
{
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "0")));
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "2")));
    call(list, add, REFCTMP(new(String, "3")));
    call(list, add, REFCTMP(new(String, "4")));

    assert((list->length) == (5));

    call(list, remove, 0);

    String * s = call(list, get, 0);
    assert(strcmp(call(s, to_cstring), "1") == 0);
    REFCDEC(s);
    s = call(list, get, 1);
    assert(strcmp(call(s, to_cstring), "2") == 0);
    REFCDEC(s);
    s = call(list, get, 2);
    assert(strcmp(call(s, to_cstring), "3") == 0);
    REFCDEC(s);
    s = call(list, get, 3);
    assert(strcmp(call(s, to_cstring), "4") == 0);
    REFCDEC(s);
    assert(call(list, get, 4) == NULL);

    call(list, remove, 1);

    s = call(list, get, 0);
    assert(strcmp(call(s, to_cstring), "1") == 0);
    REFCDEC(s);
    s = call(list, get, 1);
    assert(strcmp(call(s, to_cstring), "3") == 0);
    REFCDEC(s);
    s = call(list, get, 2);
    assert(strcmp(call(s, to_cstring), "4") == 0);
    REFCDEC(s);
    assert(call(list, get, 3) == NULL);


    call(list, remove, 2);

    s = call(list, get, 0);
    assert(strcmp(call(s, to_cstring), "1") == 0);
    REFCDEC(s);
    s = call(list, get, 1);
    assert(strcmp(call(s, to_cstring), "3") == 0);
    REFCDEC(s);
    assert(call(list, get, 2) == NULL);

    REFCDEC(list);
}

static void list_insert(void)
{
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "3")));

    assert((list->length) == (2));
    String * s = call(list, get, 0);
    assert(strcmp(call(s, to_cstring), "1") == 0);
    REFCDEC(s);
    s = call(list, get, 1);
    assert(strcmp(call(s, to_cstring), "3") == 0);
    REFCDEC(s);
    assert(call(list, get, 3) == NULL);

    call(list, insert, 2, (Object*) REFCTMP(new(String, "4")));

    assert((list->length) == (3));
    s = call(list, get, 0);
    assert(strcmp(call(s, to_cstring), "1") == 0);
    REFCDEC(s);
    s = call(list, get, 1);
    assert(strcmp(call(s, to_cstring), "3") == 0);
    REFCDEC(s);
    s = call(list, get, 2);
    assert(strcmp(call(s, to_cstring), "4") == 0);
    REFCDEC(s);
    assert(call(list, get, 3) == NULL);

    call(list, insert, 0, (Object*) REFCTMP(new(String, "0")));

    assert((list->length) == (4));
    s = call(list, get, 0);
    assert(strcmp(call(s, to_cstring), "0") == 0);
    REFCDEC(s);
    s = call(list, get, 1);
    assert(strcmp(call(s, to_cstring), "1") == 0);
    REFCDEC(s);
    s = call(list, get, 2);
    assert(strcmp(call(s, to_cstring), "3") == 0);
    REFCDEC(s);
    s = call(list, get, 3);
    assert(strcmp(call(s, to_cstring), "4") == 0);
    REFCDEC(s);
    assert(call(list, get, 4) == NULL);

    call(list, insert, 2, (Object*) REFCTMP(new(String, "2")));

    assert((list->length) == (5));
    s = call(list, get, 0);
    assert(strcmp(call(s, to_cstring), "0") == 0);
    REFCDEC(s);
    s = call(list, get, 1);
    assert(strcmp(call(s, to_cstring), "1") == 0);
    REFCDEC(s);
    s = call(list, get, 2);
    assert(strcmp(call(s, to_cstring), "2") == 0);
    REFCDEC(s);
    s = call(list, get, 3);
    assert(strcmp(call(s, to_cstring), "3") == 0);
    REFCDEC(s);
    s = call(list, get, 4);
    assert(strcmp(call(s, to_cstring), "4") == 0);
    REFCDEC(s);
    assert(call(list, get, 5) == NULL);
    REFCDEC(list);
}

static void List_contains(void)
{
	List * list = new(List);
	String * a = new(String, "a");
	String * a2 = new(String, "a");
	String * b = new(String, "b");


	assert(!(call(list, contains, a)));
	assert(!(call(list, contains, a2)));
	assert(!(call(list, contains, b)));

	call(list, add, a);
	assert(call(list, contains, a));
	assert(call(list, contains, a2));
	assert(!(call(list, contains, b)));

	call(list, add, b);
	assert(call(list, contains, a));
	assert(call(list, contains, a2));
	assert(call(list, contains, b));

	call(list, remove, 0);
	assert(!(call(list, contains, a)));
	assert(!(call(list, contains, a2)));
	assert(call(list, contains, b));

	call(list, remove, 0);
	assert(!(call(list, contains, a)));
	assert(!(call(list, contains, a2)));
	assert(!(call(list, contains, b)));

	REFCDEC(list);
	REFCDEC(a);
	REFCDEC(a2);
	REFCDEC(b);
}

static void list_to_string(void)
{
	List * l = new(List);
	String * s = call(l, to_string);
	assert(strcmp(call(s, to_cstring), "[]") == 0);
	REFCDEC(s);

	call(l, add, REFCTMP(new(String, "hi")));
	s = call(l, to_string);
	assert(strcmp(call(s, to_cstring), "[hi]") == 0);
	REFCDEC(s);

	call(l, add, REFCTMP(new(String, "hello")));
	s = call(l, to_string);
	assert(strcmp(call(s, to_cstring), "[hi; hello]") == 0);
	REFCDEC(s);

	call(l, add, NULL);
	s = call(l, to_string);
	assert(strcmp(call(s, to_cstring), "[hi; hello; NULL]") == 0);
	REFCDEC(s);

	REFCDEC(l);
}

static void list_iterator_test(void)
{
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "a")));
    call(list, add, REFCTMP(new(String, "b")));
    call(list, add, REFCTMP(new(String, "c")));

    Iterator * it = call(list, iterator);
    assert(type_equal(it, "ListIterator"));

    assert(call(it, hasNext));
    String * s = call(it, next);
    assert(strcmp(call(s, to_cstring), "a") == 0);
    REFCDEC(s);

    assert(call(it, hasNext));
    s = call(it, next);
    assert(strcmp(call(s, to_cstring), "b") == 0);
    REFCDEC(s);

    assert(call(it, hasNext));
    s = call(it, next);
    assert(strcmp(call(s, to_cstring), "c") == 0);
    REFCDEC(s);

    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);

    REFCDEC(it);
    REFCDEC(list);
}

static void list_iterator_empty(void)
{
    List * list = new(List);
    Iterator * it = call(list, iterator);
    assert(type_equal(it, "ListIterator"));
    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);
    REFCDEC(it);
    REFCDEC(list);
}
int main(void)
{
    list_test();
    list_get();
    list_set();
    list_remove();
    list_insert();
    List_contains();
    list_to_string();
    list_iterator_test();
    list_iterator_empty();
    return 0;
}


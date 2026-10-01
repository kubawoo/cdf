#include "map.h"
#include "primitives.h"
#include <assert.h>

static void map_basic(void)
{
    Map * map = new(Map);
    assert(type_equal(map, "Map"));
    assert((call(map, size)) == (0));

    String * key = new(String, "color");
    String * val = new(String, "blue");
    call(map, put, REFCTMP(key), REFCTMP(val));

    assert((call(map, size)) == (1));
    assert(call(map, contains_key, key));
    assert(call(map, contains_value, val));

    String * got = call(map, get, key);
    assert(strcmp(call(got, to_cstring), "blue") == 0);
    REFCDEC(got);

    call(map, remove, key);
    assert((call(map, size)) == (0));
    assert(!(call(map, contains_key, key)));

    REFCDEC(map);
}

static void map_multiple_entries(void)
{
    Map * map = new(Map);
    String * keys[] = {
        new(String, "a"), new(String, "b"), new(String, "c"),
        new(String, "d"), new(String, "e")
    };
    String * vals[] = {
        new(String, "1"), new(String, "2"), new(String, "3"),
        new(String, "4"), new(String, "5")
    };
    int n = 5;

    for (int i = 0; i < n; i++) {
        call(map, put, REFCTMP(keys[i]), REFCTMP(vals[i]));
    }
    assert((call(map, size)) == (5));

    for (int i = 0; i < n; i++) {
        assert(call(map, contains_key, keys[i]));
        String * v = call(map, get, keys[i]);
        char expected[2] = {0};
        expected[0] = '1' + i;
        assert(strcmp(call(v, to_cstring), expected) == 0);
        REFCDEC(v);
    }

    REFCDEC(map);
}

static void map_update(void)
{
    Map * map = new(Map);
    String * key1 = new(String, "key1");
    call(map, put, key1, NULL);
    assert(call(map, contains_key, key1));
    assert(call(map, get, key1) == NULL);
    assert((call(map, size)) == (1));

    call(map, put, key1, REFCTMP(new(String, "abc")));
    assert((call(map, size)) == (1));

    String * abc = new(String, "abc");
    String * map_abc = call(map, get, key1);
    assert(strcmp(call(map_abc, to_cstring), call(abc, to_cstring)) == 0);

    REFCDEC(map_abc);
    REFCDEC(abc);
    REFCDEC(key1);
    REFCDEC(map);
}

static void map_remove_first(void)
{
    Map * map = new(Map);
    String * k1 = new(String, "x");
    String * k2 = new(String, "y");
    String * k3 = new(String, "z");
    call(map, put, k1, REFCTMP(new(String, "10")));
    call(map, put, k2, REFCTMP(new(String, "20")));
    call(map, put, k3, REFCTMP(new(String, "30")));
    assert((call(map, size)) == (3));

    call(map, remove, k1);
    assert((call(map, size)) == (2));
    assert(!(call(map, contains_key, k1)));
    assert(call(map, contains_key, k2));
    assert(call(map, contains_key, k3));

    String * v2 = call(map, get, k2);
    assert(strcmp(call(v2, to_cstring), "20") == 0);
    REFCDEC(v2);
    String * v3 = call(map, get, k3);
    assert(strcmp(call(v3, to_cstring), "30") == 0);
    REFCDEC(v3);

    REFCDEC(map);
    REFCDEC(k1);
    REFCDEC(k2);
    REFCDEC(k3);
}

static void map_remove_middle(void)
{
    Map * map = new(Map);
    String * k1 = new(String, "x");
    String * k2 = new(String, "y");
    String * k3 = new(String, "z");
    call(map, put, k1, REFCTMP(new(String, "10")));
    call(map, put, k2, REFCTMP(new(String, "20")));
    call(map, put, k3, REFCTMP(new(String, "30")));

    call(map, remove, k2);
    assert((call(map, size)) == (2));
    assert(call(map, contains_key, k1));
    assert(!(call(map, contains_key, k2)));
    assert(call(map, contains_key, k3));

    String * v1 = call(map, get, k1);
    assert(strcmp(call(v1, to_cstring), "10") == 0);
    REFCDEC(v1);
    String * v3 = call(map, get, k3);
    assert(strcmp(call(v3, to_cstring), "30") == 0);
    REFCDEC(v3);

    REFCDEC(map);
    REFCDEC(k1);
    REFCDEC(k2);
    REFCDEC(k3);
}

static void map_remove_last(void)
{
    Map * map = new(Map);
    String * k1 = new(String, "x");
    String * k2 = new(String, "y");
    String * k3 = new(String, "z");
    call(map, put, k1, REFCTMP(new(String, "10")));
    call(map, put, k2, REFCTMP(new(String, "20")));
    call(map, put, k3, REFCTMP(new(String, "30")));

    call(map, remove, k3);
    assert((call(map, size)) == (2));
    assert(call(map, contains_key, k1));
    assert(call(map, contains_key, k2));
    assert(!(call(map, contains_key, k3)));

    REFCDEC(map);
    REFCDEC(k1);
    REFCDEC(k2);
    REFCDEC(k3);
}

static void map_remove_all(void)
{
    Map * map = new(Map);
    String * keys[] = {
        new(String, "a"), new(String, "b"),
        new(String, "c"), new(String, "d")
    };
    int n = 4;

    for (int i = 0; i < n; i++) {
        call(map, put, REFCTMP(keys[i]), REFCTMP(new(Integer, i)));
    }
    assert((call(map, size)) == (4));

    for (int i = 0; i < n; i++) {
        call(map, remove, keys[i]);
    }
    assert((call(map, size)) == (0));

    for (int i = 0; i < n; i++) {
        assert(!(call(map, contains_key, keys[i])));
    }
    REFCDEC(map);
}

static void map_remove_nonexistent(void)
{
    Map * map = new(Map);
    String * k = new(String, "k");
    call(map, put, REFCTMP(k), REFCTMP(new(String, "v")));
    assert((call(map, size)) == (1));

    String * absent = new(String, "absent");
    call(map, remove, absent);
    assert((call(map, size)) == (1));
    assert(call(map, contains_key, k));

    REFCDEC(absent);
    REFCDEC(map);
}

static void map_get_keys(void)
{
    Map * map = new(Map);
    String * k1 = new(String, "first");
    String * k2 = new(String, "second");
    call(map, put, REFCTMP(k1), REFCTMP(new(String, "v1")));
    call(map, put, REFCTMP(k2), REFCTMP(new(String, "v2")));

    List * keys = call(map, get_keys);
    assert(call(keys, size) == (2));

    String * first = call(keys, get, 0);
    String * second = call(keys, get, 1);
    bool first_ok = call(first, equals_cstring, "first") || call(first, equals_cstring, "second");
    bool second_ok = call(second, equals_cstring, "first") || call(second, equals_cstring, "second");
    bool distinct = call(first, equals_cstring, "first") ? call(second, equals_cstring, "second") : call(second, equals_cstring, "first");
    assert(first_ok);
    assert(second_ok);
    assert(distinct);
    REFCDEC(first);
    REFCDEC(second);

    REFCDEC(keys);
    REFCDEC(map);
}

static void map_get_nonexistent(void)
{
    Map * map = new(Map);
    String * k = new(String, "missing");
    assert(call(map, get, k) == NULL);
    assert((call(map, size)) == (0));
    REFCDEC(k);
    REFCDEC(map);
}

static void map_contains_key_value(void)
{
    Map * map = new(Map);
    String * k = new(String, "k");
    String * v = new(String, "v");
    call(map, put, REFCTMP(k), REFCTMP(v));

    assert(call(map, contains_key, k));
    assert(call(map, contains_value, v));

    String * other_key = new(String, "other");
    assert(!(call(map, contains_key, other_key)));

    String * other_val = new(String, "other");
    assert(!(call(map, contains_value, other_val)));

    call(map, remove, k);
    assert(!(call(map, contains_key, k)));
    assert(!(call(map, contains_value, v)));

    REFCDEC(other_key);
    REFCDEC(other_val);
    REFCDEC(map);
}

static void map_empty_ops(void)
{
    Map * map = new(Map);
    assert((call(map, size)) == (0));

    String * any = new(String, "any");
    assert(call(map, get, any) == NULL);
    assert(!(call(map, contains_key, any)));
    assert(!(call(map, contains_value, any)));
    REFCDEC(any);

    String * s = call(map, to_string);
    assert(strcmp(call(s, to_cstring), "{}") == 0);
    REFCDEC(s);
    REFCDEC(map);
}

static void map_equals_key(void)
{
    Map * map = new(Map);
    String * k1 = new(String, "same");
    String * k2 = new(String, "same");
    assert((void*)(k1) != (void*)(k2));

    call(map, put, REFCTMP(k1), REFCTMP(new(String, "value")));
    assert(call(map, contains_key, k2));

    String * v = call(map, get, k2);
    assert(strcmp(call(v, to_cstring), "value") == 0);
    REFCDEC(v);

    REFCDEC(k2);
    REFCDEC(map);
}

static void map_remove_then_put(void)
{
    Map * map = new(Map);
    String * k = new(String, "key");

    call(map, put, k, REFCTMP(new(String, "first")));
    assert((call(map, size)) == (1));

    call(map, remove, k);
    assert((call(map, size)) == (0));

    call(map, put, k, REFCTMP(new(String, "second")));
    assert((call(map, size)) == (1));

    String * v = call(map, get, k);
    assert(strcmp(call(v, to_cstring), "second") == 0);
    REFCDEC(v);

    REFCDEC(k);
    REFCDEC(map);
}

static void map_to_string(void)
{
    Map * map = new(Map);

    String * key1 = new(String, "key1");
    String * key2 = new(String, "key2");
    String * key3 = new(String, "key3");

    call(map, put, REFCTMP(key1), REFCTMP(new(String, "abc")));
    call(map, put, REFCTMP(key2), REFCTMP(new(Integer, 5)));
    call(map, put, REFCTMP(key3), NULL);

    String * s = call(map, to_string);
    assert(strcmp(call(s, to_cstring), "{key1=abc; key2=5; key3=NULL}") == 0);

    REFCDEC(s);
    REFCDEC(map);
}

static void map_iterator_test(void)
{
    Map * map = new(Map);
    String * k1 = new(String, "x");
    String * k2 = new(String, "y");
    call(map, put, REFCTMP(k1), REFCTMP(new(String, "10")));
    call(map, put, REFCTMP(k2), REFCTMP(new(String, "20")));

    Iterator * it = call(map, iterator);
    assert(type_equal(it, "MapIterator"));

    assert(call(it, hasNext));
    String * k = call(it, next);
    assert(k != NULL);
    String * v = call(map, get, k);
    assert(v != NULL);
    REFCDEC(v);
    REFCDEC(k);

    assert(call(it, hasNext));
    k = call(it, next);
    assert(k != NULL);
    v = call(map, get, k);
    assert(v != NULL);
    REFCDEC(v);
    REFCDEC(k);

    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);

    REFCDEC(it);
    REFCDEC(map);
}

static void map_iterator_empty(void)
{
    Map * map = new(Map);
    Iterator * it = call(map, iterator);
    assert(type_equal(it, "MapIterator"));
    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);
    REFCDEC(it);
    REFCDEC(map);
}
int main(void)
{
    map_basic();
    map_multiple_entries();
    map_update();
    map_remove_first();
    map_remove_middle();
    map_remove_last();
    map_remove_all();
    map_remove_nonexistent();
    map_get_keys();
    map_get_nonexistent();
    map_contains_key_value();
    map_empty_ops();
    map_equals_key();
    map_remove_then_put();
    map_to_string();
    map_iterator_test();
    map_iterator_empty();
    return 0;
}


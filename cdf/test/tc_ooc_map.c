#include "test_framework.h"
#include "ooc.h"

void map_basic(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    ASSERT_TRUE(type_equal(map, "Map"));
    ASSERT_EQUAL(call(map, get_length), 0);

    String * key = new(String, "color");
    String * val = new(String, "blue");
    call(map, put, REFCTMP(key), REFCTMP(val));

    ASSERT_EQUAL(call(map, get_length), 1);
    ASSERT_TRUE(call(map, contains_key, key));
    ASSERT_TRUE(call(map, contains_value, val));

    String * got = call(map, get, key);
    ASSERT_STRINGS_EQUAL(call(got, to_cstring), "blue");
    REFCDEC(got);

    call(map, remove, key);
    ASSERT_EQUAL(call(map, get_length), 0);
    ASSERT_FALSE(call(map, contains_key, key));

    REFCDEC(map);
}

void map_multiple_entries(TEST_CASE_ARGUMENTS) {
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
    ASSERT_EQUAL(call(map, get_length), 5);

    for (int i = 0; i < n; i++) {
        ASSERT_TRUE(call(map, contains_key, keys[i]));
        String * v = call(map, get, keys[i]);
        char expected[2] = {0};
        expected[0] = '1' + i;
        ASSERT_STRINGS_EQUAL(call(v, to_cstring), expected);
        REFCDEC(v);
    }

    REFCDEC(map);
}

void map_update(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * key1 = new(String, "key1");
    call(map, put, key1, NULL);
    ASSERT_TRUE(call(map, contains_key, key1));
    ASSERT_TRUE(call(map, get, key1) == NULL);
    ASSERT_EQUAL(call(map, get_length), 1);

    call(map, put, key1, REFCTMP(new(String, "abc")));
    ASSERT_EQUAL(call(map, get_length), 1);

    String * abc = new(String, "abc");
    String * map_abc = call(map, get, key1);
    ASSERT_STRINGS_EQUAL(call(map_abc, to_cstring), call(abc, to_cstring));

    REFCDEC(map_abc);
    REFCDEC(abc);
    REFCDEC(key1);
    REFCDEC(map);
}

void map_remove_first(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k1 = new(String, "x");
    String * k2 = new(String, "y");
    String * k3 = new(String, "z");
    call(map, put, k1, REFCTMP(new(String, "10")));
    call(map, put, k2, REFCTMP(new(String, "20")));
    call(map, put, k3, REFCTMP(new(String, "30")));
    ASSERT_EQUAL(call(map, get_length), 3);

    call(map, remove, k1);
    ASSERT_EQUAL(call(map, get_length), 2);
    ASSERT_FALSE(call(map, contains_key, k1));
    ASSERT_TRUE(call(map, contains_key, k2));
    ASSERT_TRUE(call(map, contains_key, k3));

    String * v2 = call(map, get, k2);
    ASSERT_STRINGS_EQUAL(call(v2, to_cstring), "20");
    REFCDEC(v2);
    String * v3 = call(map, get, k3);
    ASSERT_STRINGS_EQUAL(call(v3, to_cstring), "30");
    REFCDEC(v3);

    REFCDEC(map);
    REFCDEC(k1);
    REFCDEC(k2);
    REFCDEC(k3);
}

void map_remove_middle(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k1 = new(String, "x");
    String * k2 = new(String, "y");
    String * k3 = new(String, "z");
    call(map, put, k1, REFCTMP(new(String, "10")));
    call(map, put, k2, REFCTMP(new(String, "20")));
    call(map, put, k3, REFCTMP(new(String, "30")));

    call(map, remove, k2);
    ASSERT_EQUAL(call(map, get_length), 2);
    ASSERT_TRUE(call(map, contains_key, k1));
    ASSERT_FALSE(call(map, contains_key, k2));
    ASSERT_TRUE(call(map, contains_key, k3));

    String * v1 = call(map, get, k1);
    ASSERT_STRINGS_EQUAL(call(v1, to_cstring), "10");
    REFCDEC(v1);
    String * v3 = call(map, get, k3);
    ASSERT_STRINGS_EQUAL(call(v3, to_cstring), "30");
    REFCDEC(v3);

    REFCDEC(map);
    REFCDEC(k1);
    REFCDEC(k2);
    REFCDEC(k3);
}

void map_remove_last(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k1 = new(String, "x");
    String * k2 = new(String, "y");
    String * k3 = new(String, "z");
    call(map, put, k1, REFCTMP(new(String, "10")));
    call(map, put, k2, REFCTMP(new(String, "20")));
    call(map, put, k3, REFCTMP(new(String, "30")));

    call(map, remove, k3);
    ASSERT_EQUAL(call(map, get_length), 2);
    ASSERT_TRUE(call(map, contains_key, k1));
    ASSERT_TRUE(call(map, contains_key, k2));
    ASSERT_FALSE(call(map, contains_key, k3));

    REFCDEC(map);
    REFCDEC(k1);
    REFCDEC(k2);
    REFCDEC(k3);
}

void map_remove_all(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * keys[] = {
        new(String, "a"), new(String, "b"),
        new(String, "c"), new(String, "d")
    };
    int n = 4;

    for (int i = 0; i < n; i++) {
        call(map, put, REFCTMP(keys[i]), REFCTMP(new(Integer, i)));
    }
    ASSERT_EQUAL(call(map, get_length), 4);

    for (int i = 0; i < n; i++) {
        call(map, remove, keys[i]);
    }
    ASSERT_EQUAL(call(map, get_length), 0);

    for (int i = 0; i < n; i++) {
        ASSERT_FALSE(call(map, contains_key, keys[i]));
    }
    REFCDEC(map);
}

void map_remove_nonexistent(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k = new(String, "k");
    call(map, put, REFCTMP(k), REFCTMP(new(String, "v")));
    ASSERT_EQUAL(call(map, get_length), 1);

    String * absent = new(String, "absent");
    call(map, remove, absent);
    ASSERT_EQUAL(call(map, get_length), 1);
    ASSERT_TRUE(call(map, contains_key, k));

    REFCDEC(absent);
    REFCDEC(map);
}

void map_get_keys(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k1 = new(String, "first");
    String * k2 = new(String, "second");
    call(map, put, REFCTMP(k1), REFCTMP(new(String, "v1")));
    call(map, put, REFCTMP(k2), REFCTMP(new(String, "v2")));

    List * keys = call(map, get_keys);
    ASSERT_EQUAL(keys->length, 2);

    String * first = call(keys, get, 0);
    String * second = call(keys, get, 1);
    bool first_ok = call(first, equals_cstring, "first") || call(first, equals_cstring, "second");
    bool second_ok = call(second, equals_cstring, "first") || call(second, equals_cstring, "second");
    bool distinct = call(first, equals_cstring, "first") ? call(second, equals_cstring, "second") : call(second, equals_cstring, "first");
    ASSERT_TRUE(first_ok);
    ASSERT_TRUE(second_ok);
    ASSERT_TRUE(distinct);
    REFCDEC(first);
    REFCDEC(second);

    REFCDEC(keys);
    REFCDEC(map);
}

void map_get_nonexistent(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k = new(String, "missing");
    ASSERT_NULL(call(map, get, k));
    ASSERT_EQUAL(call(map, get_length), 0);
    REFCDEC(k);
    REFCDEC(map);
}

void map_contains_key_value(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k = new(String, "k");
    String * v = new(String, "v");
    call(map, put, REFCTMP(k), REFCTMP(v));

    ASSERT_TRUE(call(map, contains_key, k));
    ASSERT_TRUE(call(map, contains_value, v));

    String * other_key = new(String, "other");
    ASSERT_FALSE(call(map, contains_key, other_key));

    String * other_val = new(String, "other");
    ASSERT_FALSE(call(map, contains_value, other_val));

    call(map, remove, k);
    ASSERT_FALSE(call(map, contains_key, k));
    ASSERT_FALSE(call(map, contains_value, v));

    REFCDEC(other_key);
    REFCDEC(other_val);
    REFCDEC(map);
}

void map_empty_ops(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    ASSERT_EQUAL(call(map, get_length), 0);

    String * any = new(String, "any");
    ASSERT_NULL(call(map, get, any));
    ASSERT_FALSE(call(map, contains_key, any));
    ASSERT_FALSE(call(map, contains_value, any));
    REFCDEC(any);

    String * s = call(map, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "{}");
    REFCDEC(s);
    REFCDEC(map);
}

void map_equals_key(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k1 = new(String, "same");
    String * k2 = new(String, "same");
    ASSERT_POINTERS_NOT_EQUAL(k1, k2);

    call(map, put, REFCTMP(k1), REFCTMP(new(String, "value")));
    ASSERT_TRUE(call(map, contains_key, k2));

    String * v = call(map, get, k2);
    ASSERT_STRINGS_EQUAL(call(v, to_cstring), "value");
    REFCDEC(v);

    REFCDEC(k2);
    REFCDEC(map);
}

void map_remove_then_put(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    String * k = new(String, "key");

    call(map, put, k, REFCTMP(new(String, "first")));
    ASSERT_EQUAL(call(map, get_length), 1);

    call(map, remove, k);
    ASSERT_EQUAL(call(map, get_length), 0);

    call(map, put, k, REFCTMP(new(String, "second")));
    ASSERT_EQUAL(call(map, get_length), 1);

    String * v = call(map, get, k);
    ASSERT_STRINGS_EQUAL(call(v, to_cstring), "second");
    REFCDEC(v);

    REFCDEC(k);
    REFCDEC(map);
}

void map_to_string(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);

    String * key1 = new(String, "key1");
    String * key2 = new(String, "key2");
    String * key3 = new(String, "key3");

    call(map, put, REFCTMP(key1), REFCTMP(new(String, "abc")));
    call(map, put, REFCTMP(key2), REFCTMP(new(Integer, 5)));
    call(map, put, REFCTMP(key3), NULL);

    String * s = call(map, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "{key1=abc; key2=5; key3=NULL}");

    REFCDEC(s);
    REFCDEC(map);
}

TEST_CASES_BEGIN
    TEST_CASE(map_basic);
    TEST_CASE(map_multiple_entries);
    TEST_CASE(map_update);
    TEST_CASE(map_remove_first);
    TEST_CASE(map_remove_middle);
    TEST_CASE(map_remove_last);
    TEST_CASE(map_remove_all);
    TEST_CASE(map_remove_nonexistent);
    TEST_CASE(map_get_keys);
    TEST_CASE(map_get_nonexistent);
    TEST_CASE(map_contains_key_value);
    TEST_CASE(map_empty_ops);
    TEST_CASE(map_equals_key);
    TEST_CASE(map_remove_then_put);
    TEST_CASE(map_to_string);
TEST_CASES_END

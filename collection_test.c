#include <stdio.h>
#include <stdlib.h>
#include "ooc_list.h"
#include "ooc_array.h"
#include "ooc_map.h"
#include "ooc_string.h"
#include "ooc_collection.h"

int main() {
    printf("Testing Collection base class and iterator\\n");

    // Test List
    printf("\\n--- Testing List ---\\n");
    List *list = new(List);
    call(list, add, new(String, "alice"));
    call(list, add, new(String, "bob"));
    call(list, add, new(String, "charlie"));

    Iterator *it = call(list, iterator);
    if (!it) {
        printf("ERROR: Failed to get iterator from List\\n");
        return 1;
    }
    printf("Got iterator from List\\n");

    int count = 0;
    while (call(it, has_next)) {
        call(it, next); // advance
        count++;
    }
    printf("List iteration count: %d (expected 3)\\n", count);
    if (count != 3) {
        printf("ERROR: List iteration count mismatch\\n");
        return 1;
    }

    // Test Array
    printf("\\n--- Testing Array ---\\n");
    Array *array = new(Array, 3);
    call(array, set, 0, new(String, "x"));
    call(array, set, 1, new(String, "y"));
    call(array, set, 2, new(String, "z"));

    Iterator *it2 = call(array, iterator);
    if (!it2) {
        printf("ERROR: Failed to get iterator from Array\\n");
        return 1;
    }
    printf("Got iterator from Array\\n");

    count = 0;
    while (call(it2, has_next)) {
        call(it2, next);
        count++;
    }
    printf("Array iteration count: %d (expected 3)\\n", count);
    if (count != 3) {
        printf("ERROR: Array iteration count mismatch\\n");
        return 1;
    }

    // Test Map
    printf("\\n--- Testing Map ---\\n");
    Map *map = new(Map);
    call(map, put, new(String, "key1"), new(String, "value1"));
    call(map, put, new(String, "key2"), new(String, "value2"));
    call(map, put, new(String, "key3"), new(String, "value3"));

    Iterator *it3 = call(map, iterator);
    if (!it3) {
        printf("ERROR: Failed to get iterator from Map\\n");
        return 1;
    }
    printf("Got iterator from Map\\n");

    count = 0;
    while (call(it3, has_next)) {
        call(it3, next);
        count++;
    }
    printf("Map iteration count: %d (expected 3)\\n", count);
    if (count != 3) {
        printf("ERROR: Map iteration count mismatch\\n");
        return 1;
    }

    printf("\\nAll tests passed!\\n");
    return 0;
}
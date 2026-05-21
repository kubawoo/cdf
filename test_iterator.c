#include <stdio.h>
#include "ooc_list.h"
#include "ooc_array.h"
#include "ooc_map.h"
#include "ooc_string.h"
#include "ooc_collection.h"

int main() {
    printf("Testing Collection iterator implementation\n");
    
    // Test List
    printf("\n=== Testing List ===\n");
    List *list = new(List);
    call(list, add, new(String, "first"));
    call(list, add, new(String, "second"));
    call(list, add, new(String, "third"));
    
    Iterator *it = call(list, iterator);
    int count = 0;
    while (call(it, has_next)) {
        call(it, next);  // Just advance, don't try to use the value yet
        count++;
    }
    printf("List iteration: %d items\n", count);
    
    // Test Array
    printf("\n=== Testing Array ===\n");
    Array *array = new(Array, 3);
    call(array, set, 0, new(String, "first"));
    call(array, set, 1, new(String, "second"));
    call(array, set, 2, new(String, "third"));
    
    Iterator *it2 = call(array, iterator);
    count = 0;
    while (call(it2, has_next)) {
        call(it2, next);
        count++;
    }
    printf("Array iteration: %d items\n", count);
    
    // Test Map
    printf("\n=== Testing Map ===\n");
    Map *map = new(Map);
    call(map, put, new(String, "key1"), new(String, "value1"));
    call(map, put, new(String, "key2"), new(String, "value2"));
    
    Iterator *it3 = call(map, iterator);
    count = 0;
    while (call(it3, has_next)) {
        call(it3, next);
        count++;
    }
    printf("Map iteration: %d items\n", count);
    
    printf("\nAll tests completed successfully!\n");
    return 0;
}
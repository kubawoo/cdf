#include <stdio.h>
#include "ooc_list.h"
#include "ooc_collection.h"

int main() {
    printf("Testing Collection iterator\\n");
    
    List *list = new(List);
    printf("List created\\n");
    
    // Check if iterator method exists
    if (list->iterator) {
        printf("List has iterator method\\n");
    } else {
        printf("List missing iterator method\\n");
        return 1;
    }
    
    Iterator *it = call(list, iterator);
    if (it) {
        printf("Got iterator\\n");
        if (it->has_next && it->next) {
            printf("Iterator has required methods\\n");
        } else {
            printf("Iterator missing methods\\n");
            return 1;
        }
    } else {
        printf("Failed to get iterator\\n");
        return 1;
    }
    
    printf("SUCCESS: Collection iterator implementation is present\\n");
    return 0;
}
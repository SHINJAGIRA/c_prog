#include "sport_resource.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

void resource_list_init(ResourceList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void resource_list_free(ResourceList *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

bool resource_list_ensure_capacity(ResourceList *list, size_t min_capacity) {
    if (min_capacity <= list->capacity) return true;
    
    size_t new_capacity = list->capacity == 0 ? INITIAL_CAPACITY : list->capacity * 2;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }
    
    SportResource *new_items = realloc(list->items, new_capacity * sizeof(SportResource));
    if (!new_items) return false;
    
    list->items = new_items;
    list->capacity = new_capacity;
    return true;
}
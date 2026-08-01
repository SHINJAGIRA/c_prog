#ifndef SPORT_RESOURCE_H
#define SPORT_RESOURCE_H

#include <stdbool.h>

typedef struct {
    int id;
    char name[101];
    char category[51];
    char location[101];
    int quantity;
    bool available;
    char created_at[20];
} SportResource;

// Dynamic array for storing multiple resources
typedef struct {
    SportResource *items;
    size_t count;
    size_t capacity;
} ResourceList;

// Initialize an empty resource list
void resource_list_init(ResourceList *list);

// Free memory allocated for the list
void resource_list_free(ResourceList *list);

// Ensure capacity for at least `min_capacity` elements
bool resource_list_ensure_capacity(ResourceList *list, size_t min_capacity);

#endif
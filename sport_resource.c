#include "sport_resource.h"
#include <stdlib.h>
#include <string.h>

void array_init(ResourceArray *arr) {
    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

void array_free(ResourceArray *arr) {
    free(arr->data);
    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

int array_push(ResourceArray *arr, Resource *r) {
    if (arr->count >= arr->capacity) {
        int new_cap = arr->capacity == 0 ? 4 : arr->capacity * 2;
        Resource *new_data = realloc(arr->data, new_cap * sizeof(Resource));
        if (!new_data) return 0;
        arr->data = new_data;
        arr->capacity = new_cap;
    }
    arr->data[arr->count++] = *r;
    return 1;
}
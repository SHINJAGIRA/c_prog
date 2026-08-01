#ifndef SPORT_RESOURCE_H
#define SPORT_RESOURCE_H

/* ============ Resource Entity ============ */

typedef struct {
    int id;
    char name[100];
    char category[50];
    char location[100];
    int quantity;
    int available;
} Resource;

/* ============ Dynamic Array ============ */

typedef struct {
    Resource *data;
    int count;
    int capacity;
} ResourceArray;

void array_init(ResourceArray *arr);
void array_free(ResourceArray *arr);
int array_push(ResourceArray *arr, Resource *r);

/* ============ User Entity ============ */

typedef struct {
    int id;
    char username[50];
    char password[50];
    int is_admin;
} User;

/* Global current user (defined in auth.c) */
extern User current_user;

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sport_resource.h"
#include "db.h"

void print_resource(const SportResource *r) {
    printf("\n┌────────────────────────────────────────┐\n");
    printf("│ ID:        %d\n", r->id);
    printf("│ Name:      %s\n", r->name);
    printf("│ Category:  %s\n", r->category);
    printf("│ Location:  %s\n", r->location);
    printf("│ Quantity:  %d\n", r->quantity);
    printf("│ Available: %s\n", r->available ? "Yes" : "No");
    printf("│ Created:   %s\n", r->created_at);
    printf("└────────────────────────────────────────┘\n");
}

void print_menu() {
    printf("\n========== Campus Sport Resources ==========\n");
    printf("1. Create Resource\n");
    printf("2. List All Resources\n");
    printf("3. Get Resource by ID\n");
    printf("4. Update Resource\n");
    printf("5. Delete Resource\n");
    printf("0. Exit\n");
    printf("Choice: ");
}

SportResource input_resource() {
    SportResource r = {0};
    printf("Name: ");
    scanf(" %100[^\n]", r.name);
    printf("Category: ");
    scanf(" %50[^\n]", r.category);
    printf("Location: ");
    scanf(" %100[^\n]", r.location);
    printf("Quantity: ");
    scanf("%d", &r.quantity);
    printf("Available (1=Yes, 0=No): ");
    int avail;
    scanf("%d", &avail);
    r.available = avail ? true : false;
    return r;
}

int main() {
    DBConfig config = {
        .host = "localhost",
        .user = "root",
        .password = "your_password",
        .database = "campus_sports",
        .port = 3306
    };
    
    MYSQL *conn = db_connect(&config);
    if (!conn) return EXIT_FAILURE;
    
    int choice;
    do {
        print_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: {
                SportResource r = input_resource();
                if (db_create_resource(conn, &r)) {
                    printf("Resource created successfully! (ID: %ld)\n", mysql_insert_id(conn));
                }
                break;
            }
            case 2: {
                ResourceList list;
                if (db_get_all_resources(conn, &list)) {
                    printf("\n--- Found %zu resource(s) ---\n", list.count);
                    for (size_t i = 0; i < list.count; i++) {
                        print_resource(&list.items[i]);
                    }
                    resource_list_free(&list);
                }
                break;
            }
            case 3: {
                int id;
                printf("Enter ID: ");
                scanf("%d", &id);
                SportResource r;
                if (db_get_resource_by_id(conn, id, &r)) {
                    print_resource(&r);
                } else {
                    printf("Resource not found.\n");
                }
                break;
            }
            case 4: {
                int id;
                printf("Enter ID to update: ");
                scanf("%d", &id);
                SportResource r = input_resource();
                if (db_update_resource(conn, id, &r)) {
                    printf("Resource updated successfully!\n");
                }
                break;
            }
            case 5: {
                int id;
                printf("Enter ID to delete: ");
                scanf("%d", &id);
                if (db_delete_resource(conn, id)) {
                    printf("Resource deleted successfully!\n");
                }
                break;
            }
            case 0:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
    
    db_disconnect(conn);
    return 0;
}
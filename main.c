#include <stdio.h>
#include <stdlib.h>
#include "sport_resource.h"
#include "db.h"
#include "auth.h"

/* ============ Input/Output Helpers ============ */

Resource input_resource(void) {
    Resource r = {0};
    printf("Name: ");      scanf(" %99[^\n]", r.name);
    printf("Category: ");  scanf(" %49[^\n]", r.category);
    printf("Location: ");  scanf(" %99[^\n]", r.location);
    printf("Quantity: ");  scanf("%d", &r.quantity);
    printf("Available (1=yes, 0=no): "); scanf("%d", &r.available);
    return r;
}

void print_resource(Resource *r) {
    printf("  [%d] %s | %s | %s | qty:%d | %s\n",
           r->id, r->name, r->category, r->location,
           r->quantity, r->available ? "available" : "unavailable");
}

void print_menu(void) {
    printf("\n--- Campus Sport Resources ---\n");
    printf("Logged in as: %s [%s]\n",
           auth_get_username(),
           auth_is_admin() ? "Admin" : "User");
    printf("1. Create Resource\n");
    printf("2. List All Resources\n");
    printf("3. Find One Resource\n");
    printf("4. Update Resource\n");
    printf("5. Delete Resource\n");
    if (auth_is_admin()) {
        printf("6. Register New User\n");
    }
    printf("0. Logout\n");
    printf("> ");
}

/* ============ Main ============ */

int main() {
    const char *DB_HOST = "localhost";
    const char *DB_USER = "root";
    const char *DB_PASS = "your_password";
    const char *DB_NAME = "campus_sports";
    unsigned int DB_PORT = 3306;

    MYSQL *conn = db_connect(DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT);
    if (!conn) return 1;

    db_ensure_tables(conn);

    if (!auth_login_screen(conn)) {
        db_disconnect(conn);
        return 0;
    }

    int choice, id;
    do {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                Resource r = input_resource();
                id = db_create_resource(conn, &r);
                if (id) printf("Created with ID %d\n", id);
                break;
            }
            case 2: {
                ResourceArray arr;
                if (db_read_all_resources(conn, &arr)) {
                    printf("\n--- %d resource(s) ---\n", arr.count);
                    for (int i = 0; i < arr.count; i++) {
                        print_resource(&arr.data[i]);
                    }
                    array_free(&arr);
                }
                break;
            }
            case 3: {
                printf("ID: "); scanf("%d", &id);
                Resource r;
                if (db_read_one_resource(conn, id, &r)) print_resource(&r);
                else printf("Not found.\n");
                break;
            }
            case 4: {
                printf("ID to update: "); scanf("%d", &id);
                Resource r = input_resource();
                if (db_update_resource(conn, id, &r)) printf("Updated.\n");
                else printf("Update failed.\n");
                break;
            }
            case 5: {
                printf("ID to delete: "); scanf("%d", &id);
                if (db_delete_resource(conn, id)) printf("Deleted.\n");
                else printf("Delete failed.\n");
                break;
            }
            case 6: {
                if (!auth_is_admin()) {
                    printf("Access denied.\n");
                    break;
                }
                char new_user[50], new_pass[50];
                int is_admin_flag;
                printf("New username: "); scanf(" %49[^\n]", new_user);
                printf("New password: "); scanf(" %49[^\n]", new_pass);
                printf("Is admin (1=yes, 0=no): "); scanf("%d", &is_admin_flag);
                if (db_register_user(conn, new_user, new_pass, is_admin_flag)) {
                    printf("User registered.\n");
                }
                break;
            }
            case 0:
                printf("Logged out. Goodbye, %s!\n", auth_get_username());
                break;
            default:
                printf("Invalid.\n");
        }
    } while (choice != 0);

    db_disconnect(conn);
    return 0;
}
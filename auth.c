#include "auth.h"
#include "sport_resource.h"
#include <stdio.h>
#include <string.h>

int auth_login_screen(MYSQL *conn) {
    char username[50], password[50];
    int choice;
    
    printf("\n========== Authentication ==========\n");
    printf("1. Login\n");
    printf("2. Exit\n");
    printf("Choice: ");
    scanf("%d", &choice);
    
    if (choice == 2) return 0;
    
    printf("Username: "); scanf(" %49[^\n]", username);
    printf("Password: "); scanf(" %49[^\n]", password);
    
    if (db_login(conn, username, password)) {
        printf("\nWelcome, %s! (%s)\n",
               current_user.username,
               current_user.is_admin ? "Admin" : "User");
        return 1;
    } else {
        printf("Invalid username or password.\n");
        return auth_login_screen(conn);
    }
}

int auth_is_admin(void) {
    return current_user.is_admin;
}

const char* auth_get_username(void) {
    return current_user.username;
}
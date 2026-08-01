#ifndef DB_H
#define DB_H

#include <mysql.h>
#include "sport_resource.h"

typedef struct {
    MYSQL *conn;
    char *host;
    char *user;
    char *password;
    char *database;
    unsigned int port;
} DBConfig;

// Initialize and connect to MySQL
MYSQL* db_connect(const DBConfig *config);

// Disconnect and cleanup
void db_disconnect(MYSQL *conn);

// CRUD Operations
bool db_create_resource(MYSQL *conn, const SportResource *resource);
bool db_get_all_resources(MYSQL *conn, ResourceList *out_list);
bool db_get_resource_by_id(MYSQL *conn, int id, SportResource *out);
bool db_update_resource(MYSQL *conn, int id, const SportResource *resource);
bool db_delete_resource(MYSQL *conn, int id);

// Utility
void db_print_error(MYSQL *conn, const char *context);

#endif
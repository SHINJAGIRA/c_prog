#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void db_print_error(MYSQL *conn, const char *context) {
    fprintf(stderr, "[DB ERROR] %s: %s\n", context, mysql_error(conn));
}

MYSQL* db_connect(const DBConfig *config) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }
    
    if (!mysql_real_connect(conn, config->host, config->user, 
                            config->password, config->database,
                            config->port, NULL, 0)) {
        db_print_error(conn, "Connection failed");
        mysql_close(conn);
        return NULL;
    }
    
    printf("Connected to MySQL database: %s\n", config->database);
    return conn;
}

void db_disconnect(MYSQL *conn) {
    if (conn) {
        mysql_close(conn);
        printf("Disconnected from MySQL.\n");
    }
}

bool db_create_resource(MYSQL *conn, const SportResource *resource) {
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO sport_resources (name, category, location, quantity, available) "
        "VALUES ('%s', '%s', '%s', %d, %d)",
        resource->name, resource->category, resource->location,
        resource->quantity, resource->available ? 1 : 0);
    
    if (mysql_query(conn, query)) {
        db_print_error(conn, "INSERT failed");
        return false;
    }
    return true;
}

bool db_get_all_resources(MYSQL *conn, ResourceList *out_list) {
    resource_list_init(out_list);
    
    if (mysql_query(conn, "SELECT id, name, category, location, quantity, available, created_at FROM sport_resources")) {
        db_print_error(conn, "SELECT all failed");
        return false;
    }
    
    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) {
        db_print_error(conn, "mysql_store_result failed");
        return false;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (!resource_list_ensure_capacity(out_list, out_list->count + 1)) {
            fprintf(stderr, "Memory allocation failed\n");
            mysql_free_result(result);
            resource_list_free(out_list);
            return false;
        }
        
        SportResource *r = &out_list->items[out_list->count++];
        r->id = atoi(row[0]);
        strncpy(r->name, row[1], sizeof(r->name) - 1);
        r->name[sizeof(r->name) - 1] = '\0';
        strncpy(r->category, row[2], sizeof(r->category) - 1);
        r->category[sizeof(r->category) - 1] = '\0';
        strncpy(r->location, row[3], sizeof(r->location) - 1);
        r->location[sizeof(r->location) - 1] = '\0';
        r->quantity = atoi(row[4]);
        r->available = atoi(row[5]) ? true : false;
        strncpy(r->created_at, row[6], sizeof(r->created_at) - 1);
        r->created_at[sizeof(r->created_at) - 1] = '\0';
    }
    
    mysql_free_result(result);
    return true;
}

bool db_get_resource_by_id(MYSQL *conn, int id, SportResource *out) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT id, name, category, location, quantity, available, created_at "
        "FROM sport_resources WHERE id = %d", id);
    
    if (mysql_query(conn, query)) {
        db_print_error(conn, "SELECT by ID failed");
        return false;
    }
    
    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) return false;
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return false;
    }
    
    out->id = atoi(row[0]);
    strncpy(out->name, row[1], sizeof(out->name) - 1);
    out->name[sizeof(out->name) - 1] = '\0';
    strncpy(out->category, row[2], sizeof(out->category) - 1);
    out->category[sizeof(out->category) - 1] = '\0';
    strncpy(out->location, row[3], sizeof(out->location) - 1);
    out->location[sizeof(out->location) - 1] = '\0';
    out->quantity = atoi(row[4]);
    out->available = atoi(row[5]) ? true : false;
    strncpy(out->created_at, row[6], sizeof(out->created_at) - 1);
    out->created_at[sizeof(out->created_at) - 1] = '\0';
    
    mysql_free_result(result);
    return true;
}

bool db_update_resource(MYSQL *conn, int id, const SportResource *resource) {
    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE sport_resources SET "
        "name = '%s', category = '%s', location = '%s', "
        "quantity = %d, available = %d "
        "WHERE id = %d",
        resource->name, resource->category, resource->location,
        resource->quantity, resource->available ? 1 : 0, id);
    
    if (mysql_query(conn, query)) {
        db_print_error(conn, "UPDATE failed");
        return false;
    }
    
    if (mysql_affected_rows(conn) == 0) {
        fprintf(stderr, "No resource found with ID %d\n", id);
        return false;
    }
    return true;
}

bool db_delete_resource(MYSQL *conn, int id) {
    char query[128];
    snprintf(query, sizeof(query), "DELETE FROM sport_resources WHERE id = %d", id);
    
    if (mysql_query(conn, query)) {
        db_print_error(conn, "DELETE failed");
        return false;
    }
    
    if (mysql_affected_rows(conn) == 0) {
        fprintf(stderr, "No resource found with ID %d\n", id);
        return false;
    }
    return true;
}
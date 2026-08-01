#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============ Connection ============ */

MYSQL* db_connect(const char *host, const char *user,
                  const char *pass, const char *db, unsigned int port) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn || !mysql_real_connect(conn, host, user, pass, db, port, NULL, 0)) {
        fprintf(stderr, "Connect error: %s\n", mysql_error(conn));
        return NULL;
    }
    printf("Connected to %s\n", db);
    return conn;
}

void db_disconnect(MYSQL *conn) {
    mysql_close(conn);
    printf("Disconnected.\n");
}

/* ============ Table Setup ============ */

void db_ensure_tables(MYSQL *conn) {
    /* Users table */
    mysql_query(conn,
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "username VARCHAR(50) UNIQUE NOT NULL,"
        "password VARCHAR(50) NOT NULL,"
        "is_admin BOOLEAN DEFAULT FALSE,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")");

    /* Insert defaults if empty */
    mysql_query(conn, "SELECT COUNT(*) FROM users");
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    if (atoi(row[0]) == 0) {
        mysql_query(conn,
            "INSERT INTO users (username, password, is_admin) VALUES "
            "('admin', 'admin123', 1),"
            "('user', 'user123', 0)");
        printf("Default users: admin/admin123, user/user123\n");
    }
    mysql_free_result(res);

    /* Sport resources table */
    mysql_query(conn,
        "CREATE TABLE IF NOT EXISTS sport_resources ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "name VARCHAR(100) NOT NULL,"
        "category VARCHAR(50) NOT NULL,"
        "location VARCHAR(100) NOT NULL,"
        "quantity INT NOT NULL DEFAULT 1,"
        "available BOOLEAN NOT NULL DEFAULT TRUE,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")");
}

/* ============ Resource CRUD ============ */

int db_create_resource(MYSQL *conn, Resource *r) {
    char sql[512];
    snprintf(sql, sizeof(sql),
        "INSERT INTO sport_resources(name,category,location,quantity,available) "
        "VALUES('%s','%s','%s',%d,%d)",
        r->name, r->category, r->location, r->quantity, r->available);
    
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "Create error: %s\n", mysql_error(conn));
        return 0;
    }
    return (int)mysql_insert_id(conn);
}

int db_read_all_resources(MYSQL *conn, ResourceArray *out) {
    array_init(out);
    
    if (mysql_query(conn, "SELECT id,name,category,location,quantity,available FROM sport_resources")) {
        fprintf(stderr, "Read error: %s\n", mysql_error(conn));
        return 0;
    }
    
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Resource r;
        r.id = atoi(row[0]);
        strcpy(r.name, row[1]);
        strcpy(r.category, row[2]);
        strcpy(r.location, row[3]);
        r.quantity = atoi(row[4]);
        r.available = atoi(row[5]);
        if (!array_push(out, &r)) {
            fprintf(stderr, "Out of memory\n");
            array_free(out);
            mysql_free_result(res);
            return 0;
        }
    }
    mysql_free_result(res);
    return 1;
}

int db_read_one_resource(MYSQL *conn, int id, Resource *out) {
    char sql[256];
    snprintf(sql, sizeof(sql),
        "SELECT id,name,category,location,quantity,available FROM sport_resources WHERE id=%d", id);
    
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "Read error: %s\n", mysql_error(conn));
        return 0;
    }
    
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return 0;
    }
    
    out->id = atoi(row[0]);
    strcpy(out->name, row[1]);
    strcpy(out->category, row[2]);
    strcpy(out->location, row[3]);
    out->quantity = atoi(row[4]);
    out->available = atoi(row[5]);
    
    mysql_free_result(res);
    return 1;
}

int db_update_resource(MYSQL *conn, int id, Resource *r) {
    char sql[512];
    snprintf(sql, sizeof(sql),
        "UPDATE sport_resources SET name='%s',category='%s',location='%s',quantity=%d,available=%d WHERE id=%d",
        r->name, r->category, r->location, r->quantity, r->available, id);
    
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "Update error: %s\n", mysql_error(conn));
        return 0;
    }
    return mysql_affected_rows(conn) > 0;
}

int db_delete_resource(MYSQL *conn, int id) {
    char sql[128];
    snprintf(sql, sizeof(sql), "DELETE FROM sport_resources WHERE id=%d", id);
    
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "Delete error: %s\n", mysql_error(conn));
        return 0;
    }
    return mysql_affected_rows(conn) > 0;
}

/* ============ Auth ============ */

User current_user = {0};

int db_login(MYSQL *conn, const char *username, const char *password) {
    char sql[256];
    snprintf(sql, sizeof(sql),
        "SELECT id, username, is_admin FROM users "
        "WHERE username='%s' AND password='%s'",
        username, password);
    
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "Login error: %s\n", mysql_error(conn));
        return 0;
    }
    
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    if (!row) {
        mysql_free_result(res);
        return 0;
    }
    
    current_user.id = atoi(row[0]);
    strcpy(current_user.username, row[1]);
    current_user.is_admin = atoi(row[2]);
    
    mysql_free_result(res);
    return 1;
}

int db_register_user(MYSQL *conn, const char *username, const char *password, int is_admin) {
    char sql[256];
    snprintf(sql, sizeof(sql),
        "INSERT INTO users (username, password, is_admin) VALUES ('%s', '%s', %d)",
        username, password, is_admin);
    
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "Register error: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}
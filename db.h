#ifndef DB_H
#define DB_H

#include <mysql.h>
#include "sport_resource.h"

/* Connection */
MYSQL* db_connect(const char *host, const char *user,
                  const char *pass, const char *db, unsigned int port);
void db_disconnect(MYSQL *conn);

/* Table setup */
void db_ensure_tables(MYSQL *conn);

/* Resource CRUD */
int db_create_resource(MYSQL *conn, Resource *r);
int db_read_all_resources(MYSQL *conn, ResourceArray *out);
int db_read_one_resource(MYSQL *conn, int id, Resource *out);
int db_update_resource(MYSQL *conn, int id, Resource *r);
int db_delete_resource(MYSQL *conn, int id);

/* Auth */
int db_login(MYSQL *conn, const char *username, const char *password);
int db_register_user(MYSQL *conn, const char *username, const char *password, int is_admin);

#endif
#ifndef AUTH_H
#define AUTH_H

#include <mysql.h>

/* Login screen, returns 1 on success, 0 to exit */
int auth_login_screen(MYSQL *conn);

/* Check if current user is admin */
int auth_is_admin(void);

/* Get current username */
const char* auth_get_username(void);

#endif
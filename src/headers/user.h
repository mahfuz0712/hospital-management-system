#ifndef USER_H
#define USER_H

#define USERNAME_LEN 50
#define PASSWORD_LEN 50
#define USERS_FILE "data/users.dat"


typedef enum
{
    ROLE_ADMIN = 0,
    ROLE_EMPLOYEE = 1,
    ROLE_DOCTOR = 2
} Role;

typedef struct User
{
    int id;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    Role role;
    int linkedDoctorId; 
    struct User *next;
} User;

User *loadUsers(void);
void saveUsers(User *head);
void freeUsers(User *head);

/* ---- CRUD ---- */
User *addUser(User *head, const char *username, const char *password, Role role, int linkedDoctorId);
User *deleteUser(User *head, int id);
int updateUser(User *head, int id, const char *username, Role role, int linkedDoctorId);
int setUserPassword(User *head, int id, const char *newPlainPassword);
User *findUserByUsername(User *head, const char *username);
User *findUserById(User *head, int id);

/* ---- Auth helpers ---- */
int verifyPassword(const User *user, const char *attemptPlainPassword);
const char *roleToString(Role role);

#endif
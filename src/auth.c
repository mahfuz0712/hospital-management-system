#include <stdio.h>
#include <string.h>
#include "headers/auth.h"
#include "headers/utils.h"

#define MAX_LOGIN_ATTEMPTS 3
#define DEFAULT_ADMIN_USERNAME "admin"
#define DEFAULT_ADMIN_PASSWORD "admin123"

User *ensureDefaultAdmin(User *head)
{
    if (head != NULL)
    {
        /* users.dat already has data — nothing to seed. */
        return head;
    }

    head = addUser(head, DEFAULT_ADMIN_USERNAME, DEFAULT_ADMIN_PASSWORD, ROLE_ADMIN, -1);
    saveUsers(head);
    printf("============================================\n");
    printf(" First run detected: default admin created.\n");
    printf(" Username: %s\n", DEFAULT_ADMIN_USERNAME);
    printf(" Password: %s\n", DEFAULT_ADMIN_PASSWORD);
    printf(" Please log in and change this immediately.\n");
    printf("============================================\n");
    pauseScreen();
    return head;
}

User *login(User *head)
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++)
    {
        clearScreen();
        printf("==========================================\n");
        printf("   HOSPITAL MANAGEMENT SYSTEM - LOGIN\n");
        printf("==========================================\n");
        printf("Attempt %d of %d\n\n", attempt, MAX_LOGIN_ATTEMPTS);

        printf("Username: ");
        readLine(username, USERNAME_LEN);

        printf("Password: ");
        readLine(password, PASSWORD_LEN);

        User *match = findUserByUsername(head, username);

        if (match != NULL && verifyPassword(match, password))
        {
            printf("\nLogin successful. Welcome, %s (%s).\n", match->username, roleToString(match->role));
            pauseScreen();
            return match;
        }

        printf("\nInvalid username or password.\n");
        if (attempt < MAX_LOGIN_ATTEMPTS)
        {
            pauseScreen();
        }
    }

    printf("\nToo many failed attempts. Exiting login.\n");
    pauseScreen();
    return NULL;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/user.h"

#define XOR_KEY "NUB_CSE_SD1"

static void xorTransform(const char *input, char *output) {
    size_t keyLen = strlen(XOR_KEY);
    size_t len = strlen(input);

    for (size_t i = 0; i < len; i++)
    {
        output[i] = input[i] ^ XOR_KEY[i % keyLen];
    }
    output[len] = '\0';
}

User *loadUsers(void) {
    FILE *fp = fopen(USERS_FILE, "rb");
    if (fp == NULL)
    {
        /* File not created yet — first run. Not an error. */
        return NULL;
    }

    User *head = NULL;
    User *tail = NULL;
    User temp;

    while (fread(&temp, sizeof(User), 1, fp) == 1)
    {
        User *node = (User *)malloc(sizeof(User));
        if (node == NULL)
        {
            printf("Memory allocation failed while loading users.\n");
            break;
        }
        *node = temp;
        node->next = NULL;

        if (head == NULL)
        {
            head = node;
            tail = node;
        }
        else
        {
            tail->next = node;
            tail = node;
        }
    }

    fclose(fp);
    return head;
}

void saveUsers(User *head) {
    FILE *fp = fopen(USERS_FILE, "wb");
    if (fp == NULL)
    {
        printf("Error: could not open %s for writing.\n", USERS_FILE);
        return;
    }

    for (User *curr = head; curr != NULL; curr = curr->next)
    {
        fwrite(curr, sizeof(User), 1, fp);
    }

    fclose(fp);
}

void freeUsers(User *head) {
    User *curr = head;
    while (curr != NULL)
    {
        User *next = curr->next;
        free(curr);
        curr = next;
    }
}

User *addUser(User *head, const char *username, const char *password, Role role, int linkedDoctorId, int linkedPatientId) {
    User *node = (User *)malloc(sizeof(User));
    if (node == NULL)
    {
        printf("Memory allocation failed while adding user.\n");
        return head;
    }

    int maxId = 0;
    for (User *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id > maxId)
        {
            maxId = curr->id;
        }
    }
    node->id = maxId + 1;

    strncpy(node->username, username, USERNAME_LEN - 1);
    node->username[USERNAME_LEN - 1] = '\0';

    xorTransform(password, node->password);

    node->role = role;
    node->linkedDoctorId = linkedDoctorId;
    node->linkedPatientId = linkedPatientId;
    node->next = NULL;

    if (head == NULL)
    {
        return node;
    }

    User *curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }
    curr->next = node;

    return head;
}

User *deleteUser(User *head, int id) {
    User *curr = head;
    User *prev = NULL;

    while (curr != NULL)
    {
        if (curr->id == id)
        {
            if (prev == NULL)
            {
                head = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }
            free(curr);
            return head;
        }
        prev = curr;
        curr = curr->next;
    }

    /* id not found — list returned unchanged. */
    return head;
}

int updateUser(User *head, int id, const char *username, Role role, int linkedDoctorId, int linkedPatientId) {

    for (User *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id == id)
        {
            strncpy(curr->username, username, USERNAME_LEN - 1);
            curr->username[USERNAME_LEN - 1] = '\0';

            curr->role = role;
            curr->linkedDoctorId = linkedDoctorId;
            curr->linkedPatientId = linkedPatientId;

            return 1;
        }
    }

    /* id not found. */
    return 0;
}

int setUserPassword(User *head, int id, const char *newPlainPassword) {
    for (User *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id == id)
        {

            xorTransform(newPlainPassword, curr->password);
            return 1;
        }
    }

    /* id not found. */
    return 0;
}

User *findUserByUsername(User *head, const char *username) {

    for (User *curr = head; curr != NULL; curr = curr->next)
    {
        if (strcmp(curr->username, username) == 0)
        {
            return curr;
        }
    }
    return NULL;
}

User *findUserById(User *head, int id) {
    for (User *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id == id)
        {
            return curr;
        }
    }
    return NULL;
}

int verifyPassword(const User *user, const char *attemptPlainPassword) {
    if (user == NULL)
        return 0;

    char scrambledAttempt[PASSWORD_LEN];
    xorTransform(attemptPlainPassword, scrambledAttempt);

    return strcmp(user->password, scrambledAttempt) == 0;
}

const char *roleToString(Role role) {
    switch (role)
    {
    case ROLE_ADMIN:
        return "Administrator";
    case ROLE_EMPLOYEE:
        return "Employee";
    case ROLE_DOCTOR:
        return "Doctor";
    case ROLE_PATIENT:
        return "Patient";
    default:
        return "Unknown";
    }
}
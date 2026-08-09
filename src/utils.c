#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "headers/utils.h"

void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen(void) {
    printf("\nPress Enter to continue...");
    getchar();
}

void trimString(char *str) {
    if (str == NULL)
        return;
    int start = 0;
    while (str[start] != '\0' && isspace((unsigned char)str[start]))
    {
        start++;
    }

    if (start > 0)
    {
        int i = 0;
        while (str[start + i] != '\0')
        {
            str[i] = str[start + i];
            i++;
        }
        str[i] = '\0';
    }

    /* Trim trailing whitespace by walking back from the end. */
    int len = (int)strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        str[len - 1] = '\0';
        len--;
    }
}

void readLine(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL)
    {
        /* fgets keeps the trailing newline; strip it so stored
         * strings never contain an embedded '\n'. */
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
    }
    else
    {
        buffer[0] = '\0';
    }
    trimString(buffer);
}

int readInt(const char *prompt) {
    char temp[32];
    int value;
    char *endPtr;

    while (1)
    {
        printf("%s", prompt);
        readLine(temp, sizeof(temp));

        /* strtol lets us detect trailing garbage (e.g. "12abc")
         * which plain atoi() would silently accept as 12. */
        value = (int)strtol(temp, &endPtr, 10);

        if (temp[0] != '\0' && *endPtr == '\0')
        {
            return value;
        }
        printf("Invalid input. Please enter a whole number.\n");
    }
}

float readFloat(const char *prompt) {
    char temp[32];
    float value;
    char *endPtr;

    while (1)
    {
        printf("%s", prompt);
        readLine(temp, sizeof(temp));

        value = strtof(temp, &endPtr);

        if (temp[0] != '\0' && *endPtr == '\0')
        {
            return value;
        }
        printf("Invalid input. Please enter a valid number (e.g. 250.50).\n");
    }
}
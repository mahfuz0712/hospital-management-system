#include <string.h>
#include <ctype.h>
#include "headers/validation.h"

/* for checking dates*/
int isValidDate(const char *date)
{
    /* Expected length is exactly 10: "DD-MM-YYYY" */
    if (date == NULL || strlen(date) != 10)
    {
        return 0;
    }

    /* Dashes must sit at index 2 and index 5. */
    if (date[2] != '-' || date[5] != '-')
    {
        return 0;
    }

    /* Every other character must be a digit. */
    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
            continue;
        if (!isdigit((unsigned char)date[i]))
        {
            return 0;
        }
    }

    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');

    if (day < 1 || day > 31)
        return 0;
    if (month < 1 || month > 12)
        return 0;

    return 1;
}

int isValidTime(const char *time)
{
    /* Expected length is exactly 5: "HH:MM" */
    if (time == NULL || strlen(time) != 5)
    {
        return 0;
    }

    if (time[2] != ':')
    {
        return 0;
    }

    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
            continue;
        if (!isdigit((unsigned char)time[i]))
        {
            return 0;
        }
    }

    int hour = (time[0] - '0') * 10 + (time[1] - '0');
    int minute = (time[3] - '0') * 10 + (time[4] - '0');

    if (hour < 0 || hour > 23)
        return 0;
    if (minute < 0 || minute > 59)
        return 0;

    return 1;
}

int isValidPhone(const char *phone)
{
    if (phone == NULL)
        return 0;

    size_t len = strlen(phone);
    if (len < 7 || len > 15)
    {
        return 0;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (!isdigit((unsigned char)phone[i]))
        {
            return 0;
        }
    }

    return 1;
}

int isNonEmpty(const char *str)
{
    if (str == NULL)
        return 0;

    while (*str != '\0')
    {
        if (!isspace((unsigned char)*str))
        {
            return 1;
        }
        str++;
    }
    return 0;
}
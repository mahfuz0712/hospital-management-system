#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "headers/doctor.h"

Doctor *loadDoctors(void) {
    FILE *fp = fopen(DOCTORS_FILE, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    Doctor *head = NULL;
    Doctor *tail = NULL;
    Doctor temp;

    while (fread(&temp, sizeof(Doctor), 1, fp) == 1) {
        Doctor *node = (Doctor *)malloc(sizeof(Doctor));
        if (node == NULL)
        {
            printf("Memory allocation failed while loading doctors.\n");
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

void saveDoctors(Doctor *head) {
    FILE *fp = fopen(DOCTORS_FILE, "wb");
    if (fp == NULL)
    {
        printf("Error: could not open %s for writing.\n", DOCTORS_FILE);
        return;
    }

    for (Doctor *curr = head; curr != NULL; curr = curr->next)
    {
        fwrite(curr, sizeof(Doctor), 1, fp);
    }

    fclose(fp);
}

void freeDoctors(Doctor *head) {
    Doctor *curr = head;
    while (curr != NULL)
    {
        Doctor *next = curr->next;
        free(curr);
        curr = next;
    }
}

Doctor *addDoctor(Doctor *head, const char *name, const char *specialization, const char *phone, Availability availability) {
    Doctor *node = (Doctor *)malloc(sizeof(Doctor));
    if (node == NULL) {
        printf("Memory allocation failed while adding doctor.\n");
        return head;
    }

    int maxId = 0;
    for (Doctor *curr = head; curr != NULL; curr = curr->next) {
        if (curr->id > maxId)
            maxId = curr->id;
    }
    node->id = maxId + 1;

    strncpy(node->name, name, DOCTOR_NAME_LEN - 1);
    node->name[DOCTOR_NAME_LEN - 1] = '\0';

    strncpy(node->specialization, specialization, DOCTOR_SPECIALIZATION_LEN - 1);
    node->specialization[DOCTOR_SPECIALIZATION_LEN - 1] = '\0';

    strncpy(node->phone, phone, DOCTOR_PHONE_LEN - 1);
    node->phone[DOCTOR_PHONE_LEN - 1] = '\0';

    node->availability = availability;
    node->next = NULL;

    if (head == NULL) {
        return node;
    }

    Doctor *curr = head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = node;

    return head;
}

int updateDoctor(Doctor *head, int id, const char *name, const char *specialization, const char *phone, Availability availability) {
    Doctor *d = findDoctorById(head, id);
    if (d == NULL) {
        return 0;
    }

    strncpy(d->name, name, DOCTOR_NAME_LEN - 1);
    d->name[DOCTOR_NAME_LEN - 1] = '\0';

    strncpy(d->specialization, specialization, DOCTOR_SPECIALIZATION_LEN - 1);
    d->specialization[DOCTOR_SPECIALIZATION_LEN - 1] = '\0';

    strncpy(d->phone, phone, DOCTOR_PHONE_LEN - 1);
    d->phone[DOCTOR_PHONE_LEN - 1] = '\0';

    d->availability = availability;

    return 1;
}

Doctor *deleteDoctor(Doctor *head, int id) {
    Doctor *curr = head;
    Doctor *prev = NULL;

    while (curr != NULL) {
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

    return head;
}

int setDoctorAvailability(Doctor *head, int id, Availability availability) {
    Doctor *d = findDoctorById(head, id);
    if (d == NULL)
    {
        return 0;
    }
    d->availability = availability;
    return 1;
}

Doctor *findDoctorById(Doctor *head, int id) {
    for (Doctor *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id == id)
        {
            return curr;
        }
    }
    return NULL;
}

static int containsIgnoreCase(const char *haystack, const char *needle) {
    size_t hLen = strlen(haystack);
    size_t nLen = strlen(needle);

    if (nLen == 0 || nLen > hLen)
    {
        return 0;
    }

    for (size_t i = 0; i <= hLen - nLen; i++)
    {
        size_t j = 0;
        while (j < nLen &&
               tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j]))
        {
            j++;
        }
        if (j == nLen)
        {
            return 1;
        }
    }
    return 0;
}

static int equalsIgnoreCase(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0')
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
        {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

Doctor *searchDoctorByName(Doctor *head, const char *namePart) {

    for (Doctor *curr = head; curr != NULL; curr = curr->next)
    {
        if (containsIgnoreCase(curr->name, namePart))
        {
            return curr;
        }
    }
    return NULL;
}

Doctor *findAvailableDoctorBySpecialization(Doctor *head, const char *specialization) {

    for (Doctor *curr = head; curr != NULL; curr = curr->next)
    {
        if (equalsIgnoreCase(curr->specialization, specialization) &&
            curr->availability == DOCTOR_AVAILABLE)
        {
            return curr;
        }
    }
    return NULL;
}

const char *availabilityToString(Availability a) {
    return (a == DOCTOR_AVAILABLE) ? "Available" : "Busy";
}

void displayDoctor(const Doctor *d) {
    if (d == NULL)
    {
        printf("Doctor not found.\n");
        return;
    }

    printf("------------------------------------------\n");
    printf("Doctor ID      : %d\n", d->id);
    printf("Name           : Dr. %s\n", d->name);
    printf("Specialization : %s\n", d->specialization);
    printf("Phone          : %s\n", d->phone);
    printf("Availability   : %s\n", availabilityToString(d->availability));
    printf("------------------------------------------\n");
}

void displayAllDoctors(Doctor *head) {
    if (head == NULL)
    {
        printf("No doctor records found.\n");
        return;
    }

    printf("%-5s %-20s %-20s %-15s %-10s\n", "ID", "Name", "Specialization", "Phone", "Status");
    printf("-----------------------------------------------------------------------------\n");

    for (Doctor *curr = head; curr != NULL; curr = curr->next) {
        printf("%-5d %-20s %-20s %-15s %-10s\n", curr->id, curr->name, curr->specialization, curr->phone, availabilityToString(curr->availability));
    }
}
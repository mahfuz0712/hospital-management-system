#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "headers/patient.h"

Patient *loadPatients(void)
{
    FILE *fp = fopen(PATIENTS_FILE, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    Patient *head = NULL;
    Patient *tail = NULL;
    Patient temp;

    while (fread(&temp, sizeof(Patient), 1, fp) == 1)
    {
        Patient *node = (Patient *)malloc(sizeof(Patient));
        if (node == NULL)
        {
            printf("Memory allocation failed while loading patients.\n");
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

void savePatients(Patient *head)
{
    FILE *fp = fopen(PATIENTS_FILE, "wb");
    if (fp == NULL)
    {
        printf("Error: could not open %s for writing.\n", PATIENTS_FILE);
        return;
    }

    for (Patient *curr = head; curr != NULL; curr = curr->next)
    {
        fwrite(curr, sizeof(Patient), 1, fp);
    }

    fclose(fp);
}

void freePatients(Patient *head)
{
    Patient *curr = head;
    while (curr != NULL)
    {
        Patient *next = curr->next;
        free(curr);
        curr = next;
    }
}

Patient *addPatient(Patient *head, const char *name, int age, const char *gender, const char *phone, const char *address, const char *diagnosis)
{
    Patient *node = (Patient *)malloc(sizeof(Patient));
    if (node == NULL)
    {
        printf("Memory allocation failed while adding patient.\n");
        return head;
    }

    int maxId = 0;
    for (Patient *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id > maxId)
            maxId = curr->id;
    }
    node->id = maxId + 1;

    strncpy(node->name, name, PATIENT_NAME_LEN - 1);
    node->name[PATIENT_NAME_LEN - 1] = '\0';

    node->age = age;

    strncpy(node->gender, gender, PATIENT_GENDER_LEN - 1);
    node->gender[PATIENT_GENDER_LEN - 1] = '\0';

    strncpy(node->phone, phone, PATIENT_PHONE_LEN - 1);
    node->phone[PATIENT_PHONE_LEN - 1] = '\0';

    strncpy(node->address, address, PATIENT_ADDRESS_LEN - 1);
    node->address[PATIENT_ADDRESS_LEN - 1] = '\0';

    strncpy(node->diagnosis, diagnosis, PATIENT_DIAGNOSIS_LEN - 1);
    node->diagnosis[PATIENT_DIAGNOSIS_LEN - 1] = '\0';

    node->next = NULL;

    if (head == NULL)
    {
        return node;
    }

    Patient *curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }
    curr->next = node;

    return head;
}

int updatePatient(Patient *head, int id, const char *name, int age, const char *gender, const char *phone, const char *address, const char *diagnosis)
{
    Patient *p = findPatientById(head, id);
    if (p == NULL)
    {
        return 0;
    }

    strncpy(p->name, name, PATIENT_NAME_LEN - 1);
    p->name[PATIENT_NAME_LEN - 1] = '\0';

    p->age = age;

    strncpy(p->gender, gender, PATIENT_GENDER_LEN - 1);
    p->gender[PATIENT_GENDER_LEN - 1] = '\0';

    strncpy(p->phone, phone, PATIENT_PHONE_LEN - 1);
    p->phone[PATIENT_PHONE_LEN - 1] = '\0';

    strncpy(p->address, address, PATIENT_ADDRESS_LEN - 1);
    p->address[PATIENT_ADDRESS_LEN - 1] = '\0';

    strncpy(p->diagnosis, diagnosis, PATIENT_DIAGNOSIS_LEN - 1);
    p->diagnosis[PATIENT_DIAGNOSIS_LEN - 1] = '\0';

    return 1;
}

Patient *deletePatient(Patient *head, int id)
{
    Patient *curr = head;
    Patient *prev = NULL;

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

    return head;
}

Patient *findPatientById(Patient *head, int id)
{
    for (Patient *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id == id)
        {
            return curr;
        }
    }
    return NULL;
}

/* Case-insensitive substring check, used by searchPatientByName. */
static int containsIgnoreCase(const char *haystack, const char *needle)
{
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

Patient *searchPatientByName(Patient *head, const char *namePart)
{
    for (Patient *curr = head; curr != NULL; curr = curr->next)
    {
        if (containsIgnoreCase(curr->name, namePart))
        {
            return curr;
        }
    }
    return NULL;
}

/* Comparison function for qsort, sorting Patient* pointers by id. */
static int comparePatientById(const void *a, const void *b)
{
    Patient *p1 = *(Patient *const *)a;
    Patient *p2 = *(Patient *const *)b;
    return p1->id - p2->id;
}

Patient *searchPatientByIdBinary(Patient *head, int id, int *outComparisons)
{
    int comparisons = 0;

    /* Step 1: count nodes so we know how big an array to allocate. */
    int count = 0;
    for (Patient *curr = head; curr != NULL; curr = curr->next)
    {
        count++;
    }

    if (count == 0)
    {
        if (outComparisons)
            *outComparisons = 0;
        return NULL;
    }

    /* Step 2: copy pointers into a temporary array. This is the
     * step that makes binary search possible at all: arrays give
     * O(1) access to any index, which a linked list cannot. */
    Patient **arr = (Patient **)malloc(sizeof(Patient *) * count);
    if (arr == NULL)
    {
        printf("Memory allocation failed during binary search.\n");
        if (outComparisons)
            *outComparisons = 0;
        return NULL;
    }

    int i = 0;
    for (Patient *curr = head; curr != NULL; curr = curr->next)
    {
        arr[i++] = curr;
    }

    /* Step 3: sort the array by id (qsort, standard library). */
    qsort(arr, count, sizeof(Patient *), comparePatientById);

    /* Step 4: classic binary search over the sorted array. */
    int low = 0, high = count - 1;
    Patient *result = NULL;

    while (low <= high)
    {
        comparisons++;
        int mid = low + (high - low) / 2;

        if (arr[mid]->id == id)
        {
            result = arr[mid];
            break;
        }
        else if (arr[mid]->id < id)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    free(arr);
    if (outComparisons)
        *outComparisons = comparisons;
    return result;
}

void displayPatient(const Patient *p)
{
    if (p == NULL)
    {
        printf("Patient not found.\n");
        return;
    }

    printf("------------------------------------------\n");
    printf("Patient ID   : %d\n", p->id);
    printf("Name         : %s\n", p->name);
    printf("Age          : %d\n", p->age);
    printf("Gender       : %s\n", p->gender);
    printf("Phone        : %s\n", p->phone);
    printf("Address      : %s\n", p->address);
    printf("Diagnosis    : %s\n", p->diagnosis);
    printf("------------------------------------------\n");
}

void displayAllPatients(Patient *head)
{
    if (head == NULL)
    {
        printf("No patient records found.\n");
        return;
    }

    printf("%-5s %-20s %-5s %-8s %-15s %-20s\n", "ID", "Name", "Age", "Gender", "Phone", "Diagnosis");
    printf("-----------------------------------------------------------------------------\n");

    for (Patient *curr = head; curr != NULL; curr = curr->next)
    {
        printf("%-5d %-20s %-5d %-8s %-15s %-20s\n", curr->id, curr->name, curr->age, curr->gender, curr->phone, curr->diagnosis);
    }
}
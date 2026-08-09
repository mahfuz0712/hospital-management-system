#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/appointment.h"

Appointment *loadAppointments(void)
{
    FILE *fp = fopen(APPOINTMENTS_FILE, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    Appointment *head = NULL;
    Appointment *tail = NULL;
    Appointment temp;

    while (fread(&temp, sizeof(Appointment), 1, fp) == 1)
    {
        Appointment *node = (Appointment *)malloc(sizeof(Appointment));
        if (node == NULL)
        {
            printf("Memory allocation failed while loading appointments.\n");
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

void saveAppointments(Appointment *head)
{
    FILE *fp = fopen(APPOINTMENTS_FILE, "wb");
    if (fp == NULL)
    {
        printf("Error: could not open %s for writing.\n", APPOINTMENTS_FILE);
        return;
    }

    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        fwrite(curr, sizeof(Appointment), 1, fp);
    }

    fclose(fp);
}

void freeAppointments(Appointment *head)
{
    Appointment *curr = head;
    while (curr != NULL)
    {
        Appointment *next = curr->next;
        free(curr);
        curr = next;
    }
}

Appointment *addAppointment(Appointment *head, int patientId, int doctorId,
                            const char *date, const char *time)
{
    Appointment *node = (Appointment *)malloc(sizeof(Appointment));
    if (node == NULL)
    {
        printf("Memory allocation failed while adding appointment.\n");
        return head;
    }

    int maxId = 0;
    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id > maxId)
            maxId = curr->id;
    }
    node->id = maxId + 1;

    node->patientId = patientId;
    node->doctorId = doctorId;

    strncpy(node->date, date, APPOINTMENT_DATE_LEN - 1);
    node->date[APPOINTMENT_DATE_LEN - 1] = '\0';

    strncpy(node->time, time, APPOINTMENT_TIME_LEN - 1);
    node->time[APPOINTMENT_TIME_LEN - 1] = '\0';

    node->status = APPT_SCHEDULED;
    node->next = NULL;

    if (head == NULL)
    {
        return node;
    }

    Appointment *curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }
    curr->next = node;

    return head;
}

Appointment *deleteAppointment(Appointment *head, int id)
{
    Appointment *curr = head;
    Appointment *prev = NULL;

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

int updateAppointmentStatus(Appointment *head, int id, AppointmentStatus status)
{
    Appointment *a = findAppointmentById(head, id);
    if (a == NULL)
    {
        return 0;
    }
    a->status = status;
    return 1;
}

Appointment *findAppointmentById(Appointment *head, int id)
{
    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id == id)
        {
            return curr;
        }
    }
    return NULL;
}

int isDoctorAvailableAt(Appointment *head, int doctorId,
                        const char *date, const char *time)
{

    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->doctorId != doctorId)
        {
            continue;
        }
        if (strcmp(curr->date, date) != 0 || strcmp(curr->time, time) != 0)
        {
            continue;
        }

        if (curr->status != APPT_CANCELLED)
        {
            return 1; /* slot is taken */
        }
    }

    return 0; /* slot is free */
}

int hasPatientVisitedDoctor(Appointment *head, int patientId, int doctorId) {
    for (Appointment *curr = head; curr != NULL; curr = curr->next) {
        if (curr->patientId == patientId && curr->doctorId == doctorId) {
            return 1;
        }
    }
    return 0;
}

void displayAppointmentsByPatientId(Appointment *head, int patientId)
{

    int found = 0;

    printf("%-5s %-10s %-10s %-12s %-6s %-12s\n",
           "ID", "PatientID", "DoctorID", "Date", "Time", "Status");
    printf("-----------------------------------------------------------------\n");

    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->patientId == patientId)
        {
            printf("%-5d %-10d %-10d %-12s %-6s %-12s\n",
                   curr->id, curr->patientId, curr->doctorId,
                   curr->date, curr->time,
                   appointmentStatusToString(curr->status));
            found = 1;
        }
    }

    if (!found)
    {
        printf("No appointments found for patient ID %d.\n", patientId);
    }
}

void displayAppointmentsByDoctorId(Appointment *head, int doctorId)
{

    int found = 0;

    printf("%-5s %-10s %-10s %-12s %-6s %-12s\n",
           "ID", "PatientID", "DoctorID", "Date", "Time", "Status");
    printf("-----------------------------------------------------------------\n");

    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->doctorId == doctorId)
        {
            printf("%-5d %-10d %-10d %-12s %-6s %-12s\n",
                   curr->id, curr->patientId, curr->doctorId,
                   curr->date, curr->time,
                   appointmentStatusToString(curr->status));
            found = 1;
        }
    }

    if (!found)
    {
        printf("No appointments found for doctor ID %d.\n", doctorId);
    }
}

static int dateToComparableInt(const char *date)
{
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
               (date[8] - '0') * 10 + (date[9] - '0');

    return year * 10000 + month * 100 + day;
}

static int compareAppointmentByDate(const void *a, const void *b)
{
    Appointment *p1 = *(Appointment *const *)a;
    Appointment *p2 = *(Appointment *const *)b;
    return dateToComparableInt(p1->date) - dateToComparableInt(p2->date);
}

Appointment *searchAppointmentByDateBinary(Appointment *head, const char *date,
                                           int *outComparisons)
{
    int comparisons = 0;

    int count = 0;
    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        count++;
    }

    if (count == 0)
    {
        if (outComparisons)
            *outComparisons = 0;
        return NULL;
    }

    Appointment **arr = (Appointment **)malloc(sizeof(Appointment *) * count);
    if (arr == NULL)
    {
        printf("Memory allocation failed during binary search.\n");
        if (outComparisons)
            *outComparisons = 0;
        return NULL;
    }

    int i = 0;
    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        arr[i++] = curr;
    }

    qsort(arr, count, sizeof(Appointment *), compareAppointmentByDate);

    int targetValue = dateToComparableInt(date);
    int low = 0, high = count - 1;
    Appointment *result = NULL;

    while (low <= high)
    {
        comparisons++;
        int mid = low + (high - low) / 2;
        int midValue = dateToComparableInt(arr[mid]->date);

        if (midValue == targetValue)
        {
            result = arr[mid];
            break;
        }
        else if (midValue < targetValue)
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

const char *appointmentStatusToString(AppointmentStatus s)
{
    switch (s)
    {
    case APPT_SCHEDULED:
        return "Scheduled";
    case APPT_COMPLETED:
        return "Completed";
    case APPT_CANCELLED:
        return "Cancelled";
    default:
        return "Unknown";
    }
}

void displayAppointment(const Appointment *a)
{
    if (a == NULL)
    {
        printf("Appointment not found.\n");
        return;
    }

    printf("------------------------------------------\n");
    printf("Appointment ID : %d\n", a->id);
    printf("Patient ID     : %d\n", a->patientId);
    printf("Doctor ID      : %d\n", a->doctorId);
    printf("Date           : %s\n", a->date);
    printf("Time           : %s\n", a->time);
    printf("Status         : %s\n", appointmentStatusToString(a->status));
    printf("------------------------------------------\n");
}

void displayAllAppointments(Appointment *head)
{
    if (head == NULL)
    {
        printf("No appointment records found.\n");
        return;
    }

    printf("%-5s %-10s %-10s %-12s %-6s %-12s\n",
           "ID", "PatientID", "DoctorID", "Date", "Time", "Status");
    printf("-----------------------------------------------------------------\n");

    for (Appointment *curr = head; curr != NULL; curr = curr->next)
    {
        printf("%-5d %-10d %-10d %-12s %-6s %-12s\n",
               curr->id, curr->patientId, curr->doctorId,
               curr->date, curr->time,
               appointmentStatusToString(curr->status));
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/billing.h"

Bill *loadBills(void)
{
    FILE *fp = fopen(BILLING_FILE, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    Bill *head = NULL;
    Bill *tail = NULL;
    Bill temp;

    while (fread(&temp, sizeof(Bill), 1, fp) == 1)
    {
        Bill *node = (Bill *)malloc(sizeof(Bill));
        if (node == NULL)
        {
            printf("Memory allocation failed while loading bills.\n");
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

void saveBills(Bill *head)
{
    FILE *fp = fopen(BILLING_FILE, "wb");
    if (fp == NULL)
    {
        printf("Error: could not open %s for writing.\n", BILLING_FILE);
        return;
    }

    for (Bill *curr = head; curr != NULL; curr = curr->next)
    {
        fwrite(curr, sizeof(Bill), 1, fp);
    }

    fclose(fp);
}

void freeBills(Bill *head)
{
    Bill *curr = head;
    while (curr != NULL)
    {
        Bill *next = curr->next;
        free(curr);
        curr = next;
    }
}

Bill *addBill(Bill *head, int patientId, float consultationFee, float medicineCost, float otherCharges, const char *date)
{
    Bill *node = (Bill *)malloc(sizeof(Bill));
    if (node == NULL)
    {
        printf("Memory allocation failed while adding bill.\n");
        return head;
    }

    int maxId = 0;
    for (Bill *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id > maxId)
            maxId = curr->id;
    }
    node->id = maxId + 1;

    node->patientId = patientId;
    node->consultationFee = consultationFee;
    node->medicineCost = medicineCost;
    node->otherCharges = otherCharges;

    /* Total is always derived, never entered directly, so it
     * can never drift out of sync with its three components. */
    node->totalAmount = consultationFee + medicineCost + otherCharges;

    strncpy(node->date, date, BILL_DATE_LEN - 1);
    node->date[BILL_DATE_LEN - 1] = '\0';

    node->next = NULL;

    if (head == NULL)
    {
        return node;
    }

    Bill *curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }
    curr->next = node;

    return head;
}

Bill *deleteBill(Bill *head, int id)
{
    Bill *curr = head;
    Bill *prev = NULL;

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

Bill *findBillById(Bill *head, int id)
{
    for (Bill *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->id == id)
        {
            return curr;
        }
    }
    return NULL;
}

void displayBillsByPatientId(Bill *head, int patientId)
{
    /* LINEAR search/filter, same pattern as appointment.c's
     * per-patient and per-doctor filters. */
    int found = 0;
    float grandTotal = 0.0f;

    printf("%-5s %-10s %-12s %-10s %-10s %-10s %-10s\n", "ID", "PatientID", "Date", "Consult", "Medicine", "Other", "Total");
    printf("---------------------------------------------------------------------------\n");

    for (Bill *curr = head; curr != NULL; curr = curr->next)
    {
        if (curr->patientId == patientId)
        {
            printf("%-5d %-10d %-12s %-10.2f %-10.2f %-10.2f %-10.2f\n",
                   curr->id, curr->patientId, curr->date,
                   curr->consultationFee, curr->medicineCost,
                   curr->otherCharges, curr->totalAmount);
            grandTotal += curr->totalAmount;
            found = 1;
        }
    }

    if (!found)
    {
        printf("No billing records found for patient ID %d.\n", patientId);
    }
    else
    {
        printf("---------------------------------------------------------------------------\n");
        printf("Grand total across all bills: %.2f\n", grandTotal);
    }
}

void displayBillReceipt(const Bill *b)
{
    if (b == NULL)
    {
        printf("Bill not found.\n");
        return;
    }

    printf("==========================================\n");
    printf("        HOSPITAL BILLING RECEIPT\n");
    printf("==========================================\n");
    printf("Bill ID          : %d\n", b->id);
    printf("Patient ID       : %d\n", b->patientId);
    printf("Date             : %s\n", b->date);
    printf("------------------------------------------\n");
    printf("Consultation Fee : %10.2f\n", b->consultationFee);
    printf("Medicine Cost    : %10.2f\n", b->medicineCost);
    printf("Other Charges    : %10.2f\n", b->otherCharges);
    printf("------------------------------------------\n");
    printf("TOTAL AMOUNT     : %10.2f\n", b->totalAmount);
    printf("==========================================\n");
}

void displayAllBills(Bill *head)
{
    if (head == NULL)
    {
        printf("No billing records found.\n");
        return;
    }

    printf("%-5s %-10s %-12s %-10s %-10s %-10s %-10s\n", "ID", "PatientID", "Date", "Consult", "Medicine", "Other", "Total");
    printf("---------------------------------------------------------------------------\n");

    for (Bill *curr = head; curr != NULL; curr = curr->next)
    {
        printf("%-5d %-10d %-12s %-10.2f %-10.2f %-10.2f %-10.2f\n",
               curr->id, curr->patientId, curr->date,
               curr->consultationFee, curr->medicineCost,
               curr->otherCharges, curr->totalAmount);
    }
}
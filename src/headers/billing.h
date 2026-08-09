#ifndef BILLING_H
#define BILLING_H

#define BILL_DATE_LEN 11   /* "DD-MM-YYYY" + '\0' */
#define BILLING_FILE "data/billing.dat"

typedef struct Bill {
    int id;
    int patientId;
    float consultationFee;
    float medicineCost;
    float otherCharges;
    float totalAmount;     /* always = sum of the three above, computed automatically */
    char date[BILL_DATE_LEN];
    struct Bill *next;
} Bill;

/* ---- Lifecycle ---- */
Bill* loadBills(void);
void saveBills(Bill *head);
void freeBills(Bill *head);

/* ---- CRUD ---- */
Bill* addBill(Bill *head, int patientId, float consultationFee, float medicineCost, float otherCharges, const char *date);
Bill* deleteBill(Bill *head, int id);

/* ---- Search ---- */
Bill* findBillById(Bill *head, int id);
void displayBillsByPatientId(Bill *head, int patientId);

/* ---- Reporting ---- */
void displayAllBills(Bill *head);
void displayBillReceipt(const Bill *b);

#endif
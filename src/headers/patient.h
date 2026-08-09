#ifndef PATIENT_H
#define PATIENT_H

#define PATIENT_NAME_LEN 100
#define PATIENT_GENDER_LEN 10
#define PATIENT_PHONE_LEN 11
#define PATIENT_ADDRESS_LEN 100
#define PATIENT_DIAGNOSIS_LEN 200
#define PATIENTS_FILE "data/patients.dat"

typedef struct Patient
{
    int id;
    char name[PATIENT_NAME_LEN];
    int age;
    char gender[PATIENT_GENDER_LEN];
    char phone[PATIENT_PHONE_LEN];
    char address[PATIENT_ADDRESS_LEN];
    char diagnosis[PATIENT_DIAGNOSIS_LEN];
    struct Patient *next;
} Patient;

/* ---- Lifecycle ---- */
Patient *loadPatients(void);
void savePatients(Patient *head);
void freePatients(Patient *head);

/* ---- CRUD ---- */
Patient *addPatient(Patient *head, const char *name, int age, const char *gender, const char *phone, const char *address, const char *diagnosis);
int updatePatient(Patient *head, int id, const char *name, int age, const char *gender, const char *phone, const char *address, const char *diagnosis);
Patient *deletePatient(Patient *head, int id);

/* ---- Search ---- */
Patient *findPatientById(Patient *head, int id);
Patient *searchPatientByName(Patient *head, const char *namePart);
Patient *searchPatientByIdBinary(Patient *head, int id, int *outComparisons);

/* ---- Reporting ---- */
void displayAllPatients(Patient *head);
void displayPatient(const Patient *p);

#endif
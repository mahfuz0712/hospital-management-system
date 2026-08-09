#ifndef DOCTOR_H
#define DOCTOR_H

#define DOCTOR_NAME_LEN 100
#define DOCTOR_SPECIALIZATION_LEN 50
#define DOCTOR_PHONE_LEN 20
#define DOCTORS_FILE "data/doctors.dat"

typedef enum {
    DOCTOR_BUSY = 0,
    DOCTOR_AVAILABLE = 1
} Availability;

typedef struct Doctor {
    int id;
    char name[DOCTOR_NAME_LEN];
    char specialization[DOCTOR_SPECIALIZATION_LEN];
    char phone[DOCTOR_PHONE_LEN];
    Availability availability;
    struct Doctor *next;
} Doctor;

/* ---- Lifecycle ---- */
Doctor* loadDoctors(void);
void saveDoctors(Doctor *head);
void freeDoctors(Doctor *head);

/* ---- CRUD ---- */
Doctor* addDoctor(Doctor *head, const char *name, const char *specialization, const char *phone, Availability availability);
int updateDoctor(Doctor *head, int id, const char *name, const char *specialization, const char *phone, Availability availability);
Doctor* deleteDoctor(Doctor *head, int id);

/* Toggles a doctor's availability without needing to re-enter
 * every other field. Returns 1 if found and toggled, 0 if not. */
int setDoctorAvailability(Doctor *head, int id, Availability availability);

/* ---- Search ---- */

/* Linear search by id. */
Doctor* findDoctorById(Doctor *head, int id);
Doctor* searchDoctorByName(Doctor *head, const char *namePart);
Doctor* findAvailableDoctorBySpecialization(Doctor *head, const char *specialization);

/* ---- Reporting ---- */
void displayAllDoctors(Doctor *head);
void displayDoctor(const Doctor *d);
const char* availabilityToString(Availability a);

#endif
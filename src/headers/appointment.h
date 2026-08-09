/* ============================================================
 * appointment.h
 * ------------------------------------------------------------
 * Appointment records, linking a Patient (by id) to a Doctor
 * (by id), stored as a singly linked list and persisted to
 * APPOINTMENTS_FILE in binary form.
 *
 * Search strategy mirrors patient.h:
 *  - searchAppointmentsByPatientId(): LINEAR search/filter on
 *    the linked list. Used for "show this patient's history".
 *  - searchAppointmentsByDoctorId(): LINEAR search/filter.
 *    Used for the doctor role's "show me only MY patients" view.
 *  - searchAppointmentByDateBinary(): BINARY search. As with
 *    patient.c, the list is first copied into a temporary array
 *    sorted by date (as YYYYMMDD integers for correct
 *    chronological ordering), then a real binary search runs
 *    on that array.
 * ============================================================ */

#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#define APPOINTMENT_DATE_LEN 11   /* "DD-MM-YYYY" + '\0' */
#define APPOINTMENT_TIME_LEN 6    /* "HH:MM" + '\0' */
#define APPOINTMENTS_FILE "data/appointments.dat"

typedef enum {
    APPT_SCHEDULED = 0,
    APPT_COMPLETED = 1,
    APPT_CANCELLED = 2
} AppointmentStatus;

typedef struct Appointment {
    int id;
    int patientId;
    int doctorId;
    char date[APPOINTMENT_DATE_LEN];
    char time[APPOINTMENT_TIME_LEN];
    AppointmentStatus status;
    struct Appointment *next;
} Appointment;

/* ---- Lifecycle ---- */
Appointment* loadAppointments(void);
void saveAppointments(Appointment *head);
void freeAppointments(Appointment *head);

/* ---- CRUD ---- */
Appointment* addAppointment(Appointment *head, int patientId, int doctorId,
                             const char *date, const char *time);

Appointment* deleteAppointment(Appointment *head, int id);

/* Updates status only (e.g. mark Completed or Cancelled).
 * Returns 1 if found and updated, 0 otherwise. */
int updateAppointmentStatus(Appointment *head, int id, AppointmentStatus status);

/* ---- Search ---- */

Appointment* findAppointmentById(Appointment *head, int id);

/* LINEAR search/filter: checks whether the given doctor already
 * has a SCHEDULED appointment at the exact same date+time. This
 * is the double-booking guard — it is called BEFORE a new
 * appointment is created, so two different patients can never
 * be booked with the same doctor at the same date and time.
 * Cancelled appointments do NOT count as occupying the slot
 * (a cancelled slot is free to be rebooked); only APPT_SCHEDULED
 * (and, deliberately, APPT_COMPLETED — a doctor's history is
 * still "occupied" at that timestamp) block the slot.
 * Returns 1 if the slot is already taken, 0 if it's free. */
int isDoctorAvailableAt(Appointment *head, int doctorId,
                         const char *date, const char *time);

/* LINEAR search/filter: prints every appointment belonging to
 * the given patient id. Used for patient history lookups. */
void displayAppointmentsByPatientId(Appointment *head, int patientId);

/* LINEAR search/filter: prints every appointment belonging to
 * the given doctor id. This is the function that powers the
 * Doctor role's restricted view — a doctor only ever sees
 * appointments (and therefore patients) where doctorId matches
 * their own linked doctor record. */
void displayAppointmentsByDoctorId(Appointment *head, int doctorId);

/* BINARY search by exact date (DD-MM-YYYY). Builds a temporary
 * array sorted chronologically, then binary searches it.
 * Returns the first matching Appointment*, or NULL.
 * outComparisons receives how many comparisons binary search
 * needed (useful to display/explain efficiency). */
Appointment* searchAppointmentByDateBinary(Appointment *head, const char *date,
                                            int *outComparisons);

/* ---- Reporting ---- */
void displayAllAppointments(Appointment *head);
void displayAppointment(const Appointment *a);
const char* appointmentStatusToString(AppointmentStatus s);

#endif
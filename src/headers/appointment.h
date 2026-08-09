#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#define APPOINTMENT_DATE_LEN 11 /* "DD-MM-YYYY" + '\0' */
#define APPOINTMENT_TIME_LEN 6  /* "HH:MM" + '\0' */
#define APPOINTMENTS_FILE "data/appointments.dat"

typedef enum
{
    APPT_SCHEDULED = 0,
    APPT_COMPLETED = 1,
    APPT_CANCELLED = 2
} AppointmentStatus;

typedef struct Appointment
{
    int id;
    int patientId;
    int doctorId;
    char date[APPOINTMENT_DATE_LEN];
    char time[APPOINTMENT_TIME_LEN];
    AppointmentStatus status;
    struct Appointment *next;
} Appointment;

/* ---- Lifecycle ---- */
Appointment *loadAppointments(void);
void saveAppointments(Appointment *head);
void freeAppointments(Appointment *head);

/* ---- CRUD ---- */
Appointment *addAppointment(Appointment *head, int patientId, int doctorId,
                            const char *date, const char *time);

Appointment *deleteAppointment(Appointment *head, int id);

int updateAppointmentStatus(Appointment *head, int id, AppointmentStatus status);

/* ---- Search ---- */

Appointment *findAppointmentById(Appointment *head, int id);

int isDoctorAvailableAt(Appointment *head, int doctorId,
                        const char *date, const char *time);

void displayAppointmentsByPatientId(Appointment *head, int patientId);

void displayAppointmentsByDoctorId(Appointment *head, int doctorId);

Appointment *searchAppointmentByDateBinary(Appointment *head, const char *date,
                                           int *outComparisons);

/* ---- Reporting ---- */
void displayAllAppointments(Appointment *head);
void displayAppointment(const Appointment *a);
const char *appointmentStatusToString(AppointmentStatus s);

#endif